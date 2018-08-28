




//#include <chain_time.hpp>
//#include <publickey.hpp>

#include <basetypes.hpp>
#include <basechain.hpp>
#include <asset.hpp>
#include <string_utilities.hpp>
#include <fc/log/appender.hpp>


#include <objects/erc20_token_object.hpp>
#include <objects/account_object.hpp>
#include <objects/object_utility.hpp>
#include <objects/erc721_token_object.hpp>
#include <objects/erc721_token_account_object.hpp>
#include <objects/erc20_token_account_object.hpp>

 
#include <blockchain_exceptions.hpp>


using namespace Xmaxplatform::Chain;
using namespace Xmaxplatform::Basetypes;

namespace {
	
	//*** Utility functions

	template <class ExceptionType>
	static inline bool AlwaysPassCheckException(const ExceptionType& ex) {
		return true;
	}

	static inline bool IsDuplicateTokenNameEx(const duplicate_type_exception& ex) {
		std::string ex_msg = ex.to_string();
		return ex_msg.find("Mint ERC721 token already exist") != std::string::npos;
	}

	static inline bool IsDuplicateTransTokenNameException(const duplicate_type_exception& ex) {
		std::string ex_msg = ex.to_string();
		return ex_msg.find("ERC721 already got the token") != std::string::npos;
	}

	template <typename MultiIndexType>
	static inline const erc721_token_object_test& FindErc721ObjFromTable(MultiIndexType& tbl, const std::string& token_name, int token_decimals) {
		 auto it = tbl.get<by_token_name>().find(token_name_from_string(token_name));
		 return *it;
	}

	template <typename MultiIndexType>
	static inline const erc721_token_account_object_test& FindErc721AccountObjFromTable(MultiIndexType& tbl, const std::string& token_name, int token_decimals, const std::string& owner_name) {
		auto it = tbl.get<by_token_and_owner>().find(MakeErcTokenIndex(token_name, owner_name));
		return *it;
	}

	//*** Erc20 Utility functions

	template <typename MultiIndexType>
	static void AddErc20ObjToTable(MultiIndexType& tbl, erc20_token_object_test::id_type id, const std::string& token_name, int token_decimals,
		const std::string& owner_name, Xmaxplatform::Basetypes::share_type amount) {

		erc20_token_object_test obj;
		//erc20_token_object obj;
		obj.id = id;
		obj.token_name = token_name_from_string(token_name);
		obj.owner_name = xmax::string_to_name(owner_name.c_str());
		//obj.balance = amount;
		obj.total_supply = amount;
		tbl.insert(obj);
	}

	template <typename MultiIndexType>
	static void AddErc20AccountObjToTable(MultiIndexType& tbl, erc20_token_account_object::id_type id, const std::string& token_name, int token_decimals,
		const std::string& owner_name, int init_balance) {

		erc20_token_account_object obj;
		obj.id = id;
		obj.token_name = token_name_from_string(token_name);
		obj.owner_name = xmax::string_to_name(owner_name.c_str());
		obj.balance = static_cast<share_type>(init_balance);
		tbl.insert(obj);
	}

	template <typename MultiIndexType>
	static void MintErc20Tokens(MultiIndexType& tbl, const std::string& token_name, int token_decimals, int amount) {
		auto it = tbl.get<by_token_name>().find(token_name_from_string(token_name));

		XMAX_ASSERT(it != tbl.get<by_token_name>().end(), message_validate_exception,
			"Erc20 token:${token_name} has not created", ("token_name", token_name));

		share_type t_amount(amount);

		tbl.get<by_token_name>().modify(it, [t_amount](erc20_token_object_test& o) {
			//o.balance += t_amount;
			o.total_supply += t_amount;
		});
	}
	
