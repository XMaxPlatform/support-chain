/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#include <empty.hpp>
#include <xmaxlib/xmax.hpp>

void inputvalue::set(uint64_t _code, uint64_t _action)
{
	_code = code;
	_action = action;
}

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
		//empty;
		inputvalue iv;
		if (action == XNAME(apply))
		{
			iv.set(code, action);
		}				
		
    }

} // extern "C"
