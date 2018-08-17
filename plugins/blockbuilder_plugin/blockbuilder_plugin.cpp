/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#include <blockchain_exceptions.hpp>
#include <import_builder_state.hpp>
#include <blockbuilder_plugin.hpp>
#include <blockchain_plugin.hpp>
#include <chainnet_plugin.hpp>
#include <block.hpp>

#include <key_conversion.hpp>
#include <fc/io/json.hpp>
#include <fc/variant.hpp>
#include <fc/log/logger.hpp>

namespace Xmaxplatform {

using namespace Chain;

class blockbuilder_plugin_impl {
public:
    blockbuilder_plugin_impl(boost::asio::io_service& io)
    : _timer(io) {}
    boost::asio::deadline_timer _timer;
	void start_build();
	bool import_key(const account_name& builder, const Basetypes::string& private_key);
	bool import_key(const account_name& builder, const private_key_type& private_key);

	void set_builders_file(const Baseapp::bfs::path& file_path);

	void init_builders();

	void recieve_block(signed_block_ptr block);
private: 

	void start_check();
    void next_block();

	void genesis_block_build();
	void confirmation_block_self();
    block_build_condition next_block_impl();
    block_build_condition build_block(fc::mutable_variant_object& capture);

private:
	Baseapp::bfs::path _builders_file;
	typedef std::map<Chain::public_key_type, Chain::private_key_type> keymap;
	keymap _builder_keys;
	std::set<account_name> _builders;
};

    blockbuilder_plugin::blockbuilder_plugin()
:my(new blockbuilder_plugin_impl(app().get_io_service())) {
}

	void blockbuilder_plugin_impl::set_builders_file(const Baseapp::bfs::path& file_path)
	{
		_builders_file = file_path;
	}

    blockbuilder_plugin::~blockbuilder_plugin(){}

void blockbuilder_plugin::set_program_options(options_description& cli, options_description& cfg)
{
	cfg.add_options()
		("import-builders", bpo::value<boost::filesystem::path>(), "Json file to read builders from");
}

void blockbuilder_plugin::plugin_initialize(const variables_map& options) {
    ilog("blockbuilder_plugin::plugin_initialize");

	if (options.count("import-builders")) {
		my->set_builders_file(options.at("import-builders").as<Baseapp::bfs::path>());
	}

}

void blockbuilder_plugin::plugin_startup() {

	my->init_builders();

    ilog("blockbuilder_plugin::plugin_startup");
    my->start_build();
}

void blockbuilder_plugin::plugin_shutdown() {
    ilog("blockbuilder_plugin::plugin_startup");
}

bool blockbuilder_plugin::import_key(const account_name& builder, const Basetypes::string& private_key)
{
	return my->import_key(builder, private_key);
}
	void blockbuilder_plugin_impl::start_build()
	{
		int64_t start_delay = 1000000;

		_timer.expires_from_now(boost::posix_time::microseconds(start_delay));
		_timer.async_wait(boost::bind(&blockbuilder_plugin_impl::start_check, this));
	}
	bool blockbuilder_plugin_impl::import_key(const account_name& builder, const Basetypes::string& private_key)
	{
		fc::optional<private_key_type> optional_private_key = Utilities::wif_to_key(private_key);
		if (!optional_private_key)
		{
			//FC_THROW("Invalid private key of builder");
			return false;
		}
		import_key(builder, *optional_private_key);
		return true;
	}

	void blockbuilder_plugin_impl::init_builders()
	{

		if (_builders_file.has_leaf())
		{
			auto builder_state = fc::json::from_file(_builders_file).as<import_builder_state>();

			for (const auto& it : builder_state.import_builders)
			{
				account_name name = it.builder_name;
				import_key(name, it.sign_private_key);
			}
		}
	}

