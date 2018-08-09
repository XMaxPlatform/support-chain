/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once

#include <functional>

#include <bases.hpp>
#include <widget.hpp>
#include <cmdstack.hpp>

namespace Basecli {
	class command;

	using commandinstptr = command*;

	class command : public widget , public icommand
	{
	public:

		virtual void add_option(string names, string& val, string desc) override;
		virtual void add_flag(string names, bool& val, string desc) override;
		virtual commandptr add_subcommand(string names, string desc) override;

		virtual void set_callback(callback c) override;

		using ws = std::vector<std::unique_ptr<widget>>;

		friend class appcli;

	private:

		void apply(cmdstack& stack);

		void try_self(cmdstack& stack);

		bool try_options(cmdstack& stack);
		bool try_flags(cmdstack& stack);
		bool try_subs(cmdstack& stack);

		bool try_sys(cmdstack& stack);

		template<typename T, typename T2>
		T* new_widget(dic<T2*>& dc, const string& names, string&& desc);
		dicptr<optionptr> options;
		dicptr<flagptr>	flags;
		dicptr<commandinstptr> subs;

		callback cbk;

		ws widgets;
	};
}
