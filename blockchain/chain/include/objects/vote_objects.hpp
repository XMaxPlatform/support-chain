/**
 *  @file
 *  @copyright defined in xmax/LICENSE.txt
 */
#pragma once

#include <blockchain_types.hpp>
#include <basemisc.hpp>

#include "multi_index_includes.hpp"


namespace Xmaxplatform { namespace Chain {

    class voter_info_object : public Basechain::object<vote_info_object_type, voter_info_object> {
        OBJECT_CCTOR(voter_info_object)

        typedef std::vector<account_name> producer_list;

        id_type         id;
        account_name	owner;
        account_name	proxy;
        uint32	        is_proxy;

        uint128	        proxied_votes;
        producer_list	producers;

        share_type	    staked;
        share_type	    unstaking;
        share_type	    unstake_per_week;

        uint32	        deferred_trx_id;
        time	        last_update;
        time	        last_unstake_time;

    };


    class producer_info_object : public Basechain::object<producer_info_object_type, producer_info_object> {
        OBJECT_CCTOR(producer_info_object)

        id_type             id;
        account_name        owner;
        uint128             total_votes;
        //blockchain_configuration prefs;

        public_key_type     producer_key;
        share_type          per_block_payments;
        time                last_rewards_claim;
        time                time_became_active;
        time                last_produced_block_time;

    public:
        bool active() const { return producer_key != empty_public_key; }

        std::pair<uint128, id_type> get_vote_order() const    { return {total_votes, id}; }
        share_type  get_votes() const { return (share_type)total_votes; }
    };
    using boost::multi_index::const_mem_fun;
    struct by_owner;
    struct by_votes;

    using voter_info_id_type = voter_info_object::id_type;

    using voter_info_index = Basechain::shared_multi_index_container<
            voter_info_object,
            indexed_by<
                    ordered_unique<tag<by_id>, member<voter_info_object, voter_info_object::id_type, &voter_info_object::id>>,
                    ordered_unique<tag<by_owner>, member<voter_info_object, account_name, &voter_info_object::owner> >
            >
    >;

    using producer_info_id_type = producer_info_object::id_type;

    using producer_info_index = Basechain::shared_multi_index_container<
            producer_info_object,
            indexed_by<
                    ordered_unique<tag<by_id>,
                            member<producer_info_object, producer_info_object::id_type, &producer_info_object::id>
                    >,
                    ordered_unique<tag<by_owner>,
                            member<producer_info_object, account_name, &producer_info_object::owner>
                    >,
                    ordered_non_unique<tag<by_votes>,
                            const_mem_fun<producer_info_object, std::pair<uint128, producer_info_object::id_type>, &producer_info_object::get_vote_order>,
                            std::greater< std::pair<uint128, producer_info_object::id_type> >
                    >
            >
    >;


} }// Xmaxplatform::chain

BASECHAIN_SET_INDEX_TYPE(Xmaxplatform::Chain::voter_info_object, Xmaxplatform::Chain::voter_info_index)

BASECHAIN_SET_INDEX_TYPE(Xmaxplatform::Chain::producer_info_object, Xmaxplatform::Chain::producer_info_index)

FC_REFLECT(Basechain::oid<Xmaxplatform::Chain::voter_info_object>, (_id))

FC_REFLECT(Xmaxplatform::Chain::voter_info_object, (id)(owner)(proxy)(is_proxy)(proxied_votes)(producers)(staked)(unstaking)(unstake_per_week)(deferred_trx_id)(last_update)(last_unstake_time))

FC_REFLECT(Basechain::oid<Xmaxplatform::Chain::producer_info_object>, (_id))

FC_REFLECT(Xmaxplatform::Chain::producer_info_object, (owner)(total_votes)(producer_key)(per_block_payments)(last_rewards_claim)(time_became_active)(last_produced_block_time))