	bool blockbuilder_plugin_impl::import_key(const account_name& builder, const private_key_type& private_key)
	{
		if (_builders.find(builder) == _builders.end())
		{
			_builders.insert(builder);
		}
		public_key_type public_key = private_key.get_public_key();
		_builder_keys[public_key] = private_key;

		return true;
	}
	void blockbuilder_plugin_impl::start_check()
	{
		const Chain::chain_xmax& chain = app().get_plugin<blockchain_plugin>().getchain();
		const Chain::dynamic_states_object& states = chain.get_dynamic_states();
		if (0 == chain.head_block_num()) // start genesis.
		{
			ilog("build loop with genesis block.");
			genesis_block_build();
		}
		else// continue work.
		{
			ilog("build loop.");
			next_block();
		}
	}



    void blockbuilder_plugin_impl::next_block() {

		// Next build time.
		// If we would wait less than "1/10 of block_interval", wait for the whole block interval.
		fc::time_point now = fc::time_point::now();
		int64_t time_to_next_block_time = (Config::chain_timestamp_unit_us) - (now.time_since_epoch().count() % (Config::chain_timestamp_unit_us));
		if (time_to_next_block_time < Config::mini_next_block_us) {     // we must sleep for at least 50ms
			ilog("Less than ${t}us to next block time, time_to_next_block_time ${bt}us",
				("t", Config::mini_next_block_us)("bt", time_to_next_block_time));
			time_to_next_block_time += Config::chain_timestamp_unit_us;
		}


        _timer.expires_from_now(boost::posix_time::microseconds(time_to_next_block_time));
        _timer.async_wait(boost::bind(&blockbuilder_plugin_impl::next_block_impl, this));
    }

    block_build_condition blockbuilder_plugin_impl::next_block_impl() {
        block_build_condition result;
        fc::mutable_variant_object capture;
        try
        {
            result = build_block(capture);
        }
        catch( const fc::canceled_exception& )
        {
            //We're trying to exit. Go ahead and let this one out.
            throw;
        }
        catch( const fc::exception& e )
        {
            elog("Got exception while generating block:\n${e}", ("e", e.to_detail_string()));
            result = block_build_condition::exception;
        }

        switch(result)
        {
            case block_build_condition::generated:
            {
                //const auto& data = app().get_plugin<blockchain_plugin>().getchain();
                ilog("block_build_condition::generated");

				confirmation_block_self();
                break;
            }
            case block_build_condition::exception:
			{
				elog("exception producing block.");
				break;
			}
			case block_build_condition::not_time_yet:
			{
				ilog("not time for building block.");
				break;
			}
			case block_build_condition::not_my_turn:
			{
				ilog("not my turn to build block, builder_info: ${builder_info}", (capture));
				break;
			}
			case block_build_condition::no_private_key:
			{
				ilog("no suitable private key for '${signing_key}'.", (capture) );
				break;
			}
			default:
			{
				elog("unexpectedly block_build_condition.");
				break;
			}
        }

		next_block();
        return result;
    }

