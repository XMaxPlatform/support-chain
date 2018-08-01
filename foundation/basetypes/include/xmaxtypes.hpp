#pragma once

#include <basetypes.hpp>
#include <publickey.hpp>
#include <asset.hpp>
#include <gentypes.hpp>



namespace Xmaxplatform {
namespace Basetypes {


	inline bool operator== (const account_permission& lhs, const account_permission& rhs) {
		return std::tie(lhs.account, lhs.authority) == std::tie(rhs.account, rhs.authority);
	}

	inline bool operator!= (const account_permission& lhs, const account_permission& rhs) {
		return std::tie(lhs.account, lhs.authority) != std::tie(rhs.account, rhs.authority);
	}

	inline bool operator< (const account_permission& lhs, const account_permission& rhs) {
		return std::tie(lhs.account, lhs.authority) < std::tie(rhs.account, rhs.authority);
	}

	inline bool operator<= (const account_permission& lhs, const account_permission& rhs) {
		return std::tie(lhs.account, lhs.authority) <= std::tie(rhs.account, rhs.authority);
	}

	inline bool operator> (const account_permission& lhs, const account_permission& rhs) {
		return std::tie(lhs.account, lhs.authority) > std::tie(rhs.account, rhs.authority);
	}

	inline bool operator>= (const account_permission& lhs, const account_permission& rhs) {
		return std::tie(lhs.account, lhs.authority) >= std::tie(rhs.account, rhs.authority);
	}
}
}