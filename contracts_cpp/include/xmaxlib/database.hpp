/**
 *  @file
 *  @copyright defined in xmax/LICENSE.txt
 */
#pragma once

#include <xmaxlib/database.h>


namespace Xmaxplatform {


template<typename T>
struct table_implement_object {};


template<int Primary, int Secondary>
struct table_implement{};


template<>
struct table_implement<sizeof(uint128_t),sizeof(uint128_t)> {

    static int32_t front_primary( uint64_t scope, uint64_t code, uint64_t table_n, void* data, uint32_t len ) {
       return front_primary_i128i128( scope, code, table_n, data, len );
    }

    static int32_t back_primary( uint64_t scope, uint64_t code, uint64_t table_n, void* data, uint32_t len ) {
       return back_primary_i128i128( scope, code, table_n, data, len );
    }

    static int32_t load_primary( uint64_t scope, uint64_t code, uint64_t table_n, void* data, uint32_t len ) {
       return load_primary_i128i128( scope, code, table_n, data, len );
    }

    static int32_t next_primary( uint64_t scope, uint64_t code, uint64_t table_n, void* data, uint32_t len ) {
       return next_primary_i128i128( scope, code, table_n, data, len );
    }

    static int32_t previous_primary( uint64_t scope, uint64_t code, uint64_t table_n, void* data, uint32_t len ) {
       return previous_primary_i128i128( scope, code, table_n, data, len );
    }

    static int32_t upper_bound_primary( uint64_t scope, uint64_t code, uint64_t table_n, void* data, uint32_t len ) {
       return upper_bound_primary_i128i128( scope, code, table_n, data, len );
    }

    static int32_t lower_bound_primary( uint64_t scope, uint64_t code, uint64_t table_n, void* data, uint32_t len ) {
       return lower_bound_primary_i128i128( scope, code, table_n, data, len );
    }

    static int32_t front_secondary( uint64_t scope, uint64_t code, uint64_t table_n, void* data, uint32_t len ) {
       return front_secondary_i128i128( scope, code, table_n, data, len );
    }

    static int32_t back_secondary( uint64_t scope, uint64_t code, uint64_t table_n, void* data, uint32_t len ) {
       return back_secondary_i128i128( scope, code, table_n, data, len );
    }

    static int32_t load_secondary( uint64_t scope, uint64_t code, uint64_t table_n, void* data, uint32_t len ) {
       return load_secondary_i128i128( scope, code, table_n, data, len );
    }

    static int32_t next_secondary( uint64_t scope, uint64_t code, uint64_t table_n, void* data, uint32_t len ) {
       return next_secondary_i128i128( scope, code, table_n, data, len );
    }

    static int32_t previous_secondary( uint64_t scope, uint64_t code, uint64_t table_n, void* data, uint32_t len ) {
       return previous_secondary_i128i128( scope, code, table_n, data, len );
    }

    static int32_t upper_bound_secondary( uint64_t scope, uint64_t code, uint64_t table_n, void* data, uint32_t len ) {
       return upper_bound_secondary_i128i128( scope, code, table_n, data, len );
    }

    static int32_t lower_bound_secondary( uint64_t scope, uint64_t code, uint64_t table_n, void* data, uint32_t len ) {
       return lower_bound_secondary_i128i128( scope, code, table_n, data, len );
    }

    static int32_t remove( uint64_t scope, uint64_t table_n, const void* data ) {
       return remove_i128i128( scope, table_n, data );
    }

    static int32_t store( account_name scope, table_name table_n, const void* data, uint32_t len ) {
       return store_i128i128( scope, table_n, data, len );
    }

