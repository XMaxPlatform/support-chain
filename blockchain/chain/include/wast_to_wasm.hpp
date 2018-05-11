/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <vector>
#include <string>

namespace Xmaxplatform { namespace Chain {

std::vector<uint8_t> ConvertFromWastToWasm( const std::string& wast );
std::string  ConvertFromWasmToWast( const std::vector<uint8_t>& wasm );
std::string  ConvertFromWasmToWast( const uint8_t* data, uint64_t size );

} } // Xmaxplatform::chain
