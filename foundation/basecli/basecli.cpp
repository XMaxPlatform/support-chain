/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#include <basecli.hpp>

namespace Basecli
{
	bool appcli::init(int argc, char** argv)
	{
		return true;
	}

	command* appcli::add_command(string names, string desc)
	{
		return rootcmd.add_subcommand(names, desc);
	}
}