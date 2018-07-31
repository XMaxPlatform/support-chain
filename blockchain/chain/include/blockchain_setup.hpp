/**
 *  @file
 *  @copyright defined in xmax/LICENSE.txt
 */
#pragma once

#include <xmaxtypes.hpp>

namespace Xmaxplatform {
namespace Chain {

struct blockchain_setup : public Basetypes::blockchain_configuration {
   using Basetypes::blockchain_configuration::blockchain_configuration;

   blockchain_configuration& operator= (const Basetypes::blockchain_configuration& other);

   static blockchain_configuration get_median_values(std::vector<blockchain_configuration> votes);

   friend std::ostream& operator<< (std::ostream& s, const blockchain_configuration& p);
};

bool operator==(const Basetypes::blockchain_configuration& a, const Basetypes::blockchain_configuration& b);
inline bool operator!=(const Basetypes::blockchain_configuration& a, const Basetypes::blockchain_configuration& b) {
   return !(a == b);
}

}
} // namespace Xmaxplatform::chain

FC_REFLECT_DERIVED(Xmaxplatform::Chain::blockchain_setup, (Xmaxplatform::Basetypes::blockchain_configuration), )
