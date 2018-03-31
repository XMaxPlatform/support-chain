/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */

#include <tempdir.hpp>

#include <cstdlib>

namespace Xmaxplatform { namespace Utilities {

fc::path temp_directory_path()
{
   const char* xmax_tempdir = getenv("XMAX_TEMPDIR");
   if( xmax_tempdir != nullptr )
      return fc::path( xmax_tempdir );
   return fc::temp_directory_path() / "xmax-tmp";
}

} } // Xmaxplatform::Utilities
