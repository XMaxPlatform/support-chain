/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once
#include <application.hpp>
#include <plugin.hpp>
#include <boost/container/flat_set.hpp>
#include <chain_xmax.hpp>
#include <chainhttp_plugin.hpp>
#include <abi_serializer.hpp>
#include <blockchain_types.hpp>

using namespace Baseapp;
namespace Xmaxplatform {

    using namespace Basetypes;
	using boost::container::flat_set;
	using fc::optional;

    namespace Chain_APIs {
        struct empty{};

        class read_only {
            const Chain::chain_xmax& _chain;

        public:
            static const string KEYi128;
            static const string KEYstr;
            static const string KEYi128i128;
			static const string KEYi128i128i128;
          //  static const string KEYi64i64i64;
            static const string PRIMARY;
            static const string SECONDARY;
            static const string TERTIARY;

            read_only(const Chain::chain_xmax& chain)
                    : _chain(chain) {}


			using get_info_params = empty;

			struct get_info_results {

				get_info_results(const string& v, uint32_t hbn, uint32_t chbn, uint32_t libn,
					const Chain::xmax_type_block_id& hbi, const Basetypes::time& hbt) :
					server_version(v),
					head_block_num(hbn),
					confirmed_head_block_num(chbn),
					last_irreversible_block_num(libn),
					head_block_id(hbi),
					head_block_time(hbt)
				{

				}

				string                server_version;
				uint32_t              head_block_num = 0;	
				uint32_t              confirmed_head_block_num = 0;
				uint32_t              last_irreversible_block_num = 0;
				Chain::xmax_type_block_id  head_block_id;
				Basetypes::time    head_block_time;
			};

			get_info_results get_info( const get_info_params& params) const;


            struct get_account_results {
                name                       account_name;
                asset                      main_token = asset(0,MAIN_SYMBOL);
            };
            struct get_account_params {
                Basetypes::name account_name;
            };
			get_account_results get_account(const get_account_params& params)const;

			struct get_table_rows_params {
				bool        json = false;
				name        scope;
				name        code;
				name        table;
				string      table_key;
				string      lower_bound;
				string      upper_bound;
				uint32_t    limit = 10;
			};

			struct get_table_rows_result {
				vector<fc::variant> rows; 
				bool                more;
			};
			get_table_rows_result get_table_rows( const get_table_rows_params& params )const;
			

			struct get_block_params {
				string block_num_or_id;
			};

			struct get_block_results : public Chain::signed_block {
				get_block_results(const Chain::signed_block& b)
					:signed_block(b),
					id(b.id()),
					block_num(b.block_num()),
					ref_block_prefix(id._hash[1])
				{}

				Chain::xmax_type_block_id id;
				Chain::xmax_type_block_num	 block_num = 0;
				uint32_t             ref_block_prefix = 0;
			};

			get_block_results get_block(const get_block_params& params) const;

			struct get_block_header_params {
				string block_num_or_id;
			};

			struct get_block_header_results
			{
				get_block_header_results(const Chain::signed_block_header& b)
					:previous(b.previous),
					trxs_mroot(b.trxs_mroot),
					builder(b.builder),
					timestamp(b.timestamp.get_stamp()),
					builder_signature(b.builder_signature)
				{}

				Xmaxplatform::Chain::xmax_type_block_id            previous;
				Xmaxplatform::Chain::xmax_type_merkle_root         trxs_mroot;
				account_name										builder;
				uint32_t                                            timestamp;
				Xmaxplatform::Chain::xmax_type_signature			builder_signature;
			};

			get_block_header_results get_block_header(const get_block_header_params& params) const;

			

			struct get_code_results {
				name                   account_name;
				string                 wast;
				fc::sha256             code_hash;
				optional<Basetypes::abi>   abi;
			};

			struct get_code_params {
				name                   account_name;
			};
			get_code_results get_code(const get_code_params& params)const;

			struct get_required_keys_params {
				fc::variant transaction;
				flat_set<Chain::public_key_type> available_keys;
			};

			struct get_required_keys_result {
				flat_set<Chain::public_key_type> required_keys;
			};

			get_required_keys_result get_required_keys(const get_required_keys_params& params)const;

			struct erc20_total_supply_params {
				asset_symbol token_name;
			};

			struct erc20_total_supply_result {
				uint256 total_supply;
			};

			erc20_total_supply_result erc20_total_supply(const erc20_total_supply_params& params) const;

			struct erc20_balanceof_params {
				asset_symbol token_name;
				account_name owner;
			};

			struct erc20_balanceof_result {
				uint256 balance;
			};

			erc20_balanceof_result erc20_balanceof(const erc20_balanceof_params& params) const;