	template <typename MultiIndexType>
	static void SendErc20TokensToAccount(MultiIndexType& account_table, const std::string& token_name, int token_decimals, const std::string& owner_name,
		Xmaxplatform::Basetypes::share_type amount) {

		auto it = account_table.get<by_token_and_owner>().find(MakeErcTokenIndex(token_name, owner_name));
		const erc20_token_account_object& obj = *it;

		XMAX_ASSERT(it != account_table.get<by_token_and_owner>().end(), message_validate_exception,
			"Erc20 token:${token_name} with account:${owner_name} has not created", ("token_name", token_name)("owner_name", owner_name));

		account_table.get<by_token_and_owner>().modify(it, [amount](erc20_token_account_object& o) {
			o.balance += amount;
		});
	}


	//*** Erc721 Utility functions

	template <typename MultiIndexType>
	static void AddErc721ObjToTable(MultiIndexType& tbl, erc721_token_object_test::id_type id, const std::string& token_name, int token_decimals,
		const std::string& owner_name/*, const Xmaxplatform::Chain::xmax_erc721_id& token_id*/) {

		erc721_token_object_test obj;
		obj.id = id;		
		obj.token_name = token_name_from_string(token_name);
		obj.owner_name = xmax::string_to_name(owner_name.c_str());
		//obj.minted_tokens.insert(token_id);
		tbl.get<by_token_name>().insert(obj);
	}

	template <typename MultiIndexType>
	static void AddErc721AccountObjToTable(MultiIndexType& tbl, erc721_token_account_object_test::id_type id, const std::string& token_name, int token_decimals,
		const std::string& owner_name) {
		erc721_token_account_object_test obj;
		obj.id = id;
		obj.token_name = token_name_from_string(token_name);
		obj.owner_name = xmax::string_to_name(owner_name.c_str());
		//obj.tokens.insert(token_id);
		tbl.insert(obj);
	}

	template <typename MultiIndexType>
	static void MintErc721Token(MultiIndexType& tbl, const std::string& token_name, int token_decimals, const Xmaxplatform::Chain::xmax_erc721_id& token_id) {
		auto it = tbl.get<by_token_name>().find(token_name_from_string(token_name));
	
		auto mint_it = it->minted_tokens.find(token_id);
		XMAX_ASSERT(mint_it == it->minted_tokens.end(), duplicate_type_exception, "Mint ERC721 token already exist : ${name}", ("name", token_name));

		tbl.get<by_token_name>().modify(it, [&token_id](erc721_token_object_test& o) {
			o.minted_tokens.insert(token_id);
		});
	}

	template <typename MultiIndexType>
	static void SendErc721TokenToAccount(MultiIndexType& tbl, const std::string& token_name, int token_decimals, const std::string& owner_name,
		const Xmaxplatform::Chain::xmax_erc721_id& token_id) {
		auto it = tbl.get<by_token_and_owner>().find(MakeErcTokenIndex(token_name, owner_name));
		const erc721_token_account_object_test& obj = *it;

		auto token_it = obj.tokens.find(token_id);
		XMAX_ASSERT(token_it == obj.tokens.end(), duplicate_type_exception, "The account ${owner} ERC721 already got the token ${token_name} with id : ${token_id}", 
			("owner", owner_name)("token_name", token_name)("token_id", token_id));


		tbl.get<by_token_and_owner>().modify(it, [&token_id](erc721_token_account_object_test& o) {
			o.tokens.insert(token_id);
		});
	}

	static erc20_token_multi_index_test& GetTestErc20Container() {
		static erc20_token_multi_index_test tbl;
		return tbl;
	}

	static erc20_token_account_multi_index_test& GetTestErc20TokenAccountTable() {
		static erc20_token_account_multi_index_test tbl;
		return tbl;
	}

	static erc721_token_multi_index_test& GetTestErc721Container() {
		static erc721_token_multi_index_test tbl;
		return tbl;
	}

	static erc721_token_account_multi_index_test& GetTestErc721TokenAccountTable() {
		static erc721_token_account_multi_index_test tbl;
		return tbl;
	}
	
}

namespace fc {
	extern std::unordered_map<std::string, logger>& get_logger_map();
	extern std::unordered_map<std::string, appender::ptr>& get_appender_map();
}

BOOST_AUTO_TEST_SUITE(erc20_test_suite)


