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

#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/algorithm_ext.hpp>

namespace Xmaxplatform {
namespace Native_contract {

    using namespace Xmaxplatform::Chain;

    typedef mutable_db_table <voter_info_object, by_owner> voters_table;

    typedef mutable_db_table <builder_info_object, by_owner> builders_table;


    void xmax_voting::increase_votes(message_context_xmax &context, account_name acnt, share_type amount)
    {

        voters_table voters_tbl(context.mutable_db);

        const voter_info_object* voter = voters_tbl.find(acnt);

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

        const std::vector<account_name> *builders = nullptr;
        if (voter->proxy) {
            auto proxy = voters_tbl.find(voter->proxy);
            XMAX_ASSERT(proxy != nullptr, message_validate_exception, "selected proxy not found"); //data corruption
            voters_tbl.modify(proxy, [&](voter_info_object &a) {
                a.proxied_votes += amount;
            });
            if (proxy->is_proxy) { //only if proxy is still active. if proxy has been unregistered, we update proxied_votes, but don't propagate to builders
                builders = &proxy->builders;
            }
        } else {
            builders = &voter->builders;
        }

        if (builders) {
            builders_table builders_tbl(context.mutable_db);
            for (auto p : *builders) {
                auto prod = builders_tbl.find(p);
                XMAX_ASSERT(prod != nullptr, message_validate_exception,
                            "never existed builder"); //data corruption
                builders_tbl.modify(prod, [&](auto &v) {
                    v.total_votes += amount;
                });
            }
        }
    }

    void xmax_voting::decrease_votes(message_context_xmax &context, account_name acnt, share_type amount)
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

            const std::vector<account_name> *builders = nullptr;
            if (voter->proxy) {
                auto proxy = voters_tbl.find(voter->proxy);
                voters_tbl.modify(proxy, [&](voter_info_object &a) {
                    a.proxied_votes -= (uint128) amount;
                });
                if (proxy->is_proxy) { //only if proxy is still active. if proxy has been unregistered, we update proxied_votes, but don't propagate to builders
                    builders = &proxy->builders;
                }
            } else {
                builders = &voter->builders;
            }

