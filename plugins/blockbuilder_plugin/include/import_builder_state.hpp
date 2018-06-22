/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once

#include <string>
#include <vector>
#include <blockchain_types.hpp>
#include <fc/reflect/reflect.hpp>


namespace Xmaxplatform { 

struct import_builder
{
public:
	std::string builder_name;
	std::string sign_private_key;
};

struct import_builder_state
{
public:
	std::vector<import_builder> import_builders;
};


}  // namespace Xmaxplatform::Native_contract

FC_REFLECT(Xmaxplatform::import_builder,
           (builder_name)(sign_private_key))

FC_REFLECT(Xmaxplatform::import_builder_state, (import_builders))