BOOST_AUTO_TEST_CASE(erc20_test_issue) {
	auto& tbl = GetTestErc20Container();
			
	AddErc20ObjToTable(tbl, 1, "TST", 8, "testera", 12345);
	AddErc20ObjToTable(tbl, 2, "TSA", 8, "testera", 13579);
	AddErc20ObjToTable(tbl, 3, "TSC", 8, "testerb", 24680);
	
	//error case
	AddErc20ObjToTable(tbl, 4, "TST", 8, "testerb", 54321);
	AddErc20ObjToTable(tbl, 5, "TST", 8, "testera", 67890);

	BOOST_CHECK(tbl.size() == 3);
}

BOOST_AUTO_TEST_CASE(erc20_test_mint) {
	auto& tbl = GetTestErc20Container();

	MintErc20Tokens(tbl, "TST", 8, 20000);
	MintErc20Tokens(tbl, "TSA", 8, 10000);
	MintErc20Tokens(tbl, "TSC", 8, 5000);

	BOOST_CHECK_EXCEPTION({
		MintErc20Tokens(tbl, "TSD", 8, 12345);
		}, message_validate_exception, [](const message_validate_exception& ex) {
			std::string ex_msg = ex.to_string();
			return ex_msg.find("Erc20 token:TSD has not created") != std::string::npos;
		});

	BOOST_CHECK_EXCEPTION({
		MintErc20Tokens(tbl, "TSE", 8, 22345);
		}, message_validate_exception, AlwaysPassCheckException);
	
}

BOOST_AUTO_TEST_CASE(erc20_test_index) {
	auto& tbl = GetTestErc20Container();
	
	auto it = tbl.get<by_token_name>().find(token_name_from_string("TST"));
	BOOST_ASSERT(it != tbl.get<by_token_name>().end());
	auto& obj = *it;
	BOOST_CHECK(obj.id == erc20_token_object_test::id_type(1));
	BOOST_CHECK(obj.total_supply == 32345);

	auto it2 = tbl.get<by_token_name>().find(token_name_from_string("TSA"));
	BOOST_ASSERT(it2 != tbl.get<by_token_name>().end());
	auto& obj2 = *it2;
	BOOST_CHECK(obj2.id == erc20_token_object_test::id_type(2));
	BOOST_CHECK(obj2.total_supply == 23579);

	auto it3 = tbl.get<by_token_name>().find(token_name_from_string("TSC"));
	BOOST_ASSERT(it3 != tbl.get<by_token_name>().end());
	auto& obj3 = *it3;
	BOOST_CHECK(obj3.id == erc20_token_object_test::id_type(3));
	BOOST_CHECK(obj3.total_supply == 29680);
}

BOOST_AUTO_TEST_CASE(erc20_test_account) {
	auto& tbl = GetTestErc20TokenAccountTable();

	AddErc20AccountObjToTable(tbl, 1, "TSA", 8, "testera", 12345);
	AddErc20AccountObjToTable(tbl, 2, "TSA", 8, "testerb", 24680);
	AddErc20AccountObjToTable(tbl, 3, "TSB", 8, "testera", 13579);
	//Error cases
	AddErc20AccountObjToTable(tbl, 4, "TSB", 8, "testera", 24000);
	AddErc20AccountObjToTable(tbl, 5, "TSA", 8, "testerb", 37223);

	BOOST_CHECK(tbl.size() == 3);
}

BOOST_AUTO_TEST_CASE(erc20_test_account_send) {
	auto& tbl = GetTestErc20TokenAccountTable();

	SendErc20TokensToAccount(tbl, "TSA", 8, "testera", 10000);
	BOOST_CHECK(tbl.get<by_token_and_owner>().find(MakeErcTokenIndex("TSA", "testera"))->balance == 22345);

	SendErc20TokensToAccount(tbl, "TSB", 8, "testera", 20000);
	BOOST_CHECK(tbl.get<by_token_and_owner>().find(MakeErcTokenIndex("TSB", "testera"))->balance == 33579);

	BOOST_CHECK_EXCEPTION({
		SendErc20TokensToAccount(tbl, "TSC", 8, "testera", 20000);
		}, message_validate_exception, AlwaysPassCheckException);

}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(erc721_test_suite)

