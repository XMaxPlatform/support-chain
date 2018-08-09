/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once

#include <command.hpp>

namespace Basecli {


	class appcli
	{
	public:

		command* add_command(string names, string desc);

		bool init(int argc, char** argv);

	protected:
		command rootcmd;
	};
}
