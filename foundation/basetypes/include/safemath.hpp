#pragma once
#include "exception_macros.hpp"

namespace Xmaxplatform {
	namespace Basetypes {
		namespace SafeMath {

			template<typename T>
			T mul(T a, T b) {
				if (a == static_cast<T>(0)) {
					return static_cast<T>(0);
				}

				T c = a * b;
				XMAX_ASSERT(c / a == b);
				return c;
			}

			template<typename T>
			T div(T a, T b) {
				return a / b;
			}

			template<typename T>
			T sub(T a, T b) {				
				XMAX_ASSERT(b <= a);
				return a - b;
			}

			template<typename T>
			T add(T a, T b) {
				T c = a + b;
				XMAX_ASSERT(c >= a);
				return c;
			}


		}
	}
}