#include <cat.hpp>
#include<xmaxlib/message.hpp>

namespace MYNAME {
    using namespace Xmaxplatform;

    void add_account(uint64_t name, const account& a) {
        accounts::store(a, name);
    }

    void update_account(uint64_t name, const account& a) {
        accounts::update(a, name);
    }

    void remove_account(uint64_t name, const account& a) {
        accounts::remove(a, name);
    }

    account get_account(uint64_t name, uint64_t pk) {
        account me;
        accounts::get(pk, me, name);
        return me;
    }

    void add_cat(uint64_t name, const cat& a) {
        cats::store(a, name);
    }

    void update_cat(uint64_t name, const cat& a) {
        cats::update(a, name);
    }

    void remove_cat(uint64_t name, const cat& a) {
        cats::remove(a, name);
    }

    cat get_cat(uint64_t name, uint64_t pk) {
        cat one;
        cats::get(pk, one, name);
        return one;
    }

    cat back_cat(uint64_t name) {
        cat one;
        cats::back(one, name);
        return one;
    }

    uint64_t max_id() {
        cat one = back_cat(XNAME(cat));
        return one.id;
    }

    void add_own(uint64_t name, const catown& a) {
        catowns::store(a, name);
    }

    void update_own(uint64_t name, const catown& a) {
        catowns::update(a, name);
    }

    void remove_own(uint64_t name, const catown& a) {
        catowns::remove(a, name);
    }

    catown get_own(uint64_t name, uint64_t pk) {
        catown own;
        catowns::get(pk, own, name);
        return own;
    }

    void add_sell(uint64_t name, const catsell& a) {
        catsells::store(a, name);
    }

    void update_sell(uint64_t name, const catsell& a) {
        catsells::update(a, name);
    }

    void remove_sell(uint64_t name, const catsell& a) {
        catsells::remove(a, name);
    }

    catsell get_sell(uint64_t name, uint64_t pk) {
        catsell sell;
        catsells::get(pk, sell, name);
        return sell;
    }

    void xmax_transfer(uint64_t from, uint64_t to, uint64_t amount) {
		/*
		 Xmaxplatform::transfer trans;
      trans.from = from;
      trans.to = to;
      trans.quantity = Xmaxplatform::tokens(amount); 
      
      Xmaxplatform::message Msg(XNAME(xmax), XNAME(transfer), trans);
      Msg.add_permissions(from, XNAME(active));
      Msg.send();
		*/
     
    }

    uint64_t rand(uint64_t seed) {
        uint64_t z = (seed += uint64_t(0x9E3779B97F4A7C15));
        z = (z ^ (z >> 30)) * uint64_t(0xBF58476D1CE4E5B9);
        z = (z ^ (z >> 27)) * uint64_t(0x94D049BB133111EB);
        return z ^ (z >> 31);
    }

    uint64_t get_one_gene_value() {
        return rand(xmax_now())%10 + 1;
    }

    uint64_t get_one_gene_value_Offset(uint64_t offset) {
        return rand(xmax_now()+offset)%10 + 1;
    }

    uint64_t get_gene(uint64_t f, uint64_t m) {
        uint64_t c = 0;
        uint64_t value = get_one_gene_value();
        if (value < 5) {
            c += (0xFF00000000000000ull & f); 
        }
        else if (value < 9) {
            c += (0xFF00000000000000ull & m); 
        }
        else {
            c += (1ull<<(rand(xmax_now())%8 + 56));
        }
        value = get_one_gene_value_Offset(1ull);
        if (value < 5) {
            c += (0x00FF000000000000ull & f); 
        }
        else if (value < 9) {
            c += (0x00FF000000000000ull & m); 
        }
        else {
            c += (1ull<<(rand(xmax_now())%8 + 48));
        }
        value = get_one_gene_value_Offset(2ull);
        if (value < 5) {
            c += (0x0000FF0000000000ull & f); 
        }
        else if (value < 9) {
            c += (0x0000FF0000000000ull & m); 
        }
        else {
            c += (1ull<<(rand(xmax_now())%8 + 40));
        }
        value = get_one_gene_value_Offset(3ull);
        if (value < 5) {
            c += (0x000000FF00000000ull & f); 
        }
        else if (value < 9) {
            c += (0x000000FF00000000ull & m); 
        }
        else {
            c += (1ull<<(rand(xmax_now())%8 + 32));
        }
        value = get_one_gene_value_Offset(4ull);
        if (value < 5) {
            c += (0x00000000FF000000ull & f); 
        }
        else if (value < 9) {
            c += (0x00000000FF000000ull & m); 
        }
        else {
            c += (1ull<<(rand(xmax_now())%8 + 24));
        }
        value = get_one_gene_value_Offset(5ull);
        if (value < 5) {
            c += (0x0000000000FF0000ull & f); 
        }
        else if (value < 9) {
            c += (0x0000000000FF0000ull & m); 
        }
        else {
            c += (1ull<<(rand(xmax_now())%8 + 16));
        }
        value = get_one_gene_value_Offset(6ull);
        if (value < 5) {
            c += (0x000000000000FF00ull & f); 
        }
        else if (value < 9) {
            c += (0x000000000000FF00ull & m); 
        }
        else {
            c += (1ull<<(rand(xmax_now())%8 + 8));
        }
        value = get_one_gene_value_Offset(7ull);
        if (value < 5) {
            c += (0x00000000000000FFull & f); 
        }
        else if (value < 9) {
            c += (0x00000000000000FFull & m); 
        }
        else {
            c += (1ull<<(rand(xmax_now())%8));
        }
        return c;
    }

