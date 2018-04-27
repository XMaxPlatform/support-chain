/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once

#include <blockchain_types.hpp>
#include <basemisc.hpp>

#include "multi_index_includes.hpp"


namespace Xmaxplatform { namespace Chain {

    class voter_info_object : public Basechain::object<vote_info_object_type, voter_info_object> {
        OBJECT_CCTOR(voter_info_object)

        typedef std::vector<account_name> builder_list;

        id_type         id;
        account_name	owner;
        account_name	proxy;
        uint32	        is_proxy;

        uint128	        proxied_votes;
        builder_list	builders;

        share_type	    staked;
        share_type	    unstaking;
        share_type	    unstake_per_week;

        uint32	        deferred_trx_id;
        time	        last_update;
        time	        last_unstake_time;

    };


    class builder_info_object : public Basechain::object<builder_info_object_type, builder_info_object> {
        OBJECT_CCTOR(builder_info_object)

        id_type             id;
        account_name        owner;
        uint128             total_votes;
        //blockchain_configuration prefs;

        public_key_type     builder_key;

    public:
        bool is_actived() const { return builder_key != empty_public_key; }

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

    using builder_info_id_type = builder_info_object::id_type;

    using builder_info_index = Basechain::shared_multi_index_container<
            builder_info_object,
            indexed_by<
                    ordered_unique<tag<by_id>,
                            member<builder_info_object, builder_info_object::id_type, &builder_info_object::id>
                    >,
                    ordered_unique<tag<by_owner>,
                            member<builder_info_object, account_name, &builder_info_object::owner>
                    >,
                    ordered_non_unique<tag<by_votes>,
                            const_mem_fun<builder_info_object, std::pair<uint128, builder_info_object::id_type>, &builder_info_object::get_vote_order>,
                            std::greater< std::pair<uint128, builder_info_object::id_type> >
                    >
            >
    >;


} }// Xmaxplatform::chain

BASECHAIN_SET_INDEX_TYPE(Xmaxplatform::Chain::voter_info_object, Xmaxplatform::Chain::voter_info_index)

BASECHAIN_SET_INDEX_TYPE(Xmaxplatform::Chain::builder_info_object, Xmaxplatform::Chain::builder_info_index)

FC_REFLECT(Basechain::oid<Xmaxplatform::Chain::voter_info_object>, (_id))

FC_REFLECT(Xmaxplatform::Chain::voter_info_object, (id)(owner)(proxy)(is_proxy)(proxied_votes)(builders)(staked)(unstaking)(unstake_per_week)(deferred_trx_id)(last_update)(last_unstake_time))

FC_REFLECT(Basechain::oid<Xmaxplatform::Chain::builder_info_object>, (_id))

FC_REFLECT(Xmaxplatform::Chain::builder_info_object, (owner)(total_votes)(builder_key)(last_build_time))
                                                                                    //(per_block_payments)(last_rewards_claim)(time_became_active)


