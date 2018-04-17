/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once

#include <stdint.h>
//#include <wchar.h>

#ifdef __cplusplus
extern "C" {
#endif

	/**
	*  @defgroup types of contract.
	*  @ingroup contractdev
	*  @brief Specifies typedefs and aliases
	*
	*  @{
	*/

	typedef uint64_t xmax_name;
	typedef uint32_t xmax_time;

	typedef xmax_name account_name;
	typedef xmax_name permission_name;
	typedef xmax_name token_name;
	typedef xmax_name table_name;

	typedef xmax_name scope_name;
	typedef xmax_name action_name;

	typedef xmax_time time;
	typedef uint16_t region_id;

	typedef uint64_t asset_symbol;
	typedef int64_t share_type;
	typedef uint16_t weight_type;

#define PACKED(X) __attribute((packed)) X

	struct public_key {
		char data[34];
	};

	struct signature {
		uint8_t data[66];
	};

	struct checksum256 {
		uint8_t hash[32];
	};

	struct checksum160 {
		uint8_t hash[20];
	};

	struct checksum512 {
		uint8_t hash[64];
	};

	struct fixed_string16 {
		uint8_t len;
		char str[16];
	};

	typedef struct checksum256 transaction_id_type;

	typedef struct fixed_string16 field_name;

	struct fixed_string32 {
		uint8_t len;
		char str[32];
	};

	typedef struct fixed_string32 type_name;

	struct account_permission {
		account_name account;
		permission_name permission;
	};

#ifdef __cplusplus
} /// extern "C"
#endif
  /// @}
