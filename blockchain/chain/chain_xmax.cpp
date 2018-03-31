/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */

#include <blockchain_types.hpp>
#include <block.hpp>
#include <chain_xmax.hpp>


#include <rand.hpp>

#include <fc/smart_ref_impl.hpp>
#include <fc/uint128.hpp>
#include <fc/crypto/digest.hpp>

#include <boost/range/algorithm/copy.hpp>
#include <boost/range/algorithm_ext/is_sorted.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/adaptor/map.hpp>

#include <fstream>
#include <functional>
#include <iostream>
#include <chrono>

namespace Xmaxplatform { namespace Chain {


        void chain_xmax::setup_data_indexes() {
            //TODO
        }

        void chain_xmax::initialize_chain()
        { try {

            } FC_CAPTURE_AND_RETHROW() }

        chain_xmax::chain_xmax(database& database) : _data(database){

            setup_data_indexes();


        }

        chain_xmax::~chain_xmax() {

            _data.flush();
        }



} }