    static int32_t update( account_name scope, table_name table_n, const void* data, uint32_t len ) {
       return update_i128i128( scope, table_n, data, len );
    }
};

/**
 *  @defgroup dualindextable Dual Index Table
 *  @brief Defines a type-safe C++ wrapper around the C Dual Index Table
 *
 *  @tparam scope         - the default account name/scope that this table is located within
 *  @tparam code          - the code account name which has write permission to this table
 *  @tparam table         - a unique identifier (name) for this table
 *  @tparam Record        - the type of data stored in each row
 *  @tparam PrimaryType   - the type of the first field stored in @ref Record
 *  @tparam SecondaryType - the type of the second field stored in @ref Record
 *
 *  The primary and secondary indices are sorted as N-bit unsigned integers from lowest to highest.
 *
 *  @code
 *  struct model {
 *      uint64_t primary;
 *      uint64_t secondary;
 *      uint64_t value;
 *  };
 *
 *  typedef table<N(myscope), N(mycode), N(mytable), model, uint64_t, uint64_t> MyTable;
 *  model a { 1, 11, N(first) };
 *  model b { 2, 22, N(second) };
 *  model c { 3, 33, N(third) };
 *  model d { 4, 44, N(fourth) };
 *
 *  bool res = MyTable::store(a);
 *  ASSERT(res, "store");
 *
 *  res = MyTable::store(b);
 *  ASSERT(res, "store");
 *
 *  res = MyTable::store(c);
 *  ASSERT(res, "store");
 *
 *  res = MyTable::store(d);
 *  ASSERT(res, "store");
 *
 *  model query;
 *  res = MyTable::primary_index::get(1, query);
 *  ASSERT(res && query.primary == 1 && query.value == N(first), "first");
 *
 *  res = MyTable::primary_index::front(query);
 *  ASSERT(res && query.primary == 4 && query.value == N(fourth), "front");
 *
 *  res = MyTable::primary_index::back(query);
 *  ASSERT(res && query.primary == 1 && query.value == N(first), "back");
 *
 *  res = MyTable::primary_index::previous(query);
 *  ASSERT(res && query.primary == 2 && query.value == N(second), "previous");
 *
 *  res = MyTable::primary_index::next(query);
 *  ASSERT(res && query.primary == 1 && query.value == N(first), "first");
 *
 *  res = MyTable::secondary_index::get(11, query);
 *  ASSERT(res && query.primary == 11 && query.value == N(first), "first");
 *
 *  res = MyTable::secondary_index::front(query);
 *  ASSERT(res && query.secondary == 44 && query.value == N(fourth), "front");
 *
 *  res = MyTable::secondary_index::back(query);
 *  ASSERT(res && query.secondary == 11 && query.value == N(first), "back");
 *
 *  res = MyTable::secondary_index::previous(query);
 *  ASSERT(res && query.secondary == 22 && query.value == N(second), "previous");
 *
 *  res = MyTable::secondary_index::next(query);
 *  ASSERT(res && query.secondary == 11 && query.value == N(first), "first");
 *
 *  res = MyTable::remove(query);
 *  ASSERT(res, "remove");
 *
 *  res = MyTable::get(query);
 *  ASSERT(!res, "not found already removed");
 *
 *  @endcode
 *  @ingroup databaseCpp
  * @{
 */
template<uint64_t scope, uint64_t code, uint64_t table_n, typename Record, typename PrimaryType, typename SecondaryType = void>
struct table {
   private:
   typedef table_implement<sizeof( PrimaryType ), sizeof( SecondaryType )> implement;
   static_assert( sizeof(PrimaryType) + sizeof(SecondaryType) <= sizeof(Record), "invalid template parameters" );

   public:
   typedef PrimaryType primary;
   typedef SecondaryType secondary;

    /**
     * @brief Primary Index of the Table
     */
   struct primary_index {
      /**
      *  @param r - reference to a record to store the front record based on primary index.
      *  @param s - account scope. default is current scope of the class
      *
      *  @return true if successful read.
      */
      static bool front( Record& r, uint64_t s = scope ) {
         return implement::front_primary( s, code, table_n, &r, sizeof(Record) ) == sizeof(Record);
      }

      /**
      *  @param r - reference to a record to store the back record based on primary index.
      *  @param s - account scope. default is current scope of the class
      *
      *  @return true if successful read.
      */
      static bool back( Record& r, uint64_t s = scope ) {
         return implement::back_primary( s, code, table_n, &r, sizeof(Record) ) == sizeof(Record);
      }

