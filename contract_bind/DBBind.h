#pragma once
#include <functional>
#include <map>
#include <libplatform/libplatform.h>
#include <v8.h>
#include <jsvm_objbind/V8BindObject.h>
#include "basetypes.hpp"

using namespace v8;

namespace Xmaxplatform {
	namespace Chain {


		void LoadRecord(const v8::FunctionCallbackInfo<v8::Value>& args);
		void StoreRecord(const v8::FunctionCallbackInfo<v8::Value>& args);



		class V8TableI128 :public V8BindObject<V8TableI128>
		{
		public:
			V8TableI128(Basetypes::name _scope, Basetypes::name _code, Basetypes::name _table)
				:mScope(mScope)
				,mCode(mCode)
				,mTable(mTable)
			{}

			//V8 bind
			static constexpr inline const char* TypeName() {
				return "V8TableI128";
			}

			static _CONST_EXPR_ inline Basetypes::name TypeID() {
				return Basetypes::name("V8TableI128");
			}

			static V8TableI128* NewV8CppObj(const v8::FunctionCallbackInfo<v8::Value>& args);
			static void RegisterWithV8(v8::Isolate* isolate, v8::Handle<v8::ObjectTemplate> global);
			static void ConstructV8Object(const v8::FunctionCallbackInfo<v8::Value>& args);
			static void WeakExternalReferenceCallback(const v8::WeakCallbackInfo<V8TableI128>& data);

			void Store(Basetypes::name key, void* data, uint32_t dataLen);
			void Load(Basetypes::name key, void* data, uint32_t dataLen);
			
			void CacheDataType(uint32_t length, Basetypes::name type);

			uint32_t GetCacheDataLength()
			{
				return mDataLength;
			}
			Basetypes::name GetCacheDataType()
			{
				return mDataType;
			}
		protected:
			Basetypes::name mScope;
			Basetypes::name mCode;
			Basetypes::name mTable;
			uint32_t mDataLength;
			Basetypes::name mDataType;

		};

		
	}
}