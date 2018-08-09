/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once

#include <functional>

#include <bases.hpp>
#include <widget.hpp>

namespace Basecli {
	class command;

	using delegate_t = std::function<void()>;

	using commandptr = command*;

	class command : public widget
	{
	public:

		void add_option(string names, string& val, string desc);
		void add_flag(string names, bool& val, string desc);
		commandptr add_subcommand(string names, string desc);

		//void add_delegate(delegate_t d);

		using ws = std::vector<std::unique_ptr<widget>>;
	private:
		dicptr<optionptr> opts;
		dicptr<flagptr>	fgs;
		dicptr<commandptr> subs;
		dicptr<delegate_t> delegates;

		ws widgets;
	};
}
