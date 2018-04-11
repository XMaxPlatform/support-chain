/**
 *  @file
 *  @copyright defined in xmax/LICENSE.txt
 */
#include <xmax_voting.hpp>


#include <chain_xmax.hpp>
#include <message_context_xmax.hpp>
#include <message_xmax.hpp>
#include <blockchain_exceptions.hpp>

#include <objects/account_object.hpp>
#include <objects/xmx_token_object.hpp>
#include <objects/vote_objects.hpp>
#include <objects/chain_object_table.hpp>

#include <abi_serializer.hpp>

namespace Xmaxplatform {
namespace Native_contract {

    using namespace Xmaxplatform::Chain;

    typedef db_object_table <voter_info_object, by_owner> voters_table;

    typedef db_object_table <producer_info_object, by_owner> producers_table;


    void voting::increase_voting_power(message_context_xmax &context, account_name acnt, share_type amount)
    {

        voters_table voters_tbl(context.mutable_db);

        auto voter = voters_tbl.find(acnt);

        if (voter == nullptr) {
            voter = voters_tbl.emplace([&](voter_info_object &a) {
                a.owner = acnt;
                a.last_update = context.current_time();
                a.staked = amount;
            });
        } else {
            voters_tbl.modify(voter, [&](auto &av) {
                av.last_update = context.current_time();
                av.staked += amount;
            });
        }

        const std::vector<account_name> *producers = nullptr;
        if (voter->proxy) {
            auto proxy = voters_tbl.find(voter->proxy);
            XMAX_ASSERT(proxy != nullptr, message_validate_exception, "selected proxy not found"); //data corruption
            voters_tbl.modify(proxy, [&](voter_info_object &a) {
                a.proxied_votes += amount;
            });
            if (proxy->is_proxy) { //only if proxy is still active. if proxy has been unregistered, we update proxied_votes, but don't propagate to producers
                producers = &proxy->producers;
            }
        } else {
            producers = &voter->producers;
        }

        if (producers) {
            producers_table producers_tbl(context.mutable_db);
            for (auto p : *producers) {
                auto prod = producers_tbl.find(p);
                XMAX_ASSERT(prod != nullptr, message_validate_exception,
                            "never existed producer"); //data corruption
                producers_tbl.modify(prod, [&](auto &v) {
                    v.total_votes += amount;
                });
            }
        }
    }

    void voting::decrease_voting_power(message_context_xmax &context, account_name acnt, share_type amount)
    {

        context.require_authorization(acnt);

        voters_table voters_tbl(context.mutable_db);
        auto voter = voters_tbl.find(acnt);
        XMAX_ASSERT(voter != nullptr, message_validate_exception, "stake not found");

        if (0 < amount) {
            XMAX_ASSERT(amount <= voter->staked, message_validate_exception,
                        "cannot unstake more than total stake amount");
            voters_tbl.modify(voter, [&](voter_info_object &a) {
                a.staked -= amount;
                a.last_update = context.current_time();
            });

            const std::vector<account_name> *producers = nullptr;
            if (voter->proxy) {
                auto proxy = voters_tbl.find(voter->proxy);
                voters_tbl.modify(proxy, [&](voter_info_object &a) {
                    a.proxied_votes -= (uint128) amount;
                });
                if (proxy->is_proxy) { //only if proxy is still active. if proxy has been unregistered, we update proxied_votes, but don't propagate to producers
                    producers = &proxy->producers;
                }
            } else {
                producers = &voter->producers;
            }

            if (producers) {
                producers_table producers_tbl(context.mutable_db);
                for (auto p : *producers) {
                    auto prod = producers_tbl.find(p);
                    XMAX_ASSERT(prod != nullptr, message_validate_exception,
                                "never existed producer"); //data corruption
                    producers_tbl.modify(prod, [&](auto &v) {
                        v.total_votes -= amount;
                    });
                }
            }
        } else {
            if (voter->deferred_trx_id) {
                //XXX cancel_deferred_transaction(voter->deferred_trx_id);
            }
            voters_tbl.modify(voter, [&](voter_info_object &a) {
                a.staked += a.unstaking;
                a.unstaking = 0;
                a.unstake_per_week = 0;
                a.deferred_trx_id = 0;
                a.last_update = context.current_time();
            });
        }
    }