      /**
      *  @param r - reference to a record to store next value; must be initialized with current.
      *  @param s - account scope. default is current scope of the class
      *
      *  @return true if successful read.
      */
      static bool next( Record& r, uint64_t s = scope ) {
         return implement::next_primary( s, code, table_n, &r, sizeof(Record) ) == sizeof(Record);
      }

      /**
      *  @param r - reference to a record to store previous value; must be initialized with current.
      *  @param s - account scope. default is current scope of the class
      *
      *  @return true if successful read.
      */
      static bool previous( Record& r, uint64_t s = scope ) {
         return implement::previous_primary( s, code, table_n, &r, sizeof(Record) ) == sizeof(Record);
      }

      /**
      *  @param p - reference to primary key to load; must be initialized with a value;
      *  @param r - reference to a record to load the value to.
      *  @param s - account scope. default is current scope of the class
      *
      *  @return true if successful read.
      */
      static bool get( const PrimaryType& p, Record& r, uint64_t s = scope ) {
         *reinterpret_cast<PrimaryType*>(&r) = p;
         return implement::load_primary( s, code, table_n, &r, sizeof(Record) ) == sizeof(Record);
      }

       /**
       *  @param p - reference to primary key to get the lower bound of; must be initialized with a value;
       *  @param r - reference to a record to load the value to.
       *  @param s - account scope. default is current scope of the class
       *
       *  @return true if successful read.
       */
      static bool lower_bound( const PrimaryType& p, Record& r ) {
         return implement::lower_bound_primary( scope, code, table_n, &p &r, sizeof(Record) ) == sizeof(Record);
      }

       /**
       *  @param p - reference to primary key to get the upper bound of; must be initialized with a value;
       *  @param r - reference to a record to load the value to.
       *  @param s - account scope. default is current scope of the class
       *
       *  @return true if successful read.
       */
      static bool upper_bound( const PrimaryType& p, Record& r ) {
         return implement::upper_bound_primary( scope, code, table_n, &p &r, sizeof(Record) ) == sizeof(Record);
      }

      /**
      *  @param r - reference to a record to remove from table;
      *  @param s - account scope. default is current scope of the class
      *
      *  @return true if successfully removed;
      */
      static bool remove( const Record& r, uint64_t s = scope ) {
         return implement::remove( s, table_n, &r ) != 0;
      }
   };

   /**
     * @brief Secondary Index of the Table
     */

   struct secondary_index {
       /**
       *  @param r - reference to a record to store the front record based on secondary index.
       *  @param s - account scope. default is current scope of the class
       *
       *  @return true if successful read.
       */
       static bool front( Record& r, uint64_t s = scope ) {
          return implement::front_secondary( s, code, table_n, &r, sizeof(Record) ) == sizeof(Record);
       }

       /**
       *  @param r - reference to a record to store the back record based on secondary index.
       *  @param s - account scope. default is current scope of the class
       *
       *  @return true if successful read.
       */
       static bool back( Record& r, uint64_t s = scope ) {
          return implement::back_secondary( s, code, table_n, &r, sizeof(Record) ) == sizeof(Record);
       }

       /**
       *  @param r - reference to a record to return the next record .
       *  @param s - account scope. default is current scope of the class
       *
       *  @return true if successful read.
       */
       static bool next( Record& r, uint64_t s = scope ) {
          return implement::next_secondary( s, code, table_n, &r, sizeof(Record) ) == sizeof(Record);
       }

       /**
       *  @param r - reference to a record to return the next record.
       *  @param s - account scope. default is current scope of the class
       *
       *  @return true if successful read.
       */
       static bool previous( Record& r, uint64_t s = scope ) {
          return implement::previous_secondary( s, code, table_n, &r, sizeof(Record) ) == sizeof(Record);
       }

       /**
       *  @param p - reference to secondary index key
       *  @param r - reference to record to hold the value
       *  @param s - account scope. default is current scope of the class
       *
       *  @return true if successful read.
       */
       static bool get( const SecondaryType& p, Record& r, uint64_t s = scope ) {
          return implement::load_secondary( s, code, table_n, &p &r, sizeof(Record) ) == sizeof(Record);
       }

