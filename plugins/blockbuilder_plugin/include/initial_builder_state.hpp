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

struct initial_builder
{
public:
	std::string builder_name;
	std::string sign_private_key;
};

struct initial_builder_state
{
public:
	std::vector<initial_builder> initial_builders;
};


}  // namespace Xmaxplatform::Native_contract

FC_REFLECT(Xmaxplatform::initial_builder,
           (builder_name)(sign_private_key))

FC_REFLECT(Xmaxplatform::initial_builder_state, (initial_builders))

