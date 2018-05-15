/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once
#include <basechain.hpp>
#include <blockchain_types.hpp>
#include <blockchain_setup.hpp>
#include <builder_rule.hpp>
#include <message_context_xmax.hpp>

namespace Xmaxplatform { namespace Chain {
	using database = Basechain::database;
    class message_xmax;
	struct chain_xmax;
	class builder_object;

    class chain_init {
    public:
        virtual ~chain_init();

        virtual Basetypes::time get_chain_init_time() const = 0;

        virtual Chain::blockchain_setup get_blockchain_setup() const = 0;

        virtual Chain::xmax_builder_infos get_chain_init_builders() const = 0;

        virtual void register_handlers(chain_xmax &chain, database &db) = 0;

        virtual vector<message_xmax> prepare_data(chain_xmax &chain, database &db) = 0;
    };

} }
