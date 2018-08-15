/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once
#include <fc/exception/exception.hpp>
#include <basetypes.hpp>

#define MAKE_TOKEN_NAME(C1, C2, C3, DECIMALS) (int64_t(DECIMALS) | (uint64_t(C1) << 8) | (uint64_t(C2) << 16) | (uint64_t(C3) << 24))



#define GET_MACRO(_1, _2, _3, _4, _5, _6, _7, NAME, ...) NAME
#define PACK_TOKEN_NAME3(C1, C2, C3)  ((uint64_t(C1) << 8) | (uint64_t(C2) << 16) | (uint64_t(C3) << 24))
#define PACK_TOKEN_NAME4(C1, C2, C3, C4)  ((uint64_t(C1) << 8) | (uint64_t(C2) << 16) | (uint64_t(C3) << 24) | (uint64_t(C4) << 32))
#define PACK_TOKEN_NAME5(C1, C2, C3, C4, C5)  ((uint64_t(C1) << 8) | (uint64_t(C2) << 16) | (uint64_t(C3) << 24) | (uint64_t(C4) << 32) | (uint64_t(C5) << 40))
#define PACK_TOKEN_NAME6(C1, C2, C3, C4, C5, C6)  ((uint64_t(C1) << 8) | (uint64_t(C2) << 16) | (uint64_t(C3) << 24) | (uint64_t(C4) << 32) | (uint64_t(C5) << 40) | (uint64_t(C6) << 48))
#define PACK_TOKEN_NAME7(C1, C2, C3, C4, C5, C6, C7)  ((uint64_t(C1) << 8) | (uint64_t(C2) << 16) | (uint64_t(C3) << 24) | (uint64_t(C4) << 32) | (uint64_t(C5) << 40) | (uint64_t(C6) << 48) | (uint64_t(C7) << 56))

#define PACK_TOKEN_NAME(...) GET_MACRO(__VA_ARGS__, PACK_TOKEN_NAME7, PACK_TOKEN_NAME6, PACK_TOKEN_NAME5, PACK_TOKEN_NAME4, PACK_TOKEN_NAME3)(__VA_ARGS__)
#define MAKE_TOKEN_SYMBOL(DECIMALS, ...) (int64_t(DECIMALS) | PACK_TOKEN_NAME(__VA_ARGS__))


/// xmax with 8 digits of precision
#define MAIN_SYMBOL  MAKE_TOKEN_SYMBOL(8, 'S', 'U', 'P')

/// Defined to be largest power of 10 that fits in 53 bits of precision
#define MAIN_TOKEN_MAX_SHARE_SUPPLY   int64_t(1'000'000'000'000'000ll)

namespace Xmaxplatform { namespace Basetypes {

   using asset_symbol = uint64_t;
   using share_type   = int64;


   inline asset_symbol token_name_from_string(const string& token_str, int decimals_precision) {
	   auto char_count = token_str.size();
	   XMAX_ASSERT(char_count <= 7 && char_count >= 3, invalid_field_name_exception, "Token name invalide: ${name}", ("name", token_str));
	   asset_symbol res = int64_t(decimals_precision);
	   for (unsigned int i = 0; i < char_count; ++i)
	   {
		   res |= token_str[i] << 8 * (i + 1);
	   }	   
	   return asset_symbol(res);
   }

   struct asset
   {
      asset(share_type a = 0, asset_symbol id = MAIN_SYMBOL)
      :amount(a),symbol(id){}

      share_type   amount;
      asset_symbol symbol;

      double to_real()const { return static_cast<double>(amount) / precision(); }

      uint8_t     decimals()const;
      string      symbol_name()const;
      int64_t     precision()const;
      void        set_decimals(uint8_t d);

      static asset from_string(const string& from);
      string       to_string()const;

      asset& operator += (const asset& o)
      {
         FC_ASSERT(symbol == o.symbol);
         amount += o.amount;
         return *this;
      }

      asset& operator -= (const asset& o)
      {
         FC_ASSERT(symbol == o.symbol);
         amount -= o.amount;
         return *this;
      }
      asset operator -()const { return asset(-amount, symbol); }

      friend bool operator == (const asset& a, const asset& b)
      {
         return std::tie(a.symbol, a.amount) == std::tie(b.symbol, b.amount);
      }
      friend bool operator < (const asset& a, const asset& b)
      {
         FC_ASSERT(a.symbol == b.symbol);
         return std::tie(a.amount,a.symbol) < std::tie(b.amount,b.symbol);
      }
      friend bool operator <= (const asset& a, const asset& b) { return (a == b) || (a < b); }
      friend bool operator != (const asset& a, const asset& b) { return !(a == b); }
      friend bool operator > (const asset& a, const asset& b)  { return !(a <= b); }
      friend bool operator >= (const asset& a, const asset& b) { return !(a < b);  }

      friend asset operator - (const asset& a, const asset& b) {
         FC_ASSERT(a.symbol == b.symbol);
         return asset(a.amount - b.amount, a.symbol);
      }

      friend asset operator + (const asset& a, const asset& b) {
         FC_ASSERT(a.symbol == b.symbol);
         return asset(a.amount + b.amount, a.symbol);
      }

      friend std::ostream& operator << (std::ostream& out, const asset& a) { return out << a.to_string(); }

   };

   struct price
   {
      asset base;
      asset quote;

      price(const asset& base = asset(), const asset quote = asset())
      :base(base),quote(quote){}

      static price max(asset_symbol base, asset_symbol quote);
      static price min(asset_symbol base, asset_symbol quote);

      price max()const { return price::max(base.symbol, quote.symbol); }
      price min()const { return price::min(base.symbol, quote.symbol); }

      double to_real()const { return base.to_real() / quote.to_real(); }

      bool is_null()const;
      void validate()const;
   };

   price operator / (const asset& base, const asset& quote);
   inline price operator~(const price& p) { return price{p.quote,p.base}; }

   bool  operator <  (const asset& a, const asset& b);
   bool  operator <= (const asset& a, const asset& b);
   bool  operator <  (const price& a, const price& b);
   bool  operator <= (const price& a, const price& b);
   bool  operator >  (const price& a, const price& b);
   bool  operator >= (const price& a, const price& b);
   bool  operator == (const price& a, const price& b);
   bool  operator != (const price& a, const price& b);
   asset operator *  (const asset& a, const price& b);

}} // namespace Xmaxplatform::Basetypes

namespace fc {
    inline void to_variant(const Xmaxplatform::Basetypes::asset& var, fc::variant& vo) { vo = var.to_string(); }
    inline void from_variant(const fc::variant& var, Xmaxplatform::Basetypes::asset& vo) {
       vo = Xmaxplatform::Basetypes::asset::from_string(var.get_string());
    }
}

FC_REFLECT(Xmaxplatform::Basetypes::asset, (amount)(symbol))
FC_REFLECT(Xmaxplatform::Basetypes::price, (base)(quote))

