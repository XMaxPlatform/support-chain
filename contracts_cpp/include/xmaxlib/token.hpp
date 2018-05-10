
#pragma once
#include <xmaxlib/math.hpp>
#include <xmaxlib/utilities.hpp>
#include <xmaxlib/type_traits.hpp>


namespace Xmaxplatform {

	template<typename NumberType, uint64_t currency = XNAME(xmax) >
	struct token {
		using number_type = NumberType;
		static_assert(Xmaxplatform::is_unsigned<NumberType>::value, "NumberType can only be unsigned number");

		static const uint64_t currency_type = currency;

		token() : quantity{} {}

		template<typename T, typename = typename Xmaxplatform::enable_if<Xmaxplatform::is_unsigned<T>::value && !(sizeof(T)>sizeof(NumberType))>::type>
		explicit token(T v) :quantity(v) {};

		NumberType quantity;

		token& operator-=(const token& a) {
			assert(quantity >= a.quantity, "integer underflow subtracting token balance");
			quantity -= a.quantity;
			return *this;
		}

		token& operator+=(const token& a) {
			assert(quantity + a.quantity >= a.quantity, "integer overflow adding token balance");
			quantity += a.quantity;
			return *this;
		}

		inline friend token operator+(const token& a, const token& b) {
			token result = a;
			result += b;
			return result;
		}

		inline friend token operator-(const token& a, const token& b) {
			token result = a;
			result -= b;
			return result;
		}

		friend bool operator <= (const token& a, const token& b) { return a.quantity <= b.quantity; }

		friend bool operator <  (const token& a, const token& b) { return a.quantity <  b.quantity; }

		friend bool operator >= (const token& a, const token& b) { return a.quantity >= b.quantity; }

		friend bool operator >  (const token& a, const token& b) { return a.quantity >  b.quantity; }

		friend bool operator == (const token& a, const token& b) { return a.quantity == b.quantity; }

		friend bool operator != (const token& a, const token& b) { return a.quantity != b.quantity; }

		explicit operator bool()const { return quantity != 0; }

		inline void print() {
		//	Xmaxplatform::print(quantity, " ", name(currency_type));
		}
	};


	template<typename BaseToken, typename QuoteToken>
	struct price
	{
		typedef BaseToken  base_token_type;
		typedef QuoteToken quote_token_type;
		
		static const uint64_t precision = 1000ll * 1000ll * 1000ll * 1000ll * 1000ll;

		price() :base_per_quote((uint128_t)1ul) {}

		price(BaseToken base, QuoteToken quote) {
			assert(base >= BaseToken(1ul), "invalid price");
			assert(quote >= QuoteToken(1ul), "invalid price");

			base_per_quote = base.quantity;
			base_per_quote *= precision;
			base_per_quote /= quote.quantity;
		}

		friend QuoteToken operator / (BaseToken b, const price& q) {
			//Xmaxplatform::print("operator/ ", uint128(b.quantity), " * ", uint128(precision), " / ", q.base_per_quote, "\n");
			return QuoteToken(uint64_t((uint128(b.quantity) * uint128(precision) / q.base_per_quote)));
		}

		friend BaseToken operator * (const QuoteToken& b, const price& q) {
			//Xmaxplatform::print("b: ", b, " \n");
			//Xmaxplatform::print("operator* ", uint128(b.quantity), " * ", uint128(q.base_per_quote), " / ", precision, "\n");
			//return QuoteToken( uint64_t( mult_div_i128( b.quantity, q.base_per_quote, precision ) ) );
			return BaseToken(uint64_t((b.quantity * q.base_per_quote) / precision));
		}

		friend bool operator <= (const price& a, const price& b) { return a.base_per_quote <= b.base_per_quote; }

		friend bool operator <  (const price& a, const price& b) { return a.base_per_quote <  b.base_per_quote; }

		friend bool operator >= (const price& a, const price& b) { return a.base_per_quote >= b.base_per_quote; }

		friend bool operator >  (const price& a, const price& b) { return a.base_per_quote >  b.base_per_quote; }

		friend bool operator == (const price& a, const price& b) { return a.base_per_quote == b.base_per_quote; }

		friend bool operator != (const price& a, const price& b) { return a.base_per_quote != b.base_per_quote; }

		inline void print() {
		//	Xmaxplatform::print(base_per_quote, ".", " ", name(base_token_type::currency_type), "/", name(quote_token_type::currency_type));
		}
	private:
		Xmaxplatform::uint128 base_per_quote;
	};


	typedef Xmaxplatform::token<uint64_t, XNAME(xmx)>   tokens;


	struct PACKED(transfer) {
	
		static const uint64_t action_type = XNAME(transfer);

		account_name  from;
		account_name  to;
		tokens        quantity;
		const uint8_t memo_length = 0;
	};
}
