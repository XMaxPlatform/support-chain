/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once
#include <blockchain_types.hpp>
#include <chain_time.hpp>
#include <transaction.hpp>
#include <builder_rule.hpp>

namespace Xmaxplatform { namespace Chain {

        struct block_header
        {
            xmax_type_summary             digest() const;
			xmax_type_block_num           block_num() const { return num_from_id(previous) + 1; }
            static xmax_type_block_num num_from_id(const xmax_type_block_id& id);


            xmax_type_block_id            previous;
            chain_timestamp               timestamp;
            xmax_type_merkle_root         trxs_mroot;
            account_name                  builder;
			optional<builder_rule>		  next_builders;
        };

        struct signed_block_header : public block_header
        {
            xmax_type_block_id         id() const;
            fc::ecc::public_key        get_signer_key() const;
            void                       sign(const fc::ecc::private_key& signer);
            bool                       is_signer_valid(const fc::ecc::public_key &signer_key) const;

            xmax_type_signature        builder_signature;
        };

   //     struct thread {

			//vector<processed_generated_transaction> generated_intput;
			//vector<processed_transaction> user_input;

   //         xmax_type_summary merkle_digest() const;
   //     };

		struct transaction_receipt_header
		{
			enum result_code : uint8_t
			{
				applied = 0,
				failure = 1,
			};
			uint64_t								receipt_idx;
			fc::enum_type<uint8_t, result_code>		result;
		};

		struct transaction_receipt : public transaction_receipt_header
		{
			transaction_receipt() = default;
			transaction_receipt(transaction_package trx)
				: trx(trx)
			{
			}
			transaction_receipt(xmax_type_transaction_id id)
				: trx(id)
			{
			}

			fc::static_variant<xmax_type_transaction_id, transaction_package> trx;


			xmax_type_summary cal_digest() const;
		};

        struct signed_block : public signed_block_header
        {
			std::vector<transaction_receipt> receipts;
        };

		struct signed_block_list
		{
			Chain::vector<signed_block>	blockList;
		};

		using signed_block_ptr = std::shared_ptr<signed_block>;

		struct block_confirmation_header
		{
			xmax_type_block_id		block_id;
			account_name			verifier;

			xmax_type_summary       digest() const;
		};

		struct block_confirmation : public block_confirmation_header {
			xmax_type_block_id		block_id;
			account_name			verifier;
			xmax_type_signature		builder_signature;

			fc::ecc::public_key        get_signer_key() const;
			void sign(const fc::ecc::private_key& signer);
			bool is_signer_valid(const fc::ecc::public_key &signer_key) const;
		};

    } } // Xmaxplatform::Chain

FC_REFLECT_ENUM(Xmaxplatform::Chain::transaction_receipt::result_code, (applied)(failure))

FC_REFLECT(Xmaxplatform::Chain::transaction_receipt_header, (receipt_idx)(result))
FC_REFLECT_DERIVED(Xmaxplatform::Chain::transaction_receipt, (Xmaxplatform::Chain::transaction_receipt_header), (trx))

FC_REFLECT(Xmaxplatform::Chain::block_header, (previous)(timestamp)(trxs_mroot)(builder)(next_builders))
FC_REFLECT_DERIVED(Xmaxplatform::Chain::signed_block_header, (Xmaxplatform::Chain::block_header), (builder_signature))
FC_REFLECT_DERIVED(Xmaxplatform::Chain::signed_block, (Xmaxplatform::Chain::signed_block_header))

FC_REFLECT(Xmaxplatform::Chain::block_confirmation_header, (block_id)(verifier))
FC_REFLECT_DERIVED(Xmaxplatform::Chain::block_confirmation, (Xmaxplatform::Chain::block_confirmation_header), (builder_signature))