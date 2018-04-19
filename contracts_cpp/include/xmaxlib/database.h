
#pragma once

#include <xmaxlib/types.h>
extern "C" {

int32_t store_i64( account_name scope, table_name table, const void* data, uint32_t datalen );


int32_t update_i64( account_name scope, table_name table, const void* data, uint32_t datalen );


int32_t load_i64( account_name scope, account_name code, table_name table, void* data, uint32_t datalen );


int32_t front_i64( account_name scope, account_name code, table_name table, void* data, uint32_t datalen );


int32_t back_i64( account_name scope, account_name code, table_name table, void* data, uint32_t datalen );


int32_t next_i64( account_name scope, account_name code, table_name table, void* data, uint32_t datalen );

int32_t previous_i64( account_name scope, account_name code, table_name table, void* data, uint32_t datalen );

int32_t lower_bound_i64( account_name scope, account_name code, table_name table, void* data, uint32_t datalen );

int32_t upper_bound_i64( account_name scope, account_name code, table_name table, void* data, uint32_t datalen );

int32_t remove_i64( account_name scope, table_name table, void* data );

int32_t store_str( account_name scope, table_name table, char* key, uint32_t keylen, char* value, uint32_t valuelen );
 
int32_t update_str( account_name scope, table_name table, char* key, uint32_t keylen, char* value, uint32_t valuelen );
 
int32_t load_str( account_name scope, account_name code, table_name table, char* key, uint32_t keylen, char* value, uint32_t valuelen );

int32_t front_str( account_name scope, account_name code, table_name table, char* value, uint32_t valuelen );

 int32_t back_str( account_name scope, account_name code, table_name table, char* value, uint32_t valuelen );

 int32_t next_str( account_name scope, account_name code, table_name table, char* key, uint32_t keylen, char* value, uint32_t valuelen );

 int32_t previous_str( account_name scope, account_name code, table_name table, char* key, uint32_t keylen, char* value, uint32_t valuelen );

 int32_t lower_bound_str( account_name scope, account_name code, table_name table, char* key, uint32_t keylen, char* value, uint32_t valuelen );

 int32_t upper_bound_str( account_name scope, account_name code, table_name table, char* key, uint32_t keylen, char* value, uint32_t valuelen );
 
 int32_t remove_str( account_name scope, table_name table, char* key, uint32_t keylen );
 
int32_t load_primary_i128i128( account_name scope, account_name code, table_name table, void* data, uint32_t len );

int32_t front_primary_i128i128( account_name scope, account_name code, table_name table, void* data, uint32_t len );

int32_t back_primary_i128i128( account_name scope, account_name code, table_name table, void* data, uint32_t len );

int32_t next_primary_i128i128( account_name scope, account_name code, table_name table, void* data, uint32_t len );

int32_t previous_primary_i128i128( account_name scope, account_name code, table_name table, void* data, uint32_t len );

int32_t upper_bound_primary_i128i128( account_name scope, account_name code, table_name table, void* data, uint32_t len );

int32_t lower_bound_primary_i128i128( account_name scope, account_name code, table_name table, void* data, uint32_t len );


int32_t load_secondary_i128i128( account_name scope, account_name code, table_name table, void* data, uint32_t len );

/**
 *  @param scope - the account scope that will be read, must exist in the transaction scopes list
 *  @param code  - identifies the code that controls write-access to the data
 *  @param table - the ID/name of the table within the scope/code context to query
 *  @param data  - location to copy the front record of secondary key
 *  @param len - the maximum length of data to read, must be greater than sizeof(uint64_t)
 *
 *  @return the number of bytes read or -1 if no record found
 */
int32_t front_secondary_i128i128( account_name scope, account_name code, table_name table, void* data, uint32_t len );

/**
 *  @param scope - the account scope that will be read, must exist in the transaction scopes list
 *  @param code  - identifies the code that controls write-access to the data
 *  @param table - the ID/name of the table within the scope/code context to query
 *  @param data  - location to copy the back record of secondary key
 *  @param len - the maximum length of data to read, must be greater than sizeof(uint64_t)
 *
 *  @return the number of bytes read or -1 if no record found
 */
int32_t back_secondary_i128i128( account_name scope, account_name code, table_name table, void* data, uint32_t len );

/**
 *  @param scope - the account scope that will be read, must exist in the transaction scopes list
 *  @param code  - identifies the code that controls write-access to the data
 *  @param table - the ID/name of the table within the scope/code context to query
 *  @param data  - location to copy the next record of secondary key; must be initialized with a key.
 *  @param len - the maximum length of data to read, must be greater than sizeof(uint64_t)
 *
 *  @return the number of bytes read or -1 if no record found
 */
int32_t next_secondary_i128i128( account_name scope, account_name code, table_name table, void* data, uint32_t len );

/**
 *  @param scope - the account scope that will be read, must exist in the transaction scopes list
 *  @param code  - identifies the code that controls write-access to the data
 *  @param table - the ID/name of the table within the scope/code context to query
 *  @param data  - location to copy the previous record of secondary key; must be initialized with a key.
 *  @param len - the maximum length of data to read, must be greater than sizeof(uint64_t)
 *
 *  @return the number of bytes read or -1 if no record found
 */
int32_t previous_secondary_i128i128( account_name scope, account_name code, table_name table, void* data, uint32_t len );

/**
 *  @param scope - the account scope that will be read, must exist in the transaction scopes list
 *  @param code  - identifies the code that controls write-access to the data
 *  @param table - the ID/name of the table within the scope/code context to query
 *  @param data  - location to copy the upper bound of given secondary key; must be initialized with a key.
 *  @param len - the maximum length of data to read, must be greater than sizeof(uint64_t)
 *
 *  @return the number of bytes read or -1 if no record found
 */
int32_t upper_bound_secondary_i128i128( account_name scope, account_name code, table_name table, void* data, uint32_t len );

/**
 *  @param scope - the account scope that will be read, must exist in the transaction scopes list
 *  @param code  - identifies the code that controls write-access to the data
 *  @param table - the ID/name of the table within the scope/code context to query
 *  @param data  - location to copy the lower bound of given secondary key; must be initialized with a key.
 *  @param len - the maximum length of data to read, must be greater than sizeof(uint64_t)
 *
 *  @return the number of bytes read or -1 if no record found
 */
int32_t lower_bound_secondary_i128i128( account_name scope, account_name code, table_name table, void* data, uint32_t len );


/**
 * @param scope - the account scope that will be read, must exist in the transaction scopes list
 * @param table - the ID/name of the table within the scope/code context to query
 * @param data - must point to at lest 32 bytes containing {primary,secondary}
 *
 * @return 1 if a record was removed, and 0 if no record with key was found
 */
int32_t remove_i128i128( account_name scope, table_name table, const void* data );
/**
 * @param scope - the account scope that will be read, must exist in the transaction scopes list
 * @param table - the ID/name of the table within the scope/code context to query
 * @param data - must point to a at least 32 bytes containing (primary, secondary)
 * @param len - the length of the data
 * @return 1 if a new record was created, 0 if an existing record was updated
 */
int32_t store_i128i128( account_name scope, table_name table, const void* data, uint32_t len );

/**
 * @param scope - the account scope that will be read, must exist in the transaction scopes list
 * @param table - the ID/name of the table within the scope/code context to query
 * @param data - must to a at least 32 bytes containing (primary, secondary)
 * @param len - the length of the data
 * @return 1 if the record was updated, 0 if no record with key was found
 */
int32_t update_i128i128( account_name scope, table_name table, const void* data, uint32_t len );

///@}  dbi128i128

/**
 *  @defgroup dbi64i64i64 Triple 64 bit Index table
 *  @brief Interface to a database table with 64 bit primary, secondary and tertiary keys and arbitrary binary data value.
 *  @ingroup databaseC 
 *
 *  @param scope - the account where table data will be found
 *  @param code  - the code which owns the table
 *  @param table - the name of the table where record is stored
 *  @param data  - a pointer to memory that is at least 32 bytes long 
 *  @param len   - the length of data, must be greater than or equal to 32 bytes
 *
 *  @return the total number of bytes read or -1 for "not found" or "end" where bytes 
 *  read includes 24 bytes of the key  
 *
 *  These methods assume a database table with records of the form:
 *
 *  ```
 *     struct record {
 *        uint64  primary;
 *        uint64  secondary;
 *        uint64  tertiary;
 *        ... arbitrary data ...
 *     };
 *
 *  ```
 *
 *  You can iterate over these indices with primary index sorting records by { primary, secondary, tertiary },
 *  the secondary index sorting records by { secondary, tertiary } and the tertiary index sorting records by
 *  { tertiary }.
 *
 *  @see table class in C++ API
 *
 *  Example
 *  @code
 *  struct test_model3xi64 {
 *         uint64_t a;
 *         uint64_t b;
 *         uint64_t c;
 *         uint64_t name;
 *  };
 *
 *  test_model3xi64 alice{ 0, 0, 0, N(alice) };
 *  test_model3xi64 bob{ 1, 1, 1, N(bob) };
 *  test_model3xi64 carol{ 2, 2, 2, N(carol) };
 *  test_model3xi64 dave{ 3, 3, 3, N(dave) };
 *
 *  int32_t res = store_i64i64i64(CurrentCode(), N(table_name), &alice, sizeof(test_model3xi64));
 *  res = store_i64i64i64(CurrentCode(), N(table_name), &bob, sizeof(test_model3xi64));
 *  res = store_i64i64i64(CurrentCode(), N(table_name), &carol, sizeof(test_model3xi64));
 *  res = store_i64i64i64(CurrentCode(), N(table_name), &dave, sizeof(test_model3xi64));
 *
 *  test_model3xi64 query;
 *  query.a = 0;
 *  res = load_primary_i64i64i64(CurrentCode(), CurrentCode(), N(table_name), &query, sizeof(test_model3xi64));
 *  ASSERT(res == sizeof(test_model3xi64) && query.name == N(alice), "load");
 *
 *  res = front_primary_i64i64i64(CurrentCode(), CurrentCode(), N(table_name), &query, sizeof(test_model3xi64));
 *  ASSERT(res == sizeof(test_model3xi64) && query.name == N(dave), "front");
 *
 *  res = back_primary_i64i64i64(CurrentCode(), CurrentCode(), N(table_name), &query, sizeof(test_model3xi64));
 *  ASSERT(res == sizeof(test_model3xi64) && query.name == N(alice), "back");
 *
 *  res = previous_primary_i64i64i64(CurrentCode(), CurrentCode(), N(table_name), &query, sizeof(test_model3xi64));
 *  ASSERT(res == sizeof(test_model3xi64) && query.name == N(bob), "previous");
 *
 *  res = next_primary_i64i64i64(CurrentCode(), CurrentCode(), N(table_name), &query, sizeof(test_model3xi64));
 *  ASSERT(res == sizeof(test_model3xi64) && query.name == N(alice), "next");*
 *
 *  @endcode
 *  @{
 */

/**
 * @param scope - the account scope that will be read, must exist in the transaction scopes list
 * @param code - the code which owns the table
 * @param table - the ID/name of the table within the current scope/code context to modify
 * @param data - location to copy the record, must be initialized with the (primary,secondary,tertiary) to load
 * @param len - length of record to copy
 * @return the number of bytes read, -1 if key was not found
 *
 * @pre data is a valid pointer to a range of memory at least len bytes long
 * @pre *((uint64_t*)data) stores the primary key
 * @pre scope is declared by the current transaction
 * @pre this method is being called from an apply context (not validate or precondition)
 *
 * @post data will be initialized with the len bytes of record matching the key.
 *
 * @throw if called with an invalid precondition execution will be aborted
 *
 */
int32_t load_primary_i64i64i64( account_name scope, account_name code, table_name table, void* data, uint32_t len );

/**
 *  @param scope - the account scope that will be read, must exist in the transaction scopes list
 *  @param code  - identifies the code that controls write-access to the data
 *  @param table - the ID/name of the table within the scope/code context to query
 *  @param data  - location to copy the front record of primary key
 *  @param len - the maximum length of data to read, must be greater than sizeof(uint64_t)
 *
 *  @return the number of bytes read or -1 if no record found
 */
int32_t front_primary_i64i64i64( account_name scope, account_name code, table_name table, void* data, uint32_t len );

/**
 *  @param scope - the account scope that will be read, must exist in the transaction scopes list
 *  @param code  - identifies the code that controls write-access to the data
 *  @param table - the ID/name of the table within the scope/code context to query
 *  @param data  - location to copy the back record of primary key
 *  @param len - the maximum length of data to read, must be greater than sizeof(uint64_t)
 *
 *  @return the number of bytes read or -1 if no record found
 */
int32_t back_primary_i64i64i64( account_name scope, account_name code, table_name table, void* data, uint32_t len );

/**
 *  @param scope - the account scope that will be read, must exist in the transaction scopes list
 *  @param code  - identifies the code that controls write-access to the data
 *  @param table - the ID/name of the table within the scope/code context to query
 *  @param data  - location to copy the next record of primary key; must be initialized with a key value
 *  @param len - the maximum length of data to read, must be greater than sizeof(uint64_t)
 *
 *  @return the number of bytes read or -1 if no record found
 */
int32_t next_primary_i64i64i64( account_name scope, account_name code, table_name table, void* data, uint32_t len );

/**
 *  @param scope - the account scope that will be read, must exist in the transaction scopes list
 *  @param code  - identifies the code that controls write-access to the data
 *  @param table - the ID/name of the table within the scope/code context to query
 *  @param data  - location to copy the previous record of primary key; must be initialized with a key value
 *  @param len - the maximum length of data to read, must be greater than sizeof(uint64_t)
 *
 *  @return the number of bytes read or -1 if no record found
 */
int32_t previous_primary_i64i64i64( account_name scope, account_name code, table_name table, void* data, uint32_t len );

/**
 *  @param scope - the account scope that will be read, must exist in the transaction scopes list
 *  @param code  - identifies the code that controls write-access to the data
 *  @param table - the ID/name of the table within the scope/code context to query
 *  @param data  - location to copy the upper bound of a primary key; must be initialized with a key value
 *  @param len - the maximum length of data to read, must be greater than sizeof(uint64_t)
 *
 *  @return the number of bytes read or -1 if no record found
 */
int32_t upper_bound_primary_i64i64i64( account_name scope, account_name code, table_name table, void* data, uint32_t len );

/**
 *  @param scope - the account scope that will be read, must exist in the transaction scopes list
 *  @param code  - identifies the code that controls write-access to the data
 *  @param table - the ID/name of the table within the scope/code context to query
 *  @param data  - location to copy the lower bound of primary key; must be initialized with a key value
 *  @param len - the maximum length of data to read, must be greater than sizeof(uint64_t)
 *
 *  @return the number of bytes read or -1 if no record found
 */
int32_t lower_bound_primary_i64i64i64( account_name scope, account_name code, table_name table, void* data, uint32_t len );

/**
 * @param scope - the account scope that will be read, must exist in the transaction scopes list
 * @param code - the code which owns the table
 * @param table - the ID/name of the table within the current scope/code context to modify
 * @param data - location to copy the record, must be initialized with the (secondary,tertiary) to load
 * @param len - length of record to copy
 * @return the number of bytes read, -1 if key was not found
 *
  * @pre data is a valid pointer to a range of memory at least len bytes long
 * @pre *((uint64_t*)data) stores the secondary key
 * @pre scope is declared by the current transaction
 * @pre this method is being called from an apply context (not validate or precondition)
 *
 * @post data will be initialized with the len bytes of record matching the key.
 *
 * @throw if called with an invalid precondition execution will be aborted
 *
 */
int32_t load_secondary_i64i64i64( account_name scope, account_name code, table_name table, void* data, uint32_t len );

/**
 *  @param scope - the account scope that will be read, must exist in the transaction scopes list
 *  @param code  - identifies the code that controls write-access to the data
 *  @param table - the ID/name of the table within the scope/code context to query
 *  @param data  - location to copy the front record of a secondary key
 *  @param len - the maximum length of data to read, must be greater than sizeof(uint64_t)
 *
 *  @return the number of bytes read or -1 if no record found
 */
int32_t front_secondary_i64i64i64( account_name scope, account_name code, table_name table, void* data, uint32_t len );

/**
 *  @param scope - the account scope that will be read, must exist in the transaction scopes list
 *  @param code  - identifies the code that controls write-access to the data
 *  @param table - the ID/name of the table within the scope/code context to query
 *  @param data  - location to copy the back record of secondary key
 *  @param len - the maximum length of data to read, must be greater than sizeof(uint64_t)
 *
 *  @return the number of bytes read or -1 if no record found
 */
int32_t back_secondary_i64i64i64( account_name scope, account_name code, table_name table, void* data, uint32_t len );

/**
 *  @param scope - the account scope that will be read, must exist in the transaction scopes list
 *  @param code  - identifies the code that controls write-access to the data
 *  @param table - the ID/name of the table within the scope/code context to query
 *  @param data  - location to copy the next record; must be initialized with a key value
 *  @param len - the maximum length of data to read, must be greater than sizeof(uint64_t)
 *
 *  @return the number of bytes read or -1 if no record found
 */
int32_t next_secondary_i64i64i64( account_name scope, account_name code, table_name table, void* data, uint32_t len );

/**
 *  @param scope - the account scope that will be read, must exist in the transaction scopes list
 *  @param code  - identifies the code that controls write-access to the data
 *  @param table - the ID/name of the table within the scope/code context to query
 *  @param data  - location to copy the previous record; must be initialized with a key value
 *  @param len - the maximum length of data to read, must be greater than sizeof(uint64_t)
 *
 *  @return the number of bytes read or -1 if no record found
 */
int32_t previous_secondary_i64i64i64( account_name scope, account_name code, table_name table, void* data, uint32_t len );

/**
 *  @param scope - the account scope that will be read, must exist in the transaction scopes list
 *  @param code  - identifies the code that controls write-access to the data
 *  @param table - the ID/name of the table within the scope/code context to query
 *  @param data  - location to copy the upper bound of tertiary key; must be initialized with a key value
 *  @param len - the maximum length of data to read, must be greater than sizeof(uint64_t)
 *
 *  @return the number of bytes read or -1 if no record found
 */
int32_t upper_bound_secondary_i64i64i64( account_name scope, account_name code, table_name table, void* data, uint32_t len );

/**
 *  @param scope - the account scope that will be read, must exist in the transaction scopes list
 *  @param code  - identifies the code that controls write-access to the data
 *  @param table - the ID/name of the table within the scope/code context to query
 *  @param data  - location to copy the lower bound of secondary key; must be initialized with a key value
 *  @param len - the maximum length of data to read, must be greater than sizeof(uint64_t)
 *
 *  @return the number of bytes read or -1 if no record found
 */
int32_t lower_bound_secondary_i64i64i64( account_name scope, account_name code, table_name table, void* data, uint32_t len );

/**
 * @param scope - the account scope that will be read, must exist in the transaction scopes list
 * @param code - the code which owns the table
 * @param table - the ID/name of the table within the current scope/code context to modify
 * @param data - location to copy the record, must be initialized with the (tertiary) to load
 * @param len - length of record to copy
 * @return the number of bytes read, -1 if key was not found
 *
 * @pre data is a valid pointer to a range of memory at least len bytes long
 * @pre *((uint64_t*)data) stores the tertiary key
 * @pre scope is declared by the current transaction
 * @pre this method is being called from an apply context (not validate or precondition)
 *
 * @post data will be initialized with the len bytes of record matching the key.
 *
 * @throw if called with an invalid precondition execution will be aborted
 *
 */
int32_t load_tertiary_i64i64i64( account_name scope, account_name code, table_name table, void* data, uint32_t len );

/**
 *  @param scope - the account scope that will be read, must exist in the transaction scopes list
 *  @param code  - identifies the code that controls write-access to the data
 *  @param table - the ID/name of the table within the scope/code context to query
 *  @param data  - location to copy the front record of a tertiary key
 *  @param len - the maximum length of data to read, must be greater than sizeof(uint64_t)
 *
 *  @return the number of bytes read or -1 if no record found
 */
int32_t front_tertiary_i64i64i64( account_name scope, account_name code, table_name table, void* data, uint32_t len );

/**
 *  @param scope - the account scope that will be read, must exist in the transaction scopes list
 *  @param code  - identifies the code that controls write-access to the data
 *  @param table - the ID/name of the table within the scope/code context to query
 *  @param data  - location to copy the back record of a tertiary key
 *  @param len - the maximum length of data to read, must be greater than sizeof(uint64_t)
 *
 *  @return the number of bytes read or -1 if no record found
 */
int32_t back_tertiary_i64i64i64( account_name scope, account_name code, table_name table, void* data, uint32_t len );

/**
 *  @param scope - the account scope that will be read, must exist in the transaction scopes list
 *  @param code  - identifies the code that controls write-access to the data
 *  @param table - the ID/name of the table within the scope/code context to query
 *  @param data  - location to copy the next record; must be initialized with a key value
 *  @param len - the maximum length of data to read, must be greater than sizeof(uint64_t)
 *
 *  @return the number of bytes read or -1 if no record found
 */
int32_t next_tertiary_i64i64i64( account_name scope, account_name code, table_name table, void* data, uint32_t len );

/**
 *  @param scope - the account scope that will be read, must exist in the transaction scopes list
 *  @param code  - identifies the code that controls write-access to the data
 *  @param table - the ID/name of the table within the scope/code context to query
 *  @param data  - location to copy the previous record; must be initialized with a key value
 *  @param len - the maximum length of data to read, must be greater than sizeof(uint64_t)
 *
 *  @return the number of bytes read or -1 if no record found
 */
int32_t previous_tertiary_i64i64i64( account_name scope, account_name code, table_name table, void* data, uint32_t len );

/**
 *  @param scope - the account scope that will be read, must exist in the transaction scopes list
 *  @param code  - identifies the code that controls write-access to the data
 *  @param table - the ID/name of the table within the scope/code context to query
 *  @param data  - location to copy the upper bound of tertiary key; must be initialized with a key value
 *  @param len - the maximum length of data to read, must be greater than sizeof(uint64_t)
 *
 *  @return the number of bytes read or -1 if no record found
 */
int32_t upper_bound_tertiary_i64i64i64( account_name scope, account_name code, table_name table, void* data, uint32_t len );

/**
 *  @param scope - the account scope that will be read, must exist in the transaction scopes list
 *  @param code  - identifies the code that controls write-access to the data
 *  @param table - the ID/name of the table within the scope/code context to query
 *  @param data  - location to copy the lower bound of tertiary key; must be initialized with a key value
 *  @param len - the maximum length of data to read, must be greater than sizeof(uint64_t)
 *
 *  @return the number of bytes read or -1 if no record found
 */
int32_t lower_bound_tertiary_i64i64i64( account_name scope, account_name code, table_name table, void* data, uint32_t len );

/**
 * @param scope - the account scope that will be read, must exist in the transaction scopes list
 * @param table - the name of table where record is stored
 * @param data - must point to at least 24 bytes containing {primary,secondary,tertiary}
 *
 * @return 1 if a record was removed, and 0 if no record with key was found
 */
int32_t remove_i64i64i64( account_name scope, table_name table, const void* data );
/**
 * @param scope - the account scope that will be read, must exist in the transaction scopes list
 * @param table - the name of table where record is stored
 * @param data - must point to at least 24 bytes containing (primary,secondary,tertiary)
 * @param len - length of the data
 * @return 1 if a new record was created, 0 if an existing record was updated
 */
int32_t store_i64i64i64( account_name scope, table_name table, const void* data, uint32_t len );

/**
 * @param scope - the account scope that will be read, must exist in the transaction scopes list
 * @param table - the name of table where record is stored
 * @param data - must point to at least 24 bytes containing (primary,secondary,tertiary)
 * @param len - length of the data
 * @return 1 if the record was updated, 0 if no record with key was found
 */
int32_t update_i64i64i64( account_name scope, table_name table, const void* data, uint32_t len );

///@}  dbi64i64i64
}
