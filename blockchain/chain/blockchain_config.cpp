/**
*  @file
*  @copyright defined in xmax/LICENSE
*/

#include <blockchain_config.hpp>
#include <key_conversion.hpp>

#define xmax_build_key_string "XMX5vzzeAtf3wPMwoEJvFpXaZKAuptQHvDFdmkV1DMEN2DUXWjTGi"
#define xmax_build_private_key_string "5JhChtLZjnim5ZzreogqvUQ3pUiZne4Bg3QJhxNSifPC4xHhLys"

namespace Xmaxplatform {
namespace Config {

	const public_key xmax_build_public_key(xmax_build_key_string);
	const private_key xmax_build_private_key = *Utilities::wif_to_key(xmax_build_private_key_string);



}
}