/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#include <chaindata_plugin.hpp>
#include <blockbuilder_plugin.hpp>
#include <blockchain_plugin.hpp>

#include <fc/io/json.hpp>
#include <fc/variant.hpp>
#include <fc/log/logger.hpp>

namespace Xmaxplatform {


class blockbuilder_plugin_impl {
public:
    blockbuilder_plugin_impl(boost::asio::io_service& io)
    : _timer(io) {}
    boost::asio::deadline_timer _timer;

    void build_block();
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
    Chain::chain_xmax& chain = app().get_plugin<blockchain_plugin>().getchain();
    my->build_block();
}

void blockbuilder_plugin::plugin_shutdown() {
    ilog("blockbuilder_plugin::plugin_startup");
}

    void blockbuilder_plugin_impl::build_block() {

        fc::time_point now = fc::time_point::now();
        int64_t time_to_next_second = 1000000 - (now.time_since_epoch().count() % 1000000);
        if(time_to_next_second < 50000) {     // we must sleep for at least 50ms
            wlog("Less than 50ms to next second, time_to_next_second ${s}ms", ("s", time_to_next_second/1000));
            time_to_next_second += 1000000;
        }

        _timer.expires_from_now(boost::posix_time::microseconds(time_to_next_second));
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


                break;
            }
            case block_build_condition::exception:
                elog( "exception producing block" );
                break;
        }

        build_block();
        return result;
    }

    block_build_condition blockbuilder_plugin_impl::build_block(
            fc::mutable_variant_object &capture) {
        Chain::chain_xmax& chain = app().get_plugin<blockchain_plugin>().getchain();
        fc::time_point now_fine = fc::time_point::now();
        fc::time_point_sec now = now_fine + fc::microseconds(500000);

        auto block = chain.generate_block(
                now,
                chain.get_dynamic_states().current_builder
        );





        //TODO
        //broadcast
        return block_build_condition::generated;
    }

} // namespace Xmaxplatform