            if (builders) {
                builders_table builders_tbl(context.mutable_db);
                for (auto p : *builders) {
                    auto prod = builders_tbl.find(p);
                    XMAX_ASSERT(prod != nullptr, message_validate_exception,
                                "never existed builder"); //data corruption
                    builders_tbl.modify(prod, [&](auto &v) {
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

    void xmax_voting::vote_builder(message_context_xmax &context)
    {
        auto vp = context.msg.as<votebuilder>();

        context.require_authorization(vp.voter);

        if (vp.proxy.good()) {
            XMAX_ASSERT(vp.builders.size() == 0, message_validate_exception,
                        "cannot vote for builders and proxy at same time");
            context.require_recipient(vp.proxy);
        } else {
            XMAX_ASSERT(vp.builders.size() <= 30, message_validate_exception,
                        "attempt to vote for too many builders");
            for (size_t i = 1; i < vp.builders.size(); ++i) {
                XMAX_ASSERT(vp.builders[i - 1] < vp.builders[i], message_validate_exception,
                            "builder votes must be unique and sorted");
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


        //find old builders, update old proxy if needed
        const std::vector<account_name> *old_builders = nullptr;
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
                0) { //if proxy stoped being proxy, the votes were already taken back from builders by on( const unregister_proxy& )
                old_builders = &old_proxy->builders;
            }
        } else {
            old_builders = &voter->builders;
        }


        //find new builders, update new proxy if needed
        const std::vector<account_name> *new_builders = nullptr;
        if (vp.proxy) {
            auto new_proxy = voters_tbl.find(vp.proxy);
            XMAX_ASSERT(new_proxy != nullptr && new_proxy->is_proxy != 0, message_validate_exception,
                        "proxy not found");
            voters_tbl.modify(new_proxy, [&](auto &a) {
                a.proxied_votes += voter->staked;
            });
            new_builders = &new_proxy->builders;
        } else {
            new_builders = &vp.builders;
        }


        builders_table builders_tbl(db);

        uint128 votes = (uint128) voter->staked;
        if (voter->is_proxy) {
            votes += voter->proxied_votes;
        }

        if (old_builders) { //old_builders == nullptr if proxy has stopped being a proxy and votes were taken back from the builders at that moment
            //revoke votes only from no longer elected
            std::vector<account_name> revoked(old_builders->size());
            auto end_it = std::set_difference(old_builders->begin(), old_builders->end(), new_builders->begin(),
                                              new_builders->end(), revoked.begin());
            for (auto it = revoked.begin(); it != end_it; ++it) {
                auto prod = builders_tbl.find(*it);
                XMAX_ASSERT(prod != nullptr, message_validate_exception,
                            "never existed builder"); //data corruption
                builders_tbl.modify(prod, [&](auto &pi) {
                    pi.total_votes -= votes;
                });
            }
        }

        //update newly elected
        std::vector<account_name> elected(new_builders->size());
        auto end_it = elected.begin();
        if (old_builders) {
            end_it = std::set_difference(new_builders->begin(), new_builders->end(), old_builders->begin(),
                                         old_builders->end(), elected.begin());
        } else {
            end_it = std::copy(new_builders->begin(), new_builders->end(), elected.begin());
        }
        for (auto it = elected.begin(); it != end_it; ++it) {
            auto prod = builders_tbl.find(*it);
            XMAX_ASSERT(prod != nullptr, message_validate_exception, "builder is not registered");
            if (vp.proxy.empty()) { //direct xmax_voting, in case of proxy xmax_voting update total_votes even for inactive builders
                XMAX_ASSERT(prod->active(), message_validate_exception, "builder is not currently registered");
            }
            builders_tbl.modify(prod, [&](auto &pi) {
                pi.total_votes += votes;
            });
        }

        // save new values to the account itself
        voters_tbl.modify(voter, [&](voter_info_object &a) {
            a.proxy = vp.proxy;
            a.last_update = context.current_time();
            a.builders = vp.builders;
        });

    }

    void xmax_voting::reg_builder(message_context_xmax &context)
    {
        auto reg = context.msg.as<regbuilder>();

        context.require_authorization(reg.builder);

        builders_table builders_tbl(context.mutable_db);
        auto prod = builders_tbl.find(reg.builder);

        if (prod != nullptr) {
            builders_tbl.modify(prod, [&](builder_info_object &info) {
                //info.prefs = reg.prefs;
                info.builder_key = reg.builder_key;
            });
        } else {
            builders_tbl.emplace([&](builder_info_object &info) {
                info.owner = reg.builder;
                info.total_votes = 0;
                //info.prefs = reg.prefs;
                info.builder_key = reg.builder_key;
            });
        }
    }

    void xmax_voting::unreg_builder(message_context_xmax &context)
    {
        auto unreg = context.msg.as<unregbuilder>();

        context.require_authorization(unreg.builder);

        builders_table builders_tbl(context.mutable_db);
        auto prod = builders_tbl.find(unreg.builder);
        XMAX_ASSERT(prod != nullptr, message_validate_exception, "builder not found");

        builders_tbl.modify(prod, [&](builder_info_object &info) {
            info.builder_key = empty_public_key;
        });
    }

    void xmax_voting::reg_proxy(message_context_xmax &context)
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
                builders_table builders_tbl(context.mutable_db);
                for (auto p : proxy->builders) {
                    auto prod = builders_tbl.find(p);
                    XMAX_ASSERT(prod != nullptr, message_validate_exception,
                                "never existed builder"); //data corruption
                    builders_tbl.modify(prod, [&](auto &pi) { pi.total_votes += proxy->proxied_votes; });
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

    void xmax_voting::unreg_proxy(message_context_xmax &context)
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
            builders_table builders_tbl(context.mutable_db);
            for (auto p : proxy->builders) {
                auto prod = builders_tbl.find(p);
                XMAX_ASSERT(prod != nullptr, message_validate_exception,
                            "never existed builder"); //data corruption
                builders_tbl.modify(prod, [&](auto &pi) { pi.total_votes -= proxy->proxied_votes; });
            }
        }
    }

	xmax_builders xmax_voting::next_round(Basechain::database& db)
	{
		xmax_builders round;

		builders_table builders_tbl(db);

		const auto& AllProducersByVotes = builders_tbl.get_index<by_votes>();


		auto FilterRetiredProducers = boost::adaptors::filtered([&db](const builder_info_object& info) {
			return info.builder_key != empty_public_key;
		});

		auto ProducerObjectToName = boost::adaptors::transformed([](const builder_info_object& p) { return p.owner; });


		auto ActiveProducersByVotes = AllProducersByVotes | FilterRetiredProducers;

		int count = 0;
		for (auto it : ActiveProducersByVotes)
		{
			round.push_back(it.owner);
			++count;
			if (count >= Config::blocks_per_round)
			{
				break;
			}
		}

		if (round.empty())
		{
			round.push_back(Config::xmax_contract_name);
		}
		else
		{
			std::sort(round.begin(), round.end(), [](const account_name& r, const account_name& l) -> bool
			{
				return r < l;
			});
		}

		return round;
	}

}}