    void voting::vote_producer(message_context_xmax &context)
    {
        auto vp = context.msg.as<voteproducer>();

        context.require_authorization(vp.voter);

        if (vp.proxy.good()) {
            XMAX_ASSERT(vp.producers.size() == 0, message_validate_exception,
                        "cannot vote for producers and proxy at same time");
            context.require_recipient(vp.proxy);
        } else {
            XMAX_ASSERT(vp.producers.size() <= 30, message_validate_exception,
                        "attempt to vote for too many producers");
            for (size_t i = 1; i < vp.producers.size(); ++i) {
                XMAX_ASSERT(vp.producers[i - 1] < vp.producers[i], message_validate_exception,
                            "producer votes must be unique and sorted");
            }
        }

        auto &db = context.mutable_db;

        voters_table voters_tbl(db);

        const voter_info_object *voter = voters_tbl.find(vp.voter);

        XMAX_ASSERT(voter != nullptr && (0 < voter->staked || (voter->is_proxy && 0 < voter->proxied_votes)),
                    message_validate_exception, "no stake to vote");
        if (voter->is_proxy != 0) {
            XMAX_ASSERT(vp.proxy.empty(), message_validate_exception,
                        "account registered as a proxy is not allowed to use a proxy");
        }


        //find old producers, update old proxy if needed
        const std::vector<account_name> *old_producers = nullptr;
        if (voter->proxy.good()) {
            if (voter->proxy == vp.proxy) {
                return; // nothing changed
            }
            const voter_info_object *old_proxy = voters_tbl.find(voter->proxy);
            XMAX_ASSERT(old_proxy != nullptr, message_validate_exception, "old proxy not found"); //data corruption

            voters_tbl.modify(old_proxy, [&](auto &a) {
                a.proxied_votes -= voter->staked;
            });

            if (old_proxy->is_proxy !=
                0) { //if proxy stoped being proxy, the votes were already taken back from producers by on( const unregister_proxy& )
                old_producers = &old_proxy->producers;
            }
        } else {
            old_producers = &voter->producers;
        }


        //find new producers, update new proxy if needed
        const std::vector<account_name> *new_producers = nullptr;
        if (vp.proxy) {
            auto new_proxy = voters_tbl.find(vp.proxy);
            XMAX_ASSERT(new_proxy != nullptr && new_proxy->is_proxy != 0, message_validate_exception,
                        "proxy not found");
            voters_tbl.modify(new_proxy, [&](auto &a) {
                a.proxied_votes += voter->staked;
            });
            new_producers = &new_proxy->producers;
        } else {
            new_producers = &vp.producers;
        }


        producers_table producers_tbl(db);

        uint128 votes = (uint128) voter->staked;
        if (voter->is_proxy) {
            votes += voter->proxied_votes;
        }

        if (old_producers) { //old_producers == nullptr if proxy has stopped being a proxy and votes were taken back from the producers at that moment
            //revoke votes only from no longer elected
            std::vector<account_name> revoked(old_producers->size());
            auto end_it = std::set_difference(old_producers->begin(), old_producers->end(), new_producers->begin(),
                                              new_producers->end(), revoked.begin());
            for (auto it = revoked.begin(); it != end_it; ++it) {
                auto prod = producers_tbl.find(*it);
                XMAX_ASSERT(prod != nullptr, message_validate_exception,
                            "never existed producer"); //data corruption
                producers_tbl.modify(prod, [&](auto &pi) {
                    pi.total_votes -= votes;
                });
            }
        }

        //update newly elected
        std::vector<account_name> elected(new_producers->size());
        auto end_it = elected.begin();
        if (old_producers) {
            end_it = std::set_difference(new_producers->begin(), new_producers->end(), old_producers->begin(),
                                         old_producers->end(), elected.begin());
        } else {
            end_it = std::copy(new_producers->begin(), new_producers->end(), elected.begin());
        }
        for (auto it = elected.begin(); it != end_it; ++it) {
            auto prod = producers_tbl.find(*it);
            XMAX_ASSERT(prod != nullptr, message_validate_exception, "producer is not registered");
            if (vp.proxy.empty()) { //direct voting, in case of proxy voting update total_votes even for inactive producers
                XMAX_ASSERT(prod->active(), message_validate_exception, "producer is not currently registered");
            }
            producers_tbl.modify(prod, [&](auto &pi) {
                pi.total_votes += votes;
            });
        }

        // save new values to the account itself
        voters_tbl.modify(voter, [&](voter_info_object &a) {
            a.proxy = vp.proxy;
            a.last_update = context.current_time();
            a.producers = vp.producers;
        });

    }

