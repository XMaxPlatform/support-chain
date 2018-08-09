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

		void run_commands(int argc, char** argv);

		void run_commands(const std::vector<string>& cmds);

	protected:
		void run_commands_impl(const std::vector<string>& cmds);

		command rootcmds;

		command syscmds;

		bool brunning = false;
	};
}