       /**
       *  @param p - reference to secondary key to get the lower bound of; must be initialized with a value;
       *  @param r - reference to a record to load the value to.
       *  @param s - account scope. default is current scope of the class
       *
       *  @return true if successful read.
       */

       static bool lower_bound( const SecondaryType& p, Record& r, uint64_t s = scope ) {
          return implement::lower_bound_secondary( s, code, table_n, &p &r, sizeof(Record) ) == sizeof(Record);
       }

       /**
        *  @param p - reference to secondary key to get the upper bound of; must be initialized with a value;
        *  @param r - reference to a record to load the value to.
        *  @param s - account scope. default is current scope of the class
        *
        *  @return true if successful read.
        */
       static bool upper_bound( const SecondaryType& p, Record& r, uint64_t s = scope ) {
          return implement::upper_bound_secondary( s, code, table_n, &p &r, sizeof(Record) ) == sizeof(Record);
       }

       /**
        *  @param r - reference to a record to be removed.
        *  @param s - account scope. default is current scope of the class
        *
        *  @return true if successfully removed.
        */
       static bool remove( const Record& r, uint64_t s = scope ) {
          return implement::remove( s, table_n, &r ) != 0;
       }
    };

    /**
    *  @brief Fetches a record from the table.
    *  @details Fetches a record from the table.
    *  @param p - reference to primary key to retrieve
    *  @param r - reference to a record to load the value to.
    *  @param s - account scope. default is current scope of the class
    *
    *  @return true if successful read.
    */

    static bool get( const PrimaryType& p, Record& r, uint64_t s = scope ) {
       *reinterpret_cast<PrimaryType*>(&r) = p;
       return implement::load_primary( s, code, table_n, &r, sizeof(Record) ) == sizeof(Record);
    }

     /**
     *  @brief Store a record in the table.
     *  @details Store a record in the table.
     *  @param r - reference to a record to store.
     *  @param s - account scope. default is current scope of the class
     *
     *  @return true if successful store.
     */
    static bool store( const Record& r, uint64_t s = scope ) {
       assert( implement::store( s, table_n, &r, sizeof(r) ), "error storing record" );
       return true;
    }

    /**
    *  @brief Update a record in the table.
    *  @details Update a record in the table.
    *  @param r - reference to a record to update.
    *  @param s - account scope. default is current scope of the class
    *
    *  @return true if successful update.
    */
    static bool update( const Record& r, uint64_t s = scope ) {
       assert( implement::update( s, table_n, &r, sizeof(r) ), "error updating record" );
       return true;
    }

    /**
    *  @brief Remove a record from the table.
    *  @details Remove a record from the table.
    *  @param r - reference to a record to remove.
    *  @param s - account scope. default is current scope of the class
    *
    *  @return true if successful remove.
    */
    static bool remove( const Record& r, uint64_t s = scope ) {
       return implement::remove( s, table_n, &r ) != 0;
    }
 };
/// @}

template<>
struct table_implement<sizeof(uint64_t),0> {

    static int32_t front_primary( uint64_t scope, uint64_t code, uint64_t table_n, void* data, uint32_t len ) {
       return front_i64( scope, code, table_n, data, len );
    }

    static int32_t back_primary( uint64_t scope, uint64_t code, uint64_t table_n, void* data, uint32_t len ) {
       return back_i64( scope, code, table_n, data, len );
    }

    static int32_t load_primary( uint64_t scope, uint64_t code, uint64_t table_n, void* data, uint32_t len ) {
       return load_i64( scope, code, table_n, data, len );
    }

    static int32_t next_primary( uint64_t scope, uint64_t code, uint64_t table_n, void* data, uint32_t len ) {
       return next_i64( scope, code, table_n, data, len );
    }

    static int32_t previous_primary( uint64_t scope, uint64_t code, uint64_t table_n, void* data, uint32_t len ) {
       return previous_i64( scope, code, table_n, data, len );
    }

    static int32_t lower_bound_primary( uint64_t scope, uint64_t code, uint64_t table_n, void* data, uint32_t len ) {
       return lower_bound_i64( scope, code, table_n, data, len );
    }