			using erc721_balanceof_params = erc20_balanceof_params;
			using erc721_balanceof_result = erc20_balanceof_result;
			erc721_balanceof_result erc721_balanceof(const erc721_balanceof_params& params) const;

			struct erc721_ownerof_params {
				asset_symbol token_name;
				Chain::xmax_erc721_id token_id;
			};

			struct erc721_ownerof_result {
				account_name owner;
			};
			erc721_ownerof_result erc721_ownerof(const erc721_ownerof_params& params) const;


			void copy_row(const Chain::key_value_object& obj, vector<char>& data)const {
				data.resize(obj.primary_key.backend().size() * 4 + 4 + obj.value.size());
				fc::datastream<char*> ds(data.data(), data.size());
				fc::raw::pack(ds, obj.primary_key);
				ds.write(obj.value.data(), obj.value.size());
				data.resize(ds.tellp());
				//memcpy(data.data(), &obj.primary_key, sizeof(uint128_t));
				//memcpy(data.data() + sizeof(uint128_t), obj.value.data(), obj.value.size());
			}

			void copy_row(const Chain::keystr_value_object& obj, vector<char>& data)const {
				data.resize(obj.primary_key.size() + obj.value.size() + 8);
				fc::datastream<char*> ds(data.data(), data.size());
				fc::raw::pack(ds, obj.primary_key);
				ds.write(obj.value.data(), obj.value.size());
				data.resize(ds.tellp());
			}

			void copy_row(const Chain::key128x128_value_object& obj, vector<char>& data)const {
				data.resize(2 * sizeof(Chain::key128x128_value_object::key_type) + obj.value.size());
				memcpy(data.data(), &obj.primary_key, sizeof(Chain::key128x128_value_object::key_type));
				memcpy(data.data() + sizeof(Chain::key128x128_value_object::key_type), &obj.secondary_key, sizeof(Chain::key128x128_value_object::key_type));
				memcpy(data.data() + 2 * sizeof(Chain::key128x128_value_object::key_type), obj.value.data(), obj.value.size());
			}

			void copy_row(const Chain::key128x128x128_value_object& obj, vector<char>& data)const {
				data.resize(3 * sizeof(Chain::key128x128x128_value_object::key_type) + obj.value.size());
				memcpy(data.data(), &obj.primary_key, sizeof(Chain::key128x128x128_value_object::key_type));
				memcpy(data.data() + sizeof(Chain::key128x128x128_value_object::key_type), &obj.secondary_key, sizeof(Chain::key128x128x128_value_object::key_type));				
				memcpy(data.data() + 2 * sizeof(Chain::key128x128x128_value_object::key_type), &obj.tertiary_key, sizeof(Chain::key128x128x128_value_object::key_type));
				memcpy(data.data() + 3 * sizeof(Chain::key128x128x128_value_object::key_type), obj.value.data(), obj.value.size());
			}

			void copy_row(const Chain::key64x64x64_value_object& obj, vector<char>& data)const {
				data.resize(3 * sizeof(uint64_t) + obj.value.size());
				memcpy(data.data(), &obj.primary_key, sizeof(uint64_t));
				memcpy(data.data() + sizeof(uint64_t), &obj.secondary_key, sizeof(uint64_t));
				memcpy(data.data() + 2 * sizeof(uint64_t), &obj.tertiary_key, sizeof(uint64_t));
				memcpy(data.data() + 3 * sizeof(uint64_t), obj.value.data(), obj.value.size());
			}

			template <typename IndexType, typename Scope>
			read_only::get_table_rows_result get_table_rows_ex(const read_only::get_table_rows_params& p, const Basetypes::abi& abi)const {
				read_only::get_table_rows_result result;
				const auto& d = _chain.get_database();

				Basetypes::abi_serializer abis;
				abis.set_abi(abi);

				const auto& idx = d.get_index<IndexType, Scope>();
				int size = idx.size();
				auto lower = idx.lower_bound(boost::make_tuple(p.scope, p.code, p.table));
				auto upper = idx.upper_bound(boost::make_tuple(p.scope, p.code, name(uint128(p.table) + 1)));

				if (p.lower_bound.size())
					lower = idx.lower_bound(boost::make_tuple(p.scope, p.code, p.table, fc::variant(p.lower_bound).as<typename IndexType::value_type::key_type>()));
				if (p.upper_bound.size())
					upper = idx.lower_bound(boost::make_tuple(p.scope, p.code, p.table, fc::variant(p.upper_bound).as<typename IndexType::value_type::key_type>()));

				vector<char> data;

				auto end = fc::time_point::now() + fc::microseconds(1000 * 10); /// 10ms max time

				unsigned int count = 0;
				auto itr = lower;
				for (itr = lower; itr != upper && itr->table == p.table; ++itr) {
					copy_row(*itr, data);

					if (p.json)
						result.rows.emplace_back(abis.binary_to_variant(abis.get_table_type(p.table), data));
					else
						result.rows.emplace_back(fc::variant(data));
					if (++count == p.limit || fc::time_point::now() > end)
						break;
				}
				if (itr != upper)
					result.more = true;
				return result;
			}
        };