BOOST_AUTO_TEST_CASE(erc721_test_issue) {

	auto& tbl = GetTestErc721Container();

	AddErc721ObjToTable(tbl, 1, "AAA", 8, "testera");
	AddErc721ObjToTable(tbl, 2, "AAB", 8, "testera");
	AddErc721ObjToTable(tbl, 3, "AAC", 8, "testerb");
	AddErc721ObjToTable(tbl, 4, "AAD", 8, "testerc");
	//Error cases
	AddErc721ObjToTable(tbl, 5, "AAA", 8, "testerc");

	BOOST_CHECK(tbl.size() == 4);

}

BOOST_AUTO_TEST_CASE(erc721_test_mint) {
	auto& tbl = GetTestErc721Container();

	
	MintErc721Token(tbl, "AAA", 8, Xmaxplatform::Chain::xmax_erc721_id("f0e4c2f76c58916ec258f246851bea091d14d4247a2fc3e18694461b1816e13b"));
	MintErc721Token(tbl, "AAA", 8, Xmaxplatform::Chain::xmax_erc721_id("ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"));
	BOOST_CHECK_EXCEPTION({ 
		MintErc721Token(tbl, "AAA", 8, Xmaxplatform::Chain::xmax_erc721_id("f0e4c2f76c58916ec258f246851bea091d14d4247a2fc3e18694461b1816e13b"));
		}, duplicate_type_exception, IsDuplicateTokenNameEx);
	MintErc721Token(tbl, "AAA", 8, Xmaxplatform::Chain::xmax_erc721_id("88d4266fd4e6338d13b845fcf289579d209c897823b9217da3e161936f031589"));

	auto& obj = FindErc721ObjFromTable(tbl, "AAA", 8);
	BOOST_CHECK(obj.minted_tokens.size() == 3);
}

BOOST_AUTO_TEST_CASE(erc721_test_account) {
	auto& tbl = GetTestErc721TokenAccountTable();
	AddErc721AccountObjToTable(tbl, 1, "AAA", 8, "testera");
	AddErc721AccountObjToTable(tbl, 2, "AAB", 8, "testera");
	AddErc721AccountObjToTable(tbl, 3, "AAB", 8, "testerb");
	AddErc721AccountObjToTable(tbl, 4, "AAC", 8, "testerc");
	//Error case
	AddErc721AccountObjToTable(tbl, 5, "AAA", 8, "testera");

	BOOST_CHECK(tbl.size() == 4);

}

BOOST_AUTO_TEST_CASE(erc721_test_account_addtoken) {
	auto& tbl = GetTestErc721TokenAccountTable();


	SendErc721TokenToAccount(tbl, "AAA", 8, "testera", xmax_erc721_id{ "f0e4c2f76c58916ec258f246851bea091d14d4247a2fc3e18694461b1816e13b" });
	SendErc721TokenToAccount(tbl, "AAA", 8, "testera", xmax_erc721_id{ "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad" });
	BOOST_CHECK_EXCEPTION({
		SendErc721TokenToAccount(tbl, "AAA", 8, "testera", xmax_erc721_id("f0e4c2f76c58916ec258f246851bea091d14d4247a2fc3e18694461b1816e13b"));
		}, duplicate_type_exception, IsDuplicateTransTokenNameException);
	SendErc721TokenToAccount(tbl, "AAB", 8, "testera", xmax_erc721_id{ "f0e4c2f76c58916ec258f246851bea091d14d4247a2fc3e18694461b1816e13b" });
	SendErc721TokenToAccount(tbl, "AAB", 8, "testerb", xmax_erc721_id{ "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad" });

	auto& obj = FindErc721AccountObjFromTable(tbl, "AAA", 8, "testera");
	BOOST_CHECK(obj.tokens.size() == 2);
}

BOOST_AUTO_TEST_SUITE_END()