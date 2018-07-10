#define BOOST_TEST_MODULE chain_test




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

#include <boost/test/included/unit_test.hpp>



using namespace Xmaxplatform::Chain;
using namespace Xmaxplatform::Basetypes;

//*** Utility functions
namespace {

	static inline bool IsDuplicateTokenNameEx(const duplicate_type_exception& ex) {
		std::string ex_msg = ex.to_string();
		return ex_msg.find("Mint ERC721 token already exist") != std::string::npos;
	}

	static inline bool IsDuplicateTransTokenNameException(const duplicate_type_exception& ex) {
		std::string ex_msg = ex.to_string();
		return ex_msg.find("ERC721 already got the token") != std::string::npos;
	}

	template <typename MultiIndexType>
	static inline const erc721_token_object_test& FindErc721ObjFromTable(MultiIndexType& tbl, const std::string& token_name) {
		 auto it = tbl.get<by_token_name>().find(token_name_from_string(token_name));
		 return *it;
	}

	template <typename MultiIndexType>
	static inline const erc721_token_account_object_test& FindErc721AccountObjFromTable(MultiIndexType& tbl, const std::string& token_name, const std::string& owner_name) {
		auto it = tbl.get<by_token_and_owner>().find(MakeErcTokenIndex(token_name, owner_name));
		return *it;
	}

	template <typename MultiIndexType>
	static void AddErc20ObjToTable(MultiIndexType& tbl, erc20_token_object::id_type id, const std::string& token_name,		
		const std::string& owner_name, Xmaxplatform::Basetypes::share_type amount) {

		erc20_token_object obj;
		//erc20_token_object obj;
		obj.id = id;
		obj.token_name = token_name_from_string(token_name);
		obj.owner_name = xmax::string_to_name(owner_name.c_str());
		obj.token_amount = amount;
		tbl.get<by_token_and_owner>().insert(obj);
	}

	template <typename MultiIndexType>
	static void AddErc721ObjToTable(MultiIndexType& tbl, erc721_token_object_test::id_type id, const std::string& token_name,
		const std::string& owner_name/*, const Xmaxplatform::Chain::xmax_erc721_id& token_id*/) {

		erc721_token_object_test obj;
		obj.id = id;		
		obj.token_name = token_name_from_string(token_name);
		obj.owner_name = xmax::string_to_name(owner_name.c_str());
		//obj.minted_tokens.insert(token_id);
		tbl.get<by_token_name>().insert(obj);
	}

	template <typename MultiIndexType>
	static void AddErc721AccountObjToTable(MultiIndexType& tbl, erc721_token_account_object_test::id_type id, const std::string& token_name,
		const std::string& owner_name) {
		erc721_token_account_object_test obj;
		obj.id = id;
		obj.token_name = token_name_from_string(token_name);
		obj.owner_name = xmax::string_to_name(owner_name.c_str());
		//obj.tokens.insert(token_id);
		tbl.insert(obj);
	}

	template <typename MultiIndexType>
	static void MintErc721Token(MultiIndexType& tbl, const std::string& token_name, const Xmaxplatform::Chain::xmax_erc721_id& token_id) {
		auto it = tbl.get<by_token_name>().find(token_name_from_string(token_name));		
	
		auto mint_it = it->minted_tokens.find(token_id);
		XMAX_ASSERT(mint_it == it->minted_tokens.end(), duplicate_type_exception, "Mint ERC721 token already exist : ${name}", ("name", token_name));

		tbl.get<by_token_name>().modify(it, [&token_id](erc721_token_object_test& o) {
			o.minted_tokens.insert(token_id);
		});
	}

