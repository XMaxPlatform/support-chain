/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once
#include <blockchain_types.hpp>

namespace Xmaxplatform { namespace Chain {

        struct block_header
        {
            xmax_type_summary                   digest() const;
            uint32_t                      block_num() const { return num_from_id(previous) + 1; }
            static uint32_t num_from_id(const xmax_type_block_id& id);


            xmax_type_block_id                 previous;
            fc::time_point_sec            timestamp;
            xmax_type_merkle_root                 transaction_merkle_root;
            account_name                  producer;
            /**
             * The changes in the round of producers after this block
             *
             * Must be stored with keys *and* values sorted, thus this is a valid RoundChanges:
             * [["A", "X"],
             *  ["B", "Y"]]
             * ... whereas this is not:
             * [["A", "Y"],
             *  ["B", "X"]]
             * Even though the above examples are semantically equivalent (replace A and B with X and Y), only the first is
             * legal.
             */
            round_changes                  producer_changes;
        };

        struct signed_block_header : public block_header
        {
            xmax_type_block_id              id() const;
            fc::ecc::public_key        signee() const;
            void                       sign(const fc::ecc::private_key& signer);
            bool                       validate_signee(const fc::ecc::public_key& expected_signee) const;

            xmax_type_signature             producer_signature;
        };

    } } // Xmaxplatform::Chain

FC_REFLECT(Xmaxplatform::Chain::block_header, (previous)(timestamp)(transaction_merkle_root)(producer)(producer_changes))
FC_REFLECT_DERIVED(Xmaxplatform::Chain::signed_block_header, (Xmaxplatform::Chain::block_header), (producer_signature))
