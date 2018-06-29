/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once

#include "blockchain_config.hpp"

#include <basechain.hpp>
#include <include/xmaxtypes.hpp>
#include <include/basemisc.hpp>

#include <fc/container/flat_fwd.hpp>
#include <fc/io/varint.hpp>
#include <fc/io/enum_type.hpp>
#include <fc/crypto/sha224.hpp>
#include <fc/optional.hpp>
#include <fc/safe.hpp>
#include <fc/container/flat.hpp>
#include <fc/string.hpp>
#include <fc/io/raw.hpp>
#include <fc/uint128.hpp>
#include <fc/static_variant.hpp>
#include <fc/smart_ref_fwd.hpp>
#include <fc/crypto/ripemd160.hpp>
#include <fc/fixed_string.hpp>

#include <memory>
#include <vector>
#include <deque>
#include <cstdint>

#define OBJECT_CCTOR1(NAME) \
    NAME() = delete; \
    public: \
    template<typename Constructor, typename Allocator> \
    NAME(Constructor&& c, Basechain::allocator<Allocator>) \
    { c(*this); }
#define OBJECT_CCTOR2_MACRO(x, y, field) ,field(a)
#define OBJECT_CCTOR2(NAME, FIELDS) \
    NAME() = delete; \
    public: \
    template<typename Constructor, typename Allocator> \
    NAME(Constructor&& c, Basechain::allocator<Allocator> a) \
    : id(0) BOOST_PP_SEQ_FOR_EACH(OBJECT_CCTOR2_MACRO, _, FIELDS) \
    { c(*this); }

#if WIN32
#define OBJECT_CCTOR(...)  BOOST_PP_CAT( BOOST_PP_OVERLOAD(OBJECT_CCTOR, __VA_ARGS__)(__VA_ARGS__), BOOST_PP_EMPTY() )
#else
#define OBJECT_CCTOR(...) BOOST_PP_OVERLOAD(OBJECT_CCTOR, __VA_ARGS__)(__VA_ARGS__)
#endif




namespace Xmaxplatform { namespace Chain {
   using                               std::map;
   using                               std::vector;
   using                               std::unordered_map;
   using                               std::string;
   using                               std::deque;
   using                               std::shared_ptr;
   using                               std::weak_ptr;
   using                               std::unique_ptr;
   using                               std::set;
   using                               std::pair;
   using                               std::enable_shared_from_this;
   using                               std::tie;
   using                               std::make_pair;

   using                               fc::smart_ref;
   using                               fc::variant_object;
   using                               fc::variant;
   using                               fc::enum_type;
   using                               fc::optional;
   using                               fc::unsigned_int;
   using                               fc::signed_int;
   using                               fc::time_point_sec;
   using                               fc::time_point;
   using                               fc::safe;
   using                               fc::flat_map;
   using                               fc::flat_set;
   using                               fc::static_variant;
   using                               fc::ecc::range_proof_type;
   using                               fc::ecc::range_proof_info;
   using                               fc::ecc::commitment_type;
   struct void_t{};

   using Basechain::allocator;
   using shared_string = boost::interprocess::basic_string<char, std::char_traits<char>, allocator<char>>;
   template<typename T>
   using shared_vector = boost::interprocess::vector<T, allocator<T>>;
   template<typename T>
   using shared_set = boost::interprocess::set<T, std::less<T>, allocator<T>>;

   using private_key_type = fc::ecc::private_key;
   using chain_id_type = fc::sha256;

   using Xmaxplatform::Basetypes::name;
   using action_name = name;
   using Xmaxplatform::Basetypes::account_name;
   using Xmaxplatform::Basetypes::permission_name;
   using Xmaxplatform::Basetypes::asset;
   using Xmaxplatform::Basetypes::share_type;
   using Xmaxplatform::Basetypes::authority;
   using Xmaxplatform::Basetypes::public_key;
   using Xmaxplatform::Basetypes::transaction;
   using Xmaxplatform::Basetypes::permission_name;
   using Xmaxplatform::Basetypes::type_name;
   using Xmaxplatform::Basetypes::func_name;
   using Xmaxplatform::Basetypes::time;
   using Xmaxplatform::Basetypes::field;
   using Xmaxplatform::Basetypes::string;
   using Xmaxplatform::Basetypes::uint8;
   using Xmaxplatform::Basetypes::uint16;
   using Xmaxplatform::Basetypes::uint32;
   using Xmaxplatform::Basetypes::uint64;
   using Xmaxplatform::Basetypes::uint128;
   using Xmaxplatform::Basetypes::uint256;
   using Xmaxplatform::Basetypes::int8;
   using Xmaxplatform::Basetypes::int16;
   using Xmaxplatform::Basetypes::int32;
   using Xmaxplatform::Basetypes::int64;
   using Xmaxplatform::Basetypes::int128;
   using Xmaxplatform::Basetypes::int256;
   using Xmaxplatform::Basetypes::uint128_t;

   using xmax_type_block_id = fc::sha256;
   using xmax_type_merkle_root = fc::sha256;
   using xmax_type_transaction_id = fc::sha256;
   using xmax_type_summary = fc::sha256;
   using xmax_type_generated_transaction_id = fc::sha256;
   using xmax_type_signature = fc::ecc::compact_signature;
   using xmax_type_weight = uint16_t;
   using bytes = Basetypes::bytes;

   using public_key_type = Xmaxplatform::Basetypes::public_key;


        enum object_type
        {
            null_object_type,
            static_config_object_type,
            dynamic_states_object_type,
            xmx_token_object_type,
            key_value_object_type,
            keystr_value_object_type,
            key64x64x64_value_object_type,
            key128x128_value_object_type,
            account_object_type,
			builder_object_type,
            vote_info_object_type,
            builder_info_object_type,
            resource_token_object_type,
			transaction_object_type,
			generated_transaction_object_type,
			block_summary_object_type,
			rate_limiting_object_type,
			block_object_type,
			erc20_token_object_type,
            OBJECT_TYPE_COUNT ///< Sentry value which contains the number of different object types
        };
   
} }  // Xmaxplatform::Chain

#define empty_chain_id Xmaxplatform::basemisc::empty_sha256

namespace fc {
  void to_variant(const Xmaxplatform::Chain::shared_vector<Xmaxplatform::Basetypes::field>& c, fc::variant& v);
  void from_variant(const fc::variant& v, Xmaxplatform::Chain::shared_vector<Xmaxplatform::Basetypes::field>& fields);
}
