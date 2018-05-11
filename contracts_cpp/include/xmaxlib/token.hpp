
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

	typedef Xmaxplatform::token<uint64_t, XNAME(xmx)>   tokens;


	struct PACKED(transfer) {
	
		static const uint64_t action_type = XNAME(transfer);

		account_name  from;
		account_name  to;
		tokens        quantity;
		const uint8_t memo_length = 0;
	};
}
