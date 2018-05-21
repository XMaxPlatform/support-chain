#include <testcontract.hpp>

const uint64_t max_quantity = 9000000000ull;

namespace testcontract {

    void apply_testaction( const testcontract::testaction& msg) {
        prints("a");

        if(msg.quantity.quantity <= 0 || msg.quantity.quantity > max_quantity) {
            return;
        }

        prints("b");
         testdata data(msg.account);         

         if(!testdatas::get(msg.account, data, XNAME(testcontract))) {
             data.balance = msg.quantity;
             testdatas::store(data, XNAME(testcontract));
             prints("cc");
             return;
         }
         else {
             if(data.balance.quantity > max_quantity - msg.quantity.quantity) {
                 prints("d");
                 return;
             }

             data.balance.quantity += msg.quantity.quantity;
             testdatas::store(data, XNAME(testcontract));
             prints("e");
         }
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
        testdata data(XNAME(testcontract));
        if ( !testdatas::get(data, XNAME(testcontract) ))
            testdatas::store(testdata(XNAME(testcontract), testcontract::test_tokens(1000000000ull)), XNAME(testcontract));
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
}