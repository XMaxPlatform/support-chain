/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#include <blockchain_exceptions.hpp>
#include <blockchain_plugin.hpp>
#include <basechain.hpp>
#include <genesis_state.hpp>
#include <fc/io/json.hpp>
#include <fc/variant.hpp>
#include <fc/log/logger.hpp>

#include <native_contract_chain_init.hpp>
#include <chain_xmax.hpp>
#include <wast_to_wasm.hpp>
#include <mongodb_plugin.hpp>

namespace Xmaxplatform {


    class chain_plugin_impl {
    public:
        Baseapp::bfs::path genesis_file;
        std::unique_ptr<Chain::chain_xmax> chain;
		Chain::chain_id_type      chain_id;
		uint32_t	skip_flags = Chain::chain_xmax::skip_nothing;
    };


    blockchain_plugin::blockchain_plugin()
            : my(new chain_plugin_impl()) {
    }

    blockchain_plugin::~blockchain_plugin() {}

    void blockchain_plugin::set_program_options(options_description &cli, options_description &cfg) {
        ilog("blockchain_plugin::set_program_options");
        cfg.add_options()
                ("genesis-json", bpo::value<boost::filesystem::path>(), "File to read Genesis State from");
    }

    void blockchain_plugin::plugin_initialize(const variables_map &options) {
        ilog("blockchain_plugin::plugin_initialize");
        if (options.count("genesis-json")) {
			my->genesis_file = options.at("genesis-json").as<Baseapp::bfs::path>();
        }
    }

#define CALL(api_name, api_handle, api_namespace, call_name, http_response_code) \
{std::string("/v0/" #api_name "/" #call_name), \
   [this, api_handle](string, string body, url_response_callback cb) mutable { \
          try { \
             if (body.empty()) body = "{}"; \
             auto result = api_handle.call_name(fc::json::from_string(body).as<api_namespace::call_name ## _params>()); \
             cb(http_response_code, fc::json::to_string(result)); \
          } catch (Chain::tx_missing_sigs& e) { \
             error_results results{401, "UnAuthorized", e.to_string()}; \
             cb(401, fc::json::to_string(results)); \
          } catch (Chain::tx_duplicate& e) { \
             error_results results{409, "Conflict", e.to_string()}; \
             cb(409, fc::json::to_string(results)); \
          } catch (Chain::transaction_exception& e) { \
             error_results results{400, "Bad Request", e.to_string()}; \
             cb(400, fc::json::to_string(results)); \
          } catch (fc::eof_exception& e) { \
             error_results results{400, "Bad Request", e.to_string()}; \
             cb(400, fc::json::to_string(results)); \
             elog("Unable to parse arguments: ${args}", ("args", body)); \
          } catch (fc::exception& e) { \
             error_results results{500, "Internal Service Error", e.to_detail_string()}; \
             cb(500, fc::json::to_string(results)); \
             elog("Exception encountered while processing ${call}: ${e}", ("call", #api_name "." #call_name)("e", e)); \
          } \
       }}

#define CHAIN_RO_CALL(call_name, http_response_code) CALL(xmaxchain, ro_api, Chain_APIs::read_only, call_name, http_response_code)
#define CHAIN_RW_CALL(call_name, http_response_code) CALL(xmaxchain, rw_api, Chain_APIs::read_write, call_name, http_response_code)

    void blockchain_plugin::plugin_startup() {
        ilog("blockchain_plugin::plugin_startup");
        auto &data = app().get_plugin<chaindata_plugin>().data();
        auto genesis = fc::json::from_file(my->genesis_file).as<Native_contract::genesis_state_type>();
        Native_contract::native_contract_chain_init chainsetup(genesis);
		my->chain_id = genesis.compute_chain_id();		
		auto& mongo_plugin = app().get_plugin<mongodb_plugin>();

		Xmaxplatform::Chain::finalize_block_func finalize_func;
		if (mongo_plugin.get_state() == mongodb_plugin::registered)
		{
			auto p_mongo_plugin = &mongo_plugin;
			ilog("Found mongodb_plugin and ready to serialize data to the plugin.");
			finalize_func = [p_mongo_plugin](const Chain::signed_block& b) { p_mongo_plugin->finalize_block(b); };
		}


        my->chain.reset(new Chain::chain_xmax(data, chainsetup, finalize_func));

        register_chain_api();

    }

    void blockchain_plugin::register_chain_api() {
        ilog("register chain api");

        auto ro_api = get_read_only_api();
		auto rw_api = get_read_write_api();

        app().get_plugin<chainhttp_plugin>().add_api({
                                                        CHAIN_RO_CALL(get_account, 200),
														CHAIN_RO_CALL(get_table_rows, 200),
														CHAIN_RO_CALL(get_info, 200),
														CHAIN_RO_CALL(get_block, 200),
														CHAIN_RO_CALL(get_code, 200),
														CHAIN_RO_CALL(get_required_keys, 200),
														//---------------Write Apis-------------
														CHAIN_RW_CALL(push_transaction, 202),
														CHAIN_RW_CALL(push_transactions, 202)
                                                });
    }

    void blockchain_plugin::plugin_shutdown() {
        ilog("blockchain_plugin::plugin_startup");
    }

    Chain::chain_xmax &blockchain_plugin::getchain() { return *my->chain; }

    const Chain::chain_xmax &blockchain_plugin::getchain() const { return *my->chain; }

	void blockchain_plugin::get_chain_id(Chain::chain_id_type &cid)const {
		memcpy(cid.data(), my->chain_id.data(), cid.data_size());
	}


	//--------------------------------------------------
	Xmaxplatform::Chain_APIs::read_write blockchain_plugin::get_read_write_api() { 
		return Chain_APIs::read_write(getchain(), my->skip_flags); }

namespace Chain_APIs{


	const string read_only::KEYi64 = "i64";
	const string read_only::KEYstr = "str";
	const string read_only::KEYi128i128 = "i128i128";
	const string read_only::KEYi64i64i64 = "i64i64i64";
	const string read_only::PRIMARY = "primary";
	const string read_only::SECONDARY = "secondary";
	const string read_only::TERTIARY = "tertiary";

	//--------------------------------------------------
	Xmaxplatform::Chain_APIs::read_only::get_info_results read_only::get_info(const get_info_params& params) const {
		auto itoh = [](uint32_t n, size_t hlen = sizeof(uint32_t) << 1) {
			static const char* digits = "0123456789abcdef";
			std::string r(hlen, '0');
			for (size_t i = 0, j = (hlen - 1) * 4; i < hlen; ++i, j -= 4)
				r[i] = digits[(n >> j) & 0x0f];
			return r;
		};

		return get_info_results( itoh(static_cast<uint32_t>(app().version())),
			_chain.head_block_num(),		
			_chain.last_irreversible_block_num(),
			_chain.head_block_id(),
			_chain.head_block_time()
		);
	}
	//--------------------------------------------------
	Xmaxplatform::Chain_APIs::read_only::get_block_results read_only::get_block(const get_block_params& params) const {
		try {
			if (auto block = _chain.get_block_from_id(fc::json::from_string(params.block_num_or_id).as<Chain::xmax_type_block_id>()))
				return *block;
		}
		catch (fc::bad_cast_exception) {/* do nothing */ }
		catch (std::out_of_range) {/* do nothing */ }

		try {
			if (auto block = _chain.get_block_from_num(fc::to_uint64(params.block_num_or_id)))
				return *block;
		}
		catch (fc::bad_cast_exception) {/* do nothing */ }
		catch (std::out_of_range) {/* do nothing */ }

		FC_THROW_EXCEPTION(Chain::unknown_block_exception,
			"Could not find block: ${block}", ("block", params.block_num_or_id));
	}


	//--------------------------------------------------
	Xmaxplatform::Chain_APIs::read_only::get_code_results read_only::get_code(const get_code_params& params) const
	{
		get_code_results result;
		result.account_name = params.account_name;
		const auto& d = _chain.get_database();
		const auto& accnt = d.get<Chain::account_object, Chain::by_name>(params.account_name);

		if (accnt.code.size()) {
			result.wast = Chain::ConvertFromWasmToWast((const uint8_t*)accnt.code.data(), accnt.code.size());
			result.code_hash = fc::sha256::hash(accnt.code.data(), accnt.code.size());
		}
		Xmaxplatform::Basetypes::abi abi;
		if (Basetypes::abi_serializer::to_abi(accnt.abi, abi)) {
			result.abi = std::move(abi);
		}
		return result;
	}


	//--------------------------------------------------
	Xmaxplatform::Chain_APIs::read_only::get_required_keys_result read_only::get_required_keys(const get_required_keys_params& params) const
	{
		auto pretty_input = _chain.transaction_from_variant(params.transaction);
		auto required_keys_set = _chain.get_required_keys(pretty_input, params.available_keys);
		get_required_keys_result result;
		result.required_keys = required_keys_set;
		return result;
	}

	read_only::get_account_results read_only::get_account(const get_account_params &params) const {
        using namespace Xmaxplatform::Chain;

        get_account_results result;
        result.account_name = params.account_name;

        const auto &data = _chain.get_database();
        const auto &token = data.get<xmx_token_object, by_owner_name>(params.account_name);

        result.xmx_token = asset(token.xmx_token, XMX_SYMBOL);

        return result;
    }

	Basetypes::abi getAbi(const Chain::chain_xmax& db, const name& account) {
		const auto& d = db.get_database();
		const auto& code_accnt = d.get<Chain::account_object, Chain::by_name>(account);

		Xmaxplatform::Basetypes::abi abi;
		Basetypes::abi_serializer::to_abi(code_accnt.abi, abi);
		return abi;
	}

	string getTableType(const Basetypes::abi& abi, const name& tablename) {
		for (const auto& t : abi.tables) {
			if (t.table_name == tablename) {
				return t.index_type;
			}
		}
		FC_ASSERT(!"ABI does not define table", "Table ${table} not specified in ABI", ("table", tablename));
	}

	read_only::get_table_rows_result read_only::get_table_rows(const read_only::get_table_rows_params& p)const {
		const Basetypes::abi abi = getAbi(_chain, p.code);
		auto table_type = getTableType(abi, p.table);
		auto table_key = PRIMARY;

		if (table_type == KEYi64) {
			return get_table_rows_ex<Chain::key_value_index, Chain::by_scope_primary>(p, abi);
		}
		else if (table_type == KEYstr) {
			return get_table_rows_ex<Chain::keystr_value_index, Chain::by_scope_primary>(p, abi);
		}
		else if (table_type == KEYi128i128) {
			if (table_key == PRIMARY)
				return get_table_rows_ex<Chain::key128x128_value_index, Chain::by_scope_primary>(p, abi);
			if (table_key == SECONDARY)
				return get_table_rows_ex<Chain::key128x128_value_index, Chain::by_scope_secondary>(p, abi);
		}
		else if (table_type == KEYi64i64i64) {
			if (table_key == PRIMARY)
				return get_table_rows_ex<Chain::key64x64x64_value_index, Chain::by_scope_primary>(p, abi);
			if (table_key == SECONDARY)
				return get_table_rows_ex<Chain::key64x64x64_value_index, Chain::by_scope_secondary>(p, abi);
			if (table_key == TERTIARY)
				return get_table_rows_ex<Chain::key64x64x64_value_index, Chain::by_scope_tertiary>(p, abi);
		}
		FC_ASSERT(false, "invalid table type/key ${type}/${key}", ("type", table_type)("key", table_key)("code_abi", abi));
	}

	

	//--------------------------------------------------
	Xmaxplatform::Chain_APIs::read_write::push_transaction_results read_write::push_transaction(const push_transaction_params& params)
	{
		auto pretty_input = _chain.transaction_from_variant(params);
		auto ptrx = _chain.push_transaction(pretty_input, skip_flags);
		auto pretty_trx = _chain.transaction_to_variant(ptrx);
		auto pretty_events = _chain.transaction_events_to_variant(ptrx);
		return read_write::push_transaction_results{ pretty_input.id(), pretty_trx, pretty_events };
	}


	//--------------------------------------------------
	Xmaxplatform::Chain_APIs::read_write::push_transactions_results read_write::push_transactions(const push_transactions_params& params) {
		FC_ASSERT(params.size() <= 1000, "Attempt to push too many transactions at once");

		push_transactions_results result;
		result.reserve(params.size());
		for (const auto& item : params) {
			try {
				result.emplace_back(push_transaction(item));
			}
			catch (const fc::exception& e) {
				result.emplace_back(read_write::push_transaction_results{ Chain::xmax_type_transaction_id(),
					fc::mutable_variant_object("error", e.to_detail_string()) });
			}
		}
		return result;
	}

}// namespace Chain_APIs
} // namespace Xmaxplatform
