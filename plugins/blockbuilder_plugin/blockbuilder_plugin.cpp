/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#include <chaindata_plugin.hpp>
#include <blockbuilder_plugin.hpp>
#include <blockchain_plugin.hpp>
#include <block.hpp>

#include <fc/io/json.hpp>
#include <fc/variant.hpp>
#include <fc/log/logger.hpp>

namespace Xmaxplatform {


class blockbuilder_plugin_impl {
public:
    blockbuilder_plugin_impl(boost::asio::io_service& io)
    : _timer(io) {}
    boost::asio::deadline_timer _timer;
	void start_build();
private: 
	void start_check();
    void build_block();

	void genesis_block_build();
    block_build_condition block_build_loop();
    block_build_condition build_block(fc::mutable_variant_object& capture);
};

    blockbuilder_plugin::blockbuilder_plugin()
:my(new blockbuilder_plugin_impl(app().get_io_service())) {
}

    blockbuilder_plugin::~blockbuilder_plugin(){}

void blockbuilder_plugin::set_program_options(options_description& cli, options_description& cfg)
{

}

void blockbuilder_plugin::plugin_initialize(const variables_map& options) {
    ilog("blockbuilder_plugin::plugin_initialize");
}

void blockbuilder_plugin::plugin_startup() {
    ilog("blockbuilder_plugin::plugin_startup");
    my->start_build();
}

void blockbuilder_plugin::plugin_shutdown() {
    ilog("blockbuilder_plugin::plugin_startup");
}

	void blockbuilder_plugin_impl::start_build()
	{
		int64_t start_delay = 1000000;

		_timer.expires_from_now(boost::posix_time::microseconds(start_delay));
		_timer.async_wait(boost::bind(&blockbuilder_plugin_impl::start_check, this));
	}

	void blockbuilder_plugin_impl::start_check()
	{
		const Chain::chain_xmax& chain = app().get_plugin<blockchain_plugin>().getchain();
		const Chain::dynamic_states_object& states = chain.get_dynamic_states();
		if (0 == chain.head_block_num())
		{
			ilog("build loop with genesis block.");
			genesis_block_build();
		}
		else
		{
			ilog("build loop.");
			build_block();
		}
	}



    void blockbuilder_plugin_impl::build_block() {

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
        _timer.async_wait(boost::bind(&blockbuilder_plugin_impl::block_build_loop, this));
    }

    block_build_condition blockbuilder_plugin_impl::block_build_loop() {
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
                const auto& data = app().get_plugin<blockchain_plugin>().getchain();
                ilog("block_build_condition::generated");
                break;
            }
            case block_build_condition::exception:
                elog( "exception producing block." );
                break;
			case block_build_condition::not_time_yet:
				ilog("not time for producing block.");
				break;
			default:
			{
				elog("unexpectedly block_build_condition.");
				break;
			}
        }

        build_block();
        return result;
    }

	void blockbuilder_plugin_impl::genesis_block_build()
	{
		Chain::chain_xmax& chain = app().get_plugin<blockchain_plugin>().getchain();

		const Chain::dynamic_states_object& states = chain.get_dynamic_states();

		Chain::chain_timestamp now_timestamp = Chain::chain_timestamp::from(states.state_time);

		const account_name current_builder = states.block_builder;

		try
		{
			Chain::signed_block block = chain.generate_block(
				now_timestamp,
				current_builder
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

		build_block();
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

		const account_name current_builder = chain.get_block_builder(delta_slot);


		assert(now > chain.head_block_time());

		Chain::signed_block block = chain.generate_block(now_timestamp, current_builder);

        //TODO
        //broadcast
        return block_build_condition::generated;
    }

} // namespace Xmaxplatform
