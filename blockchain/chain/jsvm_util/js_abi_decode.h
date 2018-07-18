#pragma once
#include <functional>
#include <map>
#include <libplatform/libplatform.h>
#include <v8.h>
#include <fc/variant.hpp>

using namespace v8;

namespace Xmaxplatform {
	namespace Chain {
		struct js_abi_decode {
			fc::variant JsonStrToVar(const std::string& json);

			fc::variant MessageByteToVar(const std::string& json);
			template<typename T> void GetFeildFromVar(const std::string& feildName,const fc::variant& var,T& feild);

// #define GET_FIELD( VO, FIELD, RESULT ) \
//        if( VO.contains(#FIELD) ) fc::from_variant( VO[#FIELD], RESULT.FIELD )

		};

		template<typename T>
		void GetFeildFromVar(const std::string& feildName, const fc::variant& var,T& feild)
		{
			const variant_object& vo = var.get_object();
			if (vo.contains(feildName.c_str()))
			{
				fc::from_variant(vo[feildName.c_str()], feild)
			}
		}

	}
}