    static int32_t upper_bound_primary( uint64_t scope, uint64_t code, uint64_t table_n, void* data, uint32_t len ) {
        return upper_bound_i64(scope, code, table_n, data, len);
    }

    static int32_t remove( uint64_t scope, uint64_t table_n, const void* data ) {
       return remove_i64( scope, table_n, (uint64_t*)data);
    }

    static int32_t store( account_name scope, table_name table_n, const void* data, uint32_t len ) {
       return store_i64( scope, table_n, data, len );
    }

    static int32_t update( account_name scope, table_name table_n, const void* data, uint32_t len ) {
       return update_i64( scope, table_n, data, len );
    }
};

 /**
  *  @defgroup singleindextable Single Index Table
  *  @brief Defines a type-safe C++ wrapper around the C Single Index Table
  *
  *  @tparam scope - the default account name scope that this table is located within
  *  @tparam code  - the code account name which has write permission to this table
  *  @tparam table - a unique identifier (name) for this table
  *  @tparam Record - the type of data stored in each row
  *  @tparam PrimaryType - the type of the first field stored in @ref Record
  *
  *  Example
  *  @code
  *
  *  struct my_model {
  *     uint128_t number;
  *     uint64_t  name;
  *  };
  *
  *  typedef table<N(myscope), N(mycode), N(mytable), my_model, uint128_t> MyTable;
  *
  *  my_model a { 1, N(one) };
  *  my_model b { 2, N(two) };
  *  my_model c { 3, N(three) };
  *  my_model d { 4, N(four) };
  *
  *  bool res = MyTable::store(a);
  *  ASSERT(res, "store");

  *  res = MyTable::store(b);
  *  ASSERT(res, "store");
  *
  *  res = MyTable::store(c);
  *  ASSERT(res, "store");
  *
  *  res = MyTable::store(d);
  *  ASSERT(res, "store");
  *
  *  my_model query;
  *  res = MyTable::front(query);
  *  ASSERT(res && query.number == 4 && query.name == N(four), "front");
  *
  *  res = MyTable::back(query);
  *  ASSERT(res && query.number == 1 && query.name == N(one), "back");
  *
  *  res = MyTable::primary_index::previous(query);
  *  ASSERT(res && query.number == 2 && query.name == N(two), "previous");
  *
  *  res = MyTable::primary_index::next(query);
  *  ASSERT(res && query.number == 1 && query.name == N(one), "next");
  *
  *  query.number = 4;
  *  res = MyTable::get(query);
  *  ASSERT(res && query.number == 4 && query.name = N(four), "get");
  *
  *  query.name = N(Four);
  *  res = MyTable.update(query);
  *  ASSERT(res && query.number == 4 && query.name == N(Four), "update");
  *
  *  res = MyTable.remove(query);
  *  ASSERT(res, "remove");
  *
  *  res = MyTable.get(query);
  *  ASSERT(!res, "get of removed record");
  *
  *  @endcode
  *  @ingroup databaseCpp
  *  @{
  */
template<uint64_t scope, uint64_t code, uint64_t table_n, typename Record, typename PrimaryType>
struct table<scope,code,table_n,Record,PrimaryType,void> {
   private:
   typedef table_implement<sizeof( PrimaryType ),0> implment;
   static_assert( sizeof(PrimaryType) <= sizeof(Record), "invalid template parameters" );

   public:
   typedef PrimaryType primary;
    /**
     * @brief Primary Index of the Table
     */
   struct primary_index {
      /**
       * @param r - reference to a record to store the front.
       * @param s - scope; defaults to scope of the class.
       *
       *  @return true if successfully retrieved the front of the table.
       */
      static bool front( Record& r, uint64_t s = scope ) {
         return implement::front_primary( s, code, table_n, &r, sizeof(Record) ) == sizeof(Record);
      }

      /**
       *  @param r - reference to a record to store the back.
       *  @param s - scope; defaults to scope of the class.
       *
       *  @return true if successfully retrieved the back of the table.
       */
      static bool back( Record& r, uint64_t s = scope ) {
         return implement::back_primary( s, code, table_n, &r, sizeof(Record) ) == sizeof(Record);
      }

