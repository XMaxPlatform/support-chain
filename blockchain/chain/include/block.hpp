/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once
#include <blockchain_types.hpp>

namespace Xmaxplatform { namespace Chain {

        struct block_header
        {
            xmax_type_summary             digest() const;
            uint32_t                      block_num() const { return num_from_id(previous) + 1; }
            static uint32_t num_from_id(const xmax_type_block_id& id);


            xmax_type_block_id            previous;
            fc::time_point_sec            timestamp;
            xmax_type_merkle_root         transaction_merkle_root;
            account_name                  builder;
        };

        struct signed_block_header : public block_header
        {
            xmax_type_block_id         id() const;
            fc::ecc::public_key        get_signer_key() const;
            void                       sign(const fc::ecc::private_key& signer);
            bool                       is_signer_valid(const fc::ecc::public_key &signer_key) const;

            xmax_type_signature        builder_signature;
        };

        struct thread {

            xmax_type_summary merkle_digest() const;
        };

        struct signed_block : public signed_block_header
        {
            xmax_type_merkle_root calculate_merkle_root() const;
            vector<vector<thread>> threads;
        };

    } } // Xmaxplatform::Chain

FC_REFLECT(Xmaxplatform::Chain::block_header, (previous)(timestamp)(transaction_merkle_root)(builder))
FC_REFLECT_DERIVED(Xmaxplatform::Chain::signed_block_header, (Xmaxplatform::Chain::block_header), (builder_signature))
FC_REFLECT(Xmaxplatform::Chain::thread, )
FC_REFLECT_DERIVED(Xmaxplatform::Chain::signed_block, (Xmaxplatform::Chain::signed_block_header), (threads))