		class read_write {
			Chain::chain_xmax& _chain;
			uint32_t skip_flags;

		public:
			read_write(Chain::chain_xmax& chain, uint32_t skip_flags)
				: _chain(chain), skip_flags(skip_flags) {}

			using push_transaction_params = fc::variant_object;

			struct push_transaction_results {
				Chain::xmax_type_transaction_id  transaction_id;
				fc::variant                 processed;
				fc::variant                 events;
			};

			push_transaction_results push_transaction_package(Chain::transaction_package_ptr package);

			push_transaction_results push_transaction(const push_transaction_params& params);

			using push_transactions_params = vector<push_transaction_params>;
			using push_transactions_results = vector<push_transaction_results>;
			push_transactions_results push_transactions(const push_transactions_params& params);

		};

    } // namespace Chain_APIs


class blockchain_plugin : public plugin<blockchain_plugin> {
public:

    BASEAPP_DEPEND_PLUGINS((chainhttp_plugin))
    blockchain_plugin();
   virtual ~blockchain_plugin();

   virtual void set_program_options(options_description& cli, options_description& cfg) override;

   void plugin_initialize(const variables_map& options);
   void plugin_startup();
   void plugin_shutdown();

    Chain::chain_xmax& getchain();
    const Chain::chain_xmax& getchain() const;

	void get_chain_id(Chain::chain_id_type &cid) const;

    Chain_APIs::read_only get_read_only_api() const { return Chain_APIs::read_only(getchain()); }
	Chain_APIs::read_write get_read_write_api();
private:
    std::unique_ptr<class chain_plugin_impl> my;
    void register_chain_api();
};

}

FC_REFLECT(Xmaxplatform::Chain_APIs::empty, )

FC_REFLECT( Xmaxplatform::Chain_APIs::read_only::get_account_results, (account_name)(main_token) )

FC_REFLECT( Xmaxplatform::Chain_APIs::read_only::get_account_params, (account_name) )

FC_REFLECT(Xmaxplatform::Chain_APIs::read_only::get_table_rows_params, (json)(table_key)(scope)(code)(table)(lower_bound)(upper_bound)(limit))

FC_REFLECT(Xmaxplatform::Chain_APIs::read_only::get_table_rows_result, (rows)(more))

FC_REFLECT(Xmaxplatform::Chain_APIs::read_only::get_block_params, (block_num_or_id))

FC_REFLECT_DERIVED(Xmaxplatform::Chain_APIs::read_only::get_block_results, (Xmaxplatform::Chain::signed_block), (id)(block_num)(ref_block_prefix));

FC_REFLECT(Xmaxplatform::Chain_APIs::read_only::get_block_header_params, (block_num_or_id))

FC_REFLECT(Xmaxplatform::Chain_APIs::read_only::get_block_header_results, (previous)(trxs_mroot)(builder)(timestamp)(builder_signature));


FC_REFLECT(Xmaxplatform::Chain_APIs::read_only::get_info_results,
(server_version)(head_block_num)(confirmed_head_block_num)(last_irreversible_block_num)(head_block_id)(head_block_time))
FC_REFLECT(Xmaxplatform::Chain_APIs::read_write::push_transaction_results, (transaction_id)(processed)(events))
FC_REFLECT(Xmaxplatform::Chain_APIs::read_only::get_required_keys_params, (transaction)(available_keys))
FC_REFLECT(Xmaxplatform::Chain_APIs::read_only::get_required_keys_result, (required_keys))
FC_REFLECT(Xmaxplatform::Chain_APIs::read_only::get_code_results, (account_name)(code_hash)(wast)(abi))
FC_REFLECT(Xmaxplatform::Chain_APIs::read_only::get_code_params, (account_name))
FC_REFLECT(Xmaxplatform::Chain_APIs::read_only::erc20_total_supply_params, (token_name))
FC_REFLECT(Xmaxplatform::Chain_APIs::read_only::erc20_total_supply_result, (total_supply))
FC_REFLECT(Xmaxplatform::Chain_APIs::read_only::erc20_balanceof_params, (token_name)(owner))
FC_REFLECT(Xmaxplatform::Chain_APIs::read_only::erc20_balanceof_result, (balance))
FC_REFLECT(Xmaxplatform::Chain_APIs::read_only::erc721_ownerof_params, (token_name)(token_id))
FC_REFLECT(Xmaxplatform::Chain_APIs::read_only::erc721_ownerof_result, (owner))