      /**
       *  @param r - reference to store the next record. Must be initialized with a key.
       *  @param s - scope; defaults to scope of the class.
       *
       *  @return true if successfully retrieved the next record.
       */
      static bool next( Record& r, uint64_t s = scope ) {
         return implement::next_primary( s, code, table_n, &r, sizeof(Record) ) == sizeof(Record);
      }

      /**
       *  @param r - reference to store previous record. Must be initialized with a key.
       *  @param s - scope; defaults to scope of the class.
       *
       *  @return true if successfully retrieved the previous record.
       */
      static bool previous( Record& r, uint64_t s = scope ) {
         return implement::previous_primary( s, code, table_n, &r, sizeof(Record) ) == sizeof(Record);
      }

       /**
       *  @param p - reference to the primary key to retrieve the record.
       *  @param r - reference to hold the result of the query.
        * @param s - scope; defaults to scope of the class.
       *  @return true if successfully retrieved the record.
       */
      static bool get( const PrimaryType& p, Record& r, uint64_t s = scope ) {
         *reinterpret_cast<PrimaryType*>(&r) = p;
         return implement::load_primary( s, code, table_n, &r, sizeof(Record) ) == sizeof(Record);
      }

       /**
       *  @param p - reference to the primary key to retrieve the lower bound.
       *  @param r - reference to hold the result of the query.
       *  @return true if successfully retrieved the record.
       */
      static bool lower_bound( const PrimaryType& p, Record& r ) {
         *reinterpret_cast<PrimaryType*>(&r) = p;
         return implement::lower_bound_primary( scope, code, table_n, &r, sizeof(Record) ) == sizeof(Record);
      }

       /**
       *  @param p - reference to the primary key to retrieve the upper bound.
       *  @param r - reference to hold the result of the query.
       *  @return true if successfully retrieved the record.
       */
       static bool upper_bound( const PrimaryType& p, Record& r ) {
         *reinterpret_cast<PrimaryType*>(&r) = p;
         return implement::upper_bound_primary( scope, code, table_n, &r, sizeof(Record) ) == sizeof(Record);
      }

       /**
       *  @param r - reference to record to be removed.
       *  @return true if successfully removed.
       */
       static bool remove( const Record& r, uint64_t s = scope ) {
         return implement::remove( s, table_n, &r ) != 0;
      }
   };


    /**
     * @brief Fetches the front of the table
     * @details Fetches the front of the table
     * @param r - reference to hold the value
     * @param s - scope; defaults to scope of the class.
     * @return true if successfully retrieved the front
     */
    static bool front( Record& r, uint64_t s = scope ) { return primary_index::front(r, s); }

    /**
     * @brief Fetches the back of the table
     * @details Fetches the back of the table
     * @param r - reference to hold the value
     * @param s - scope; defaults to scope of the class.
     * @return true if successfully retrieved the back
     */
    static bool back( Record& r, uint64_t s = scope )  { return primary_index::back(r, s);  }

    /**
     * @brief Retrieves the record for the specified primary key
     * @details Retrieves the record for the specified primary key
     * @param p - the primary key of the record to fetch
     * @param r - reference of record to hold return value
     * @param s - scope; defaults to scope of the class.
     * @return true if get succeeds.
     */
   static bool get( const PrimaryType& p, Record& r, uint64_t s = scope ) {
      *reinterpret_cast<PrimaryType*>(&r) = p;
      return implement::load_primary( s, code, table_n, &r, sizeof(Record) ) == sizeof(Record);
   }

   /**
    * @brief Retrieves a record based on initialized primary key value
    * @details Retrieves a record based on initialized primary key value
    * @param r - reference of a record to hold return value; must be initialized to the primary key to be fetched.
    * @param s - scope; defaults to scope of the class.
    * @return true if get succeeds.
    */
   static bool get( Record& r, uint64_t s = scope ) {
      return implement::load_primary( s, code, table_n, &r, sizeof(Record) ) == sizeof(Record);
   }

   static bool store( const Record& r, uint64_t s = scope ) {
      return implement::store( s, table_n, &r, sizeof(r) ) != 0;
   }


