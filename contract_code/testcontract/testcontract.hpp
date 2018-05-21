#include <xmaxlib/xmax.hpp>
#include <xmaxlib/core.h>
#include <xmaxlib/database.hpp>
#include <xmaxlib/token.hpp>
#include <xmaxlib/message.hpp>
#include <xmaxlib/transaction.hpp>

namespace testcontract
{
typedef Xmaxplatform::token<uint64_t, XNAME(testcontract)> test_tokens;

/**
     * @brief test action
     * @abi action msg
     */
struct testaction
{
    /**
     *  test account field
     */
    account_name account;

    /**
     *  account to transfer to
     */
    test_tokens quantity;
};

/**
     * @brief Test data for store data
     * @abi table testdata
     */
struct testdata
{
    uint64_t key;

    test_tokens balance;

    testdata(uint64_t key = XNAME(testcontract), test_tokens b = test_tokens()):key(key), balance(b) {}
};

/**
   Defines the database table for test information
   **/
using testdatas = Xmaxplatform::table<XNAME(defaultscope), XNAME(testcontract), XNAME(testdata), testdata, uint64_t>;

} // namespace testcontract
