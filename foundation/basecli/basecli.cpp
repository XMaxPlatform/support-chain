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

	void appcli::parse_commands(const std::vector<string>& cmds)
	{
		cmdstack stack(cmds);

		while (brunning && stack.noend())
		{
			syscmds.apply(stack);
			if (brunning && stack.noend())
			{
				rootcmds.apply(stack);
			}
		}
	}

	void appcli::run()
	{
		brunning = true;
		string input;
		std::vector<string> ins;

		while (brunning)
		{
			std::getline(std::cin, input);
			ins = utils::split(input);
			parse_commands(ins);
		}

	}
}