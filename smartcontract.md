# Xmax smart contract
Xmax blockchain smart contract mannual.

# Table of contents
1. [Example - Create testcontract in Windows](#examplewindows)



<a name="examplewindows"></a>
## Example: Create a custom smart contract - testcontract

### Create smart contract folder
Crate a smart contract folder `testcontract` anywhere, such as in `contract_code` folder.

``` bash
mkdir testcontract
```

### Create smart contract files
Smart contract is written in C++ language, so we need to create a C++ header file and source file: `testcontract.hpp` and `testcontract.cpp` in the folder.

###  Add the necessary include file commands and namespace to the header file.

Normally you may need these headers:
```C++
#include <xmaxlib/xmax.hpp>
#include <xmaxlib/core.h>
#include <xmaxlib/database.hpp>

namespace testcontract {

}
```

### Add the necessary `init` and `apply` function to the cpp source file
```C++
#include <testcontract.hpp>

/**
 *  The init() and apply() methods must have C calling convention so that the blockchain can lookup and
 *  call these methods.
 */
extern "C" {
     /**
     *  This method is called once when the contract is published or updated.
     */
    void init()  {        
      
    }

    /// The apply method implements the dispatch of events to this contract
    void apply( uint64_t code, uint64_t action ) {
       
    }
}
```


### Add the contract message struct

```C++
//...
#include <xmaxlib/token.hpp>

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
}
```
The comments part: `@abi action msg` of the struct is necessary for parsing. 

### Add the message dispatch logic in `apply` function and add the message handle function.

 
```C++

#include <testcontract.hpp>

namespace testcontract {

    void apply_testaction( const testcontract::testaction& msg) {

    }
}

using namespace testcontract;

/**
 *  The init() and apply() methods must have C calling convention so that the blockchain can lookup and
 *  call these methods.
 */
extern "C" {

    /**
     *  This method is called once when the contract is published or updated.
     */
    void init()  {        
        
    }

    /// The apply method implements the dispatch of events to this contract
    void apply( uint64_t code, uint64_t action ) {
        //Dispatch call method accroding to the code and action
        if( code == XNAME(testcontract)) {
            if( action == XNAME(testaction)) {
                apply_testaction( Xmaxplatform::current_message<testcontract::testaction>() );
            }


        }
    }
```
You need to make sure the code's name is same as the contract: `testcontract`, using the `XNAME` macro to decode the name string. Then dispatch the function call according to the action name. Get and convert the current message data to the specific message struct using the `Xmaxplatform::current_message` template method.


### Impelment the message business logic.

#### Add data table to save business data

Add bellow struct to `testcontract.hpp`
```C++
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
```

#### Add contract init logic

```C++
void init()  {        
        testdata data(XNAME(testcontract));
        if ( !testdatas::get(data, XNAME(testcontract) ))
            testdatas::store(testdata(XNAME(testcontract), testcontract::test_tokens(1000000000ull)), XNAME(testcontract));
    }
```
We set contract account `testcontract` with the init tokens.


#### Handle message logic

When received message, we check the quantity validation first, return if not valid. Then we check the contract account for the account specified by the message, create one if not exist. Finally we add the token to the account balance.
```C++
void apply_testaction( const testcontract::testaction& msg) {
      
        if(msg.quantity.quantity <= 0 || msg.quantity.quantity > max_quantity) {
            return;
        }

         testdata data(msg.account);         

         if(!testdatas::get(msg.account, data, XNAME(testcontract))) {
             data.balance = msg.quantity;
             testdatas::store(data, XNAME(testcontract));
             return;
         }
         else {
             if(data.balance.quantity > max_quantity - msg.quantity.quantity) {
                 return;
             }

             data.balance.quantity += msg.quantity.quantity;
             testdatas::store(data, XNAME(testcontract));
         }
    }
```
