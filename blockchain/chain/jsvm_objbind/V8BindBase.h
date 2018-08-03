#pragma once
#include <functional>
#include <map>
#include <libplatform/libplatform.h>
#include <v8.h>
#include "basetypes.hpp"

namespace Xmaxplatform {
	namespace Chain {
		class V8BindBase
		{
		public:
			V8BindBase()
			{

			}
			~V8BindBase()
			{

			}

			Basetypes::name GetTypeID()
			{
				return mScriptID;
			}

		protected:

			Basetypes::name mScriptID;



		};
	}
}