   static bool update( const Record& r, uint64_t s = scope ) {
      return implement::update( s, table_n, &r, sizeof(r) ) != 0;
   }


   static bool remove( const Record& r, uint64_t s = scope ) {
      return implement::remove( s, table_n, &r ) != 0;
   }
}; 

template<>
struct table_implement_object<char*> {

    static int32_t store( account_name scope, table_name table_n, char* key, uint32_t keylen, char* data, uint32_t datalen ) {
        return store_str( scope, table_n, key, keylen, data, datalen );
    }

    static int32_t update( account_name scope, table_name table_n, char* key, uint32_t keylen, char* data, uint32_t datalen ) {
        return update_str( scope, table_n, key, keylen, data, datalen );
    }

    static int32_t front( account_name scope, account_name code, table_name table_n, char* data, uint32_t len ) {
        return front_str( scope, code, table_n, data, len );
    }

    static int32_t back( account_name scope, account_name code, table_name table_n, char* data, uint32_t len ) {
        return back_str( scope, code, table_n, data, len );
    }

    static int32_t load( account_name scope, account_name code, table_name table_n, char* key, uint32_t keylen, char* data, uint32_t datalen ) {
        return load_str( scope, code, table_n, key, keylen, data, datalen );
    }

    static int32_t next( account_name scope, account_name code, table_name table_n, char* key, uint32_t keylen, char* data, uint32_t datalen ) {
        return next_str( scope, code, table_n, key, keylen, data, datalen );
    }

    static int32_t previous( account_name scope, account_name code, table_name table_n, char* key, uint32_t keylen, char* data, uint32_t datalen ) {
        return previous_str( scope, code, table_n, key, keylen, data, datalen );
    }

    static int32_t lower_bound( account_name scope, account_name code, table_name table_n, char* key, uint32_t keylen, char* data, uint32_t datalen ) {
        return lower_bound_str( scope, code, table_n, key, keylen, data, datalen );
    }

    static int32_t upper_bound( account_name scope, account_name code, table_name table_n, char* key, uint32_t keylen, char* data, uint32_t datalen ) {
        return upper_bound_str( scope, code, table_n, key, keylen, data, datalen );
    }

    static int32_t remove( account_name scope, table_name table_n, char* key, uint32_t keylen ) {
        return remove_str( scope, table_n, key, keylen );
    }
};

/**
  *  @defgroup singlevarindextable Single Variable Length Index Table
  *  @brief Defines a type-safe C++ wrapper around the C Single Variable Length Index Table (e.g. string index)
  *
  *  @tparam scope - the default account name scope that this table is located within
  *  @tparam code  - the code account name which has write permission to this table
  *  @tparam table_n - a unique identifier (name) for this table
  *  @tparam PrimaryType - the type of the first field stored in @ref Record
  *
  *  @ingroup databaseCpp
  *  @{
  */

template<account_name scope, account_name code, table_name table_n, typename PrimaryType>
struct var_table {
    private:
    typedef table_implement_object<PrimaryType> implement;

    public:
    typedef PrimaryType primary;

    /**
     * @brief Store a record to the table
     * @details Store a record to the table
     * @param key - key of the data to be stored
     * @param keylen - length of the key
     * @param record - data to be stored
     * @param len - length of data to be stored
     * @return 1 if a new record was created, 0 if an existing record was updated
     */
    int32_t store( primary key, uint32_t keylen, char* record, uint32_t len ) {
        return implement::store( scope, table_n, key, keylen, record, len );
    }

    /**
     * @brief Update a record in the table
     * @details Update a record to the table
     * @param key - key of the data to be updated
     * @param keylen - length of the key
     * @param record - data to be updated
     * @param len - length of data to be updated
     * @return 1 if the record was updated, 0 if no record with key was found
     */
    int32_t update( primary key, uint32_t keylen, char* record, uint32_t len ) {
        return implement::update( scope, table_n, key, keylen, record, len );
    }