    void apply_init() {
        cat newcat;
        if (cats::back(newcat, XNAME(cat))) {
            return;
        }
        newcat.id = 0ull;
        newcat.name = XNAME(cat);
        newcat.price = 1000ull;
        newcat.sell = true;
        newcat.growth = 10ull;
        newcat.bear = 10ull;
        newcat.smart = 10ull;
        newcat.charm = 10ull;
        newcat.generation = 0ull;
        newcat.feedvalue = 0ull;
        newcat.bearcount = 0ull;
        newcat.canbear = true;
        newcat.cooltime = 0ull;
        newcat.gene = (0ull<<56) + (0ull<<48) + (0ull<<40) + (0ull<<32) + (0ull<<24) + (0ull<<16) + (0ull<<8) + 0ull;
        add_cat(XNAME(cat), newcat);
        catown own;
        own.id = newcat.id;
        own.sell = newcat.sell;
        add_own(XNAME(cat), own);
        catsell sell;
        sell.id = newcat.id;
        sell.price = newcat.price;
        add_sell(XNAME(cat), sell);
        newcat.id = max_id()+1;
        newcat.generation = 0ull;
        newcat.gene = (1ull<<56) + (1ull<<48) + (1ull<<40) + (1ull<<32) + (1ull<<24) + (1ull<<16) + (1ull<<8) + 1ull;
        add_cat(XNAME(cat), newcat);
        own.id = newcat.id;
        own.sell = newcat.sell;
        add_own(XNAME(cat), own);
        sell.id = newcat.id;
        sell.price = newcat.price;
        add_sell(XNAME(cat), sell);
        newcat.id = max_id()+1;
        newcat.generation = 1ull;
        newcat.gene =(2ull<<56) + (2ull<<48) + (2ull<<40) + (2ull<<32) + (2ull<<24) + (2ull<<16) + (2ull<<8) + 2ull;
        add_cat(XNAME(cat), newcat);
        own.id = newcat.id;
        own.sell = newcat.sell;
        add_own(XNAME(cat), own);
        sell.id = newcat.id;
        sell.price = newcat.price;
        add_sell(XNAME(cat), sell);
        newcat.id = max_id()+1;
        newcat.generation = 1ull;
        newcat.gene = (3ull<<56) + (3ull<<48) + (3ull<<40) + (3ull<<32) + (3ull<<24) + (3ull<<16) + (3ull<<8) + 3ull;
        add_cat(XNAME(cat), newcat);
        own.id = newcat.id;
        own.sell = newcat.sell;
        add_own(XNAME(cat), own);
        sell.id = newcat.id;
        sell.price = newcat.price;
        add_sell(XNAME(cat), sell);
    }

    void apply_feed(const buymsg& msg) {
       // assert(msg.id >= 0ull, "Must transfer a id greater than 0");

        cat one = get_cat(XNAME(cat), msg.id);
       // assert(one.id == msg.id, "Must transfer from have the id");
      //  assert(one.name == msg.name, "Must transfer from have the id");
        catown own = get_own(msg.name, msg.id);
       // assert(own.id == msg.id, "Must transfer from have the id");
        account me = get_account(msg.name);

        one.growth += 1ull;
        one.bear += 1ull;
        one.smart += 1ull;
        me.feedcount += 1ull;
        update_cat(XNAME(cat), one);
        update_account(msg.name, me);
        xmax_transfer(msg.name, XNAME(cat), 2ull);
    }

    void apply_sell(const sellmsg& msg) {
     //   assert(msg.id >= 0ull, "Must transfer a id greater than 0");

        cat one = get_cat(XNAME(cat), msg.id);
     //   assert(one.id == msg.id, "Must transfer from have the id");
     //   assert(one.name == msg.name, "Must transfer from have the id");
     //   assert(one.sell == false, "Must transfer from have the sell");
        catown own = get_own(msg.name, msg.id);
     //   assert(own.id == msg.id, "Must transfer from have the id");

        one.sell = true;
        own.sell = true;
        update_cat(XNAME(cat), one);
        update_own(msg.name, own);
        catsell sell;
        sell.id = msg.id;
        sell.price = one.price;
        add_sell(XNAME(cat), sell);
    }