	void blockbuilder_plugin_impl::genesis_block_build()
	{

		// init first block properties.
		Chain::chain_xmax& chain = app().get_plugin<blockchain_plugin>().getchain();

		const Chain::dynamic_states_object& states = chain.get_dynamic_states();

		Chain::chain_timestamp now_timestamp = Chain::chain_timestamp::from(states.state_time) + Chain::chain_timestamp::create(1);

		const account_name current_builder = Config::xmax_contract_name;
		const fc::optional<fc::ecc::private_key> current_key = Config::xmax_build_private_key;

		try
		{
			// build the first block.
			chain.build_block(
				now_timestamp,
				*(current_key)
			);
		}
		catch (const fc::canceled_exception&)
		{
			//We're trying to exit. Go ahead and let this one out.
			throw;
		}
		catch (const fc::exception& e)
		{
			elog("Got exception while generating genesis block:\n${e}", ("e", e.to_detail_string()));	
		}

		next_block();
	}
    block_build_condition blockbuilder_plugin_impl::build_block(
            fc::mutable_variant_object &capture) {

        Chain::chain_xmax& chain = app().get_plugin<blockchain_plugin>().getchain();
        fc::time_point now = fc::time_point::now();

        Chain::chain_timestamp now_timestamp = Chain::chain_timestamp::from(now);

		//delta slot
		uint32_t delta_slot = chain.get_delta_slot_at_time(now_timestamp);
		if (delta_slot == 0)
		{
			capture("next_time", chain.get_delta_slot_time(1));
			return block_build_condition::not_time_yet;
		}
		assert(now > chain.head_block_time());

		const builder_info& current_builder = chain.get_block_builder(delta_slot);

		// not ower builder.
		if (_builders.find(current_builder.builder_name) == _builders.end())
		{
			capture("builder_info", current_builder);
			return block_build_condition::not_my_turn;
		}
		// 
		auto private_key = _builder_keys.find(current_builder.block_signing_key);
		if (private_key == _builder_keys.end())
		{
			capture("signing_key", current_builder.block_signing_key);
			return block_build_condition::no_private_key;

		}

		chain.build_block(now_timestamp, private_key->second);

       
		chainnet_plugin& netPlugin = app().get_plugin<chainnet_plugin>();
		netPlugin.broadcast_block( *chain.head_block() );
        return block_build_condition::generated;
    }

	void blockbuilder_plugin_impl::recieve_block(signed_block_ptr block)
	{
		Chain::chain_xmax& chain = app().get_plugin<blockchain_plugin>().getchain();

		uint32_t order_slot = chain.get_order_slot_at_time(block->timestamp);

		const builder_rule& verifiers = chain.get_verifiers_by_order(order_slot);


		std::vector<std::pair<account_name, private_key_type>> keys;

		for (const auto& it : verifiers.builders)
		{
			auto private_key = _builder_keys.find(it.block_signing_key);
			if (private_key != _builder_keys.end())
			{
				keys.push_back(std::make_pair(it.builder_name, private_key->second));
			}
		}

		chain.confirm_block(block);

		if (keys.size())
		{
			chainnet_plugin& netPlugin = app().get_plugin<chainnet_plugin>();
			broadcast_confirm_func confirm_func = std::bind(&chainnet_plugin::broadcast_confirm, std::ref(netPlugin), std::placeholders::_1);

			for (const auto& key : keys)
			{
				chain.broadcast_confirmation(key.first, key.second, confirm_func);

			}
		}

	}

	void blockbuilder_plugin_impl::confirmation_block_self()
	{
		Chain::chain_xmax& chain = app().get_plugin<blockchain_plugin>().getchain();
		block_pack_ptr pack_ptr = chain.head_block_pack();

		if (!pack_ptr)
		{
			return;
		}
		const auto& verifiers = pack_ptr->current_builders;

		for (const auto v : verifiers.builders)
		{
			if (v.builder_name != pack_ptr->new_header.builder)
			{
				// 
				auto private_key = _builder_keys.find(v.block_signing_key);
				if (private_key != _builder_keys.end())
				{
					block_confirmation conf;
					conf.block_id = pack_ptr->block_id;
					conf.verifier = v.builder_name;

					conf.sign(private_key->second);

					chain.push_confirmation(conf);
				}
			}

		}

	}

	void blockbuilder_plugin::on_recv_message(const signed_block &msg)
	{
		signed_block_ptr pblock = std::make_shared<signed_block>();
		*pblock = msg;
		my->recieve_block(pblock);
	}

	void blockbuilder_plugin::on_recv_message(const block_confirmation& msg)
	{
		Chain::chain_xmax& chain = app().get_plugin<blockchain_plugin>().getchain();
		chain.push_confirmation(msg);
	}

	Chain::vector<Chain::signed_block> blockbuilder_plugin::get_sync_blocklist(const uint32_t& lastnum)
	{
		Chain::chain_xmax& chain = app().get_plugin<blockchain_plugin>().getchain();
		std::vector<signed_block> blocklist = chain.get_syncblock_from_lastnum(lastnum);		
		return blocklist;
	}

} // namespace Xmaxplatform