	template <typename MultiIndexType>
	static void SendErc20TokenToAccount(MultiIndexType& tbl, const std::string& token_name, const std::string& owner_name,
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


BOOST_AUTO_TEST_CASE(erc20_test_add) {
	auto& tbl = GetTestErc20Container();
			
	AddErc20ObjToTable(tbl, 1, "TST", "testera", 12345);
	AddErc20ObjToTable(tbl, 2, "TST", "testerb", 54321);
	AddErc20ObjToTable(tbl, 3, "TSA", "testera", 13579);
	AddErc20ObjToTable(tbl, 4, "TSA", "testerb", 24680);
	//error case
	AddErc20ObjToTable(tbl, 5, "TST", "testera", 67890);

	BOOST_CHECK(tbl.size() == 4);
}

BOOST_AUTO_TEST_CASE(erc20_test_index) {
	auto& tbl = GetTestErc20Container();
	
	auto it = tbl.get<by_token_and_owner>().find(MakeErcTokenIndex("TST", "testera"));
	BOOST_ASSERT(it != tbl.get<by_token_and_owner>().end());
	auto& obj = *it;
	BOOST_CHECK(obj.id == erc20_token_object::id_type(1));
	BOOST_CHECK(obj.token_amount == 12345);

	auto it2 = tbl.get<by_token_and_owner>().find(MakeErcTokenIndex("TST", "testerb"));
	BOOST_ASSERT(it2 != tbl.get<by_token_and_owner>().end());
	auto& obj2 = *it2;
	BOOST_CHECK(obj2.id == erc20_token_object::id_type(2));
	BOOST_CHECK(obj2.token_amount == 54321);

	auto it3 = tbl.get<by_token_and_owner>().find(MakeErcTokenIndex("TSA", "testera"));
	BOOST_ASSERT(it3 != tbl.get<by_token_and_owner>().end());
	auto& obj3 = *it3;
	BOOST_CHECK(obj3.id == erc20_token_object::id_type(3));
	BOOST_CHECK(obj3.token_amount == 13579);

	auto it4 = tbl.get<by_token_and_owner>().find(MakeErcTokenIndex("TSA", "testerb"));
	BOOST_ASSERT(it4 != tbl.get<by_token_and_owner>().end());
	auto& obj4 = *it4;
	BOOST_CHECK(obj4.id == erc20_token_object::id_type(4));
	BOOST_CHECK(obj4.token_amount == 24680);
}



BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(erc721_test_suite)

BOOST_AUTO_TEST_CASE(erc72_test_issue) {

	auto& tbl = GetTestErc721Container();

	AddErc721ObjToTable(tbl, 1, "AAA", "testera");
	AddErc721ObjToTable(tbl, 2, "AAB", "testera");
	AddErc721ObjToTable(tbl, 3, "AAC", "testerb");
	AddErc721ObjToTable(tbl, 4, "AAD", "testerc");
	//Error cases
	AddErc721ObjToTable(tbl, 5, "AAA", "testerc");

	BOOST_CHECK(tbl.size() == 4);

}

BOOST_AUTO_TEST_CASE(erc721_test_mint) {
	auto& tbl = GetTestErc721Container();

	
	MintErc721Token(tbl, "AAA", Xmaxplatform::Chain::xmax_erc721_id("f0e4c2f76c58916ec258f246851bea091d14d4247a2fc3e18694461b1816e13b"));
	MintErc721Token(tbl, "AAA", Xmaxplatform::Chain::xmax_erc721_id("ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"));
	BOOST_CHECK_EXCEPTION({ 
		MintErc721Token(tbl, "AAA", Xmaxplatform::Chain::xmax_erc721_id("f0e4c2f76c58916ec258f246851bea091d14d4247a2fc3e18694461b1816e13b"));
		}, duplicate_type_exception, IsDuplicateTokenNameEx);
	MintErc721Token(tbl, "AAA", Xmaxplatform::Chain::xmax_erc721_id("88d4266fd4e6338d13b845fcf289579d209c897823b9217da3e161936f031589"));

	auto& obj = FindErc721ObjFromTable(tbl, "AAA");
	BOOST_CHECK(obj.minted_tokens.size() == 3);
}

BOOST_AUTO_TEST_CASE(erc721_test_account) {
	auto& tbl = GetTestErc721TokenAccountTable();
	AddErc721AccountObjToTable(tbl, 1, "AAA", "testera");
	AddErc721AccountObjToTable(tbl, 2, "AAB", "testera");
	AddErc721AccountObjToTable(tbl, 3, "AAB", "testerb");
	AddErc721AccountObjToTable(tbl, 4, "AAC", "testerc");
	//Error case
	AddErc721AccountObjToTable(tbl, 5, "AAA", "testera");

	BOOST_CHECK(tbl.size() == 4);

}

BOOST_AUTO_TEST_CASE(erc721_test_account_addtoken) {
	auto& tbl = GetTestErc721TokenAccountTable();


	SendErc20TokenToAccount(tbl, "AAA", "testera", xmax_erc721_id{ "f0e4c2f76c58916ec258f246851bea091d14d4247a2fc3e18694461b1816e13b" });
	SendErc20TokenToAccount(tbl, "AAA", "testera", xmax_erc721_id{ "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad" });
	BOOST_CHECK_EXCEPTION({
		SendErc20TokenToAccount(tbl, "AAA", "testera", xmax_erc721_id("f0e4c2f76c58916ec258f246851bea091d14d4247a2fc3e18694461b1816e13b"));
		}, duplicate_type_exception, IsDuplicateTransTokenNameException);
	SendErc20TokenToAccount(tbl, "AAB", "testera", xmax_erc721_id{ "f0e4c2f76c58916ec258f246851bea091d14d4247a2fc3e18694461b1816e13b" });
	SendErc20TokenToAccount(tbl, "AAB", "testerb", xmax_erc721_id{ "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad" });

	auto& obj = FindErc721AccountObjFromTable(tbl, "AAA", "testera");
	BOOST_CHECK(obj.tokens.size() == 2);
}

BOOST_AUTO_TEST_SUITE_END()