    void apply_buy(const buymsg& msg) {
    //    assert(msg.id >= 0ull, "Must transfer a id greater than 0");

        cat one = get_cat(XNAME(cat), msg.id);
    //    assert(one.id == msg.id, "Must transfer from have the id");
    //    assert(one.name != msg.name, "Must transfer from have the id");
    //    assert(one.sell == true, "Must transfer from have the sell");
        catown own = get_own(one.name, msg.id);
    //    assert(own.id == msg.id, "Must transfer from have the id");
        catsell sell = get_sell(XNAME(cat), msg.id);
     //   assert(sell.id == msg.id, "Must transfer from have the id");

        int64_t seller = one.name;
        remove_own(one.name, own);
        remove_sell(XNAME(cat), sell);
        one.name = msg.name;
        one.sell = false;
        update_cat(XNAME(cat), one);
        own.sell = one.sell;
        add_own(msg.name, own);
        xmax_transfer(msg.name, seller, one.price);
    }

    void apply_bear(const bearmsg& msg) {
    //    assert(msg.id >= 0ull, "Must transfer a id greater than 0");
    //    assert(msg.to >= 0ull, "Must transfer a id greater than 0");
    //    assert(msg.id != msg.to, "Must transfer a id greater than 0");

        cat one = get_cat(XNAME(cat), msg.id);
    //    assert(one.id == msg.id, "Must transfer from have the id");
    //    assert(one.name == msg.name, "Must transfer from have the id");
    //   assert(one.can_bear(), "Must transfer from have the id");
        cat two = get_cat(XNAME(cat), msg.to);
    //    assert(two.id == msg.to, "Must transfer from have the id");
    //    assert(two.name == msg.name, "Must transfer from have the id");
    //    assert(two.can_bear(), "Must transfer from have the id");

        one.cooltime = xmax_now() + 60*60;
        one.bearcount += 1ull;
        two.cooltime = one.cooltime;
        two.bearcount += 1ull;
        update_cat(XNAME(cat), one);
        update_cat(XNAME(cat), two);
        cat newcat;
        newcat.id = max_id()+1;
        newcat.name = msg.name;
        newcat.price = 1000ull;
        newcat.sell = false;
        newcat.growth = 10ull;
        newcat.bear = 10ull;
        newcat.smart = 10ull;
        newcat.charm = 10ull;
        newcat.generation = one.generation+1;//max
        newcat.feedvalue = 0ull;
        newcat.bearcount = 0ull;
        newcat.canbear = true;
        newcat.cooltime = 0ull;
        newcat.gene = get_gene(one.gene, two.gene);
        add_cat(XNAME(cat), newcat);
        catown own;
        own.id = newcat.id;
        own.sell = newcat.sell;
        add_own(msg.name, own);
        xmax_transfer(msg.name, XNAME(cat), 100ull);
    }

    void apply_cooltime(const coolmsg& msg) {
		/*
		assert(msg.id >= 0ull, "Must transfer a id greater than 0");
		assert(msg.amount >= 0ull, "Must transfer a amount greater than 0");
		*/
       

        cat one = get_cat(XNAME(cat), msg.id);
		/*
		assert(one.id == msg.id, "Must transfer from have the id");
        assert(one.name == msg.name, "Must transfer from have the id");
        assert(one.can_bear() == false, "Must transfer from have the id");
		*/


        one.cooltime -= msg.amount*60;
        update_cat(XNAME(cat), one);
        xmax_transfer(msg.name, XNAME(cat), msg.amount);
    }

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
      // Xmaxplatform::print( "Init World!\n", Xmaxplatform::name(XNAME(cat)) );
       MYNAME::apply_init();
    }

    /// The apply method implements the dispatch of events to this contract
    void apply( uint64_t code, uint64_t action ) {
     //  Xmaxplatform::print( "Hello World: ", Xmaxplatform::name(code), "->", Xmaxplatform::name(action), "\n" );
       if (code == XNAME(cat)) {
           if (action == XNAME(sell)) {
               MYNAME::apply_sell(Xmaxplatform::current_message<MYNAME::sellmsg>());
           }
           else if (action == XNAME(buy)) {
               MYNAME::apply_buy(Xmaxplatform::current_message<MYNAME::buymsg>());
           }
           else if (action == XNAME(bear)) {
               MYNAME::apply_bear(Xmaxplatform::current_message<MYNAME::bearmsg>());
           }
           else if (action == XNAME(cooltime)) {
               MYNAME::apply_cooltime(Xmaxplatform::current_message<MYNAME::coolmsg>());
           }
       }
    }

} // extern "C"
