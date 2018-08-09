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
		appcli();

		commandptr add_command(string names, string desc);

		void run();

		void parse_commands(const std::vector<string>& cmds);

	protected:

		command* one_command();

		command rootcmds;

		command syscmds;

		bool brunning = false;
	};
}