    /**
     * @brief Fetches the front of the table
     * @details Fetches the front of the table
     * @param key - key of the data to be updated
     * @param keylen - length of the key
     * @param record - data to be updated
     * @param len - length of data to be updated
     * @return the number of bytes read or -1 if key was not found
     */
    int32_t front( char* record, uint32_t len ) {
        return implement::front( scope, code, table_n, record, len );
    }

     /**
     * @brief Fetches the back of the table
     * @details Fetches the back of the table
     * @param key - key of the data to be updated
     * @param keylen - length of the key
     * @param record - data to be updated
     * @param len - length of data to be updated
     * @return the number of bytes read or -1 if key was not found
     */
    int32_t back( char* record, uint32_t len ) {
        return implement::back( scope, code, table_n, record, len );
    }

    /**
     * @brief Fetches a record from the table
     * @details Fetches a record from the table
     * @param key - key of the data to be updated
     * @param keylen - length of the key
     * @param record - data to be updated
     * @param len - length of data to be updated
     * @return the number of bytes read or -1 if key was not found
     */
    int32_t load( primary key, uint32_t keylen, char* record, uint32_t len ) {
       return implement::load( scope, code, table_n, key, keylen, record, len );
    }

    /**
     * @brief Fetches a record which key is next of the given key
     * @details Fetches a record which key is next of the given key
     * @param key - key of the data to be updated
     * @param keylen - length of the key
     * @param record - data to be updated
     * @param len - length of data to be updated
     * @return the number of bytes read or -1 if key was not found
     */
    int32_t next( primary key, uint32_t keylen, char* record, uint32_t len ) {
       return implement::next( scope, code, table_n, key, keylen, record, len );
    }

    /**
     * @brief Fetches a record which key is previous of the given key
     * @details Fetches a record which key is previous of the given key
     * @param key - key of the data to be updated
     * @param keylen - length of the key
     * @param record - data to be updated
     * @param len - length of data to be updated
     * @return the number of bytes read or -1 if key was not found
     */
    int32_t previous( primary key, uint32_t keylen, char* record, uint32_t len ) {
       return implement::previous( scope, code, table_n, key, keylen, record, len );
    }

    /**
     * @brief Fetches a record which key is the nearest larger than or equal to the given key
     * @details Fetches a record which key is the nearest larger than or equal to the given key
     * @param key - key of the data to be updated
     * @param keylen - length of the key
     * @param record - data to be updated
     * @param len - length of data to be updated
     * @return the number of bytes read or -1 if key was not found
     */
    int32_t lower_bound( primary key, uint32_t keylen, char* record, uint32_t len ) {
       return implement::lower_bound( scope, code, table_n, key, keylen, record, len );
    }

    /**
     * @brief Fetches a record which key is the nearest larger than the given key
     * @details Fetches a record which key is the nearest larger than the given key
     * @param key - key of the data to be updated
     * @param keylen - length of the key
     * @param record - data to be updated
     * @param len - length of data to be updated
     * @return the number of bytes read or -1 if key was not found
     */
    int32_t upper_bound( primary key, uint32_t keylen, char* record, uint32_t len ) {
       return implement::upper_bound( scope, code, table_n, key, keylen, record, len );
    }

    /**
     * @brief Remove a record from the table.
     * @details Remove a record from the table.
     * @param key - key of the data to be updated
     * @param keylen - length of the key
     * @param record - data to be updated
     * @param len - length of data to be updated
     * @return 1 if a record was removed, and 0 if no record with key was found
     */
    int32_t remove( primary key, uint32_t keylen ) {
       return implement::remove( scope, table_n, key, keylen );
    }
};

/// @} singlevarindextable


} // namespace Xmaxplatform

#define TABLE2(NAME, SCOPE, CODE, TABLE, TYPE, PRIMARY_NAME, PRIMARY_TYPE, SECONDARY_NAME, SECONDARY_TYPE) \
   using NAME = Xmaxplatform::table<N(SCOPE),N(CODE),N(TABLE),TYPE,PRIMARY_TYPE,SECONDARY_TYPE>; \
   typedef NAME::primary_index PRIMARY_NAME; \
   typedef NAME::secondary_index SECONDARY_NAME;
