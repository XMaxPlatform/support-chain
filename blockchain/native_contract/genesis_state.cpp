/**
 *  @file
 *  @copyright defined in xmax/LICENSE.txt
 */

#include <genesis_state.hpp>

// these are required to serialize a genesis_state
#include <fc/smart_ref_impl.hpp>   // required for gcc in release mode

namespace Xmaxplatform { namespace Native_contract {

Chain::chain_id_type genesis_state_type::compute_chain_id() const {
   return initial_chain_id;
}

} } // Xmaxplatform::Native_contract