    void voting::reg_producer(message_context_xmax &context)
    {
        auto reg = context.msg.as<regproducer>();

        context.require_authorization(reg.producer);

        producers_table producers_tbl(context.mutable_db);
        auto prod = producers_tbl.find(reg.producer);

        if (prod != nullptr) {
            producers_tbl.modify(prod, [&](producer_info_object &info) {
                //info.prefs = reg.prefs;
                info.producer_key = reg.producer_key;
            });
        } else {
            producers_tbl.emplace([&](producer_info_object &info) {
                info.owner = reg.producer;
                info.total_votes = 0;
                //info.prefs = reg.prefs;
                info.producer_key = reg.producer_key;
            });
        }
    }

    void voting::unreg_prod(message_context_xmax &context)
    {
        auto unreg = context.msg.as<unregprod>();

        context.require_authorization(unreg.producer);

        producers_table producers_tbl(context.mutable_db);
        auto prod = producers_tbl.find(unreg.producer);
        XMAX_ASSERT(prod != nullptr, message_validate_exception, "producer not found");

        producers_tbl.modify(prod, [&](producer_info_object &info) {
            info.producer_key = empty_public_key;
        });
    }

    void voting::reg_proxy(message_context_xmax &context)
    {

        auto reg = context.msg.as<regproxy>();

        context.require_authorization(reg.proxy);

        voters_table voters_tbl(context.mutable_db);
        auto proxy = voters_tbl.find(reg.proxy);
        if (proxy != nullptr) {
            XMAX_ASSERT(proxy->is_proxy == 0, message_validate_exception, "account is already a proxy");
            XMAX_ASSERT(proxy->proxy == empty_name, message_validate_exception,
                        "account that uses a proxy is not allowed to become a proxy");
            voters_tbl.modify(proxy, [&](voter_info_object &a) {
                a.is_proxy = 1;
                a.last_update = context.current_time();
                //a.proxied_votes may be > 0, if the proxy has been unregistered, so we had to keep the value
            });
            if (0 < proxy->proxied_votes) {
                producers_table producers_tbl(context.mutable_db);
                for (auto p : proxy->producers) {
                    auto prod = producers_tbl.find(p);
                    XMAX_ASSERT(prod != nullptr, message_validate_exception,
                                "never existed producer"); //data corruption
                    producers_tbl.modify(prod, [&](auto &pi) { pi.total_votes += proxy->proxied_votes; });
                }
            }
        } else {
            voters_tbl.emplace([&](voter_info_object &a) {
                a.owner = reg.proxy;
                a.last_update = context.current_time();
                a.proxy = empty_name;
                a.is_proxy = 1;
                a.proxied_votes = 0;
                a.staked = 0;
            });
        }
    }

    void voting::unreg_proxy(message_context_xmax &context)
    {
        auto reg = context.msg.as<unregproxy>();

        context.require_authorization(reg.proxy);

        voters_table voters_tbl(context.mutable_db);
        auto proxy = voters_tbl.find(reg.proxy);
        XMAX_ASSERT(proxy != nullptr, message_validate_exception, "proxy not found");
        XMAX_ASSERT(proxy->is_proxy == 1, message_validate_exception, "account is not a proxy");

        voters_tbl.modify(proxy, [&](voter_info_object &a) {
            a.is_proxy = 0;
            a.last_update = context.current_time();
            //a.proxied_votes should be kept in order to be able to reenable this proxy in the future
        });

        if (0 < proxy->proxied_votes) {
            producers_table producers_tbl(context.mutable_db);
            for (auto p : proxy->producers) {
                auto prod = producers_tbl.find(p);
                XMAX_ASSERT(prod != nullptr, message_validate_exception,
                            "never existed producer"); //data corruption
                producers_tbl.modify(prod, [&](auto &pi) { pi.total_votes -= proxy->proxied_votes; });
            }
        }
    }


}}