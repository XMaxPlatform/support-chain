/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#include <basecli.hpp>
#include <utils.hpp>
#include <iostream>

namespace Basecli
{

	appcli::appcli()
	{
		// init sys commands.

		auto exitc = syscmds.add_subcommand("exit", "exit the cli.");

		exitc->set_callback([&]() {
			brunning = false;
		});
	}

	commandptr appcli::add_command(string names, string desc)
	{
		return rootcmds.add_subcommand(names, desc);
	}



	void appcli::run_commands_impl(const std::vector<string>& cmds)
	{
		cmdstack stack(cmds);

		if (brunning && stack.noend())
		{
			syscmds.apply(stack);
			if (brunning && stack.noend())
			{
				rootcmds.apply(stack);
			}
		}
	}

	void appcli::run_commands(int argc, char** argv)
	{
		std::vector<string> ins;

		for (int i = 0; i < argc; ++i)
		{
			ins.emplace_back(string(argv[i]));
		}
	}

	void appcli::run_commands(const std::vector<string>& cmds)
	{
		brunning = true;

		run_commands_impl(cmds);

		brunning = false;
	}

	void appcli::run()
	{
		brunning = true;
		string input;
		std::vector<string> ins;

		while (brunning)
		{
			logstart << ">>>";
			std::getline(std::cin, input);
			ins = utils::split(input);
			run_commands_impl(ins);
		}

	}
}