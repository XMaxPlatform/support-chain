/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#include <command.hpp>
#include <utils.hpp>

namespace Basecli {

	template<typename T>
	using scope = std::unique_ptr<T>;

	template<typename T>
	void check_dic(dicptr<T>& opts)
	{
		if (!opts)
		{
			opts = std::make_unique<dic<T>>();
		}
	}

	template<typename TWidget>
	TWidget* find_widget(const dic<TWidget*>& dc, const string& name)
	{
		dic<TWidget*>::const_iterator itr = dc.find(name);
		if (itr != dc.end())
		{
			return (*itr).second;
		}
		return nullptr;
	}

	template<typename T, typename T2>
	T* command::new_widget(dic<T2*>& dc, const string& names, string&& desc)
	{
		T* sp(new T);
		widget* wd = static_cast<widget*>(sp);
		sp->allname = names;
		sp->ns = utils::parse_names(names);
		sp->desc = std::move(desc);

		widgets.emplace_back(sp);

		for (const auto& n : sp->ns)
		{
			auto itr = dc.find(n);
			if (itr == dc.end())
			{
				dc.insert(std::make_pair(n, sp));
			}
			else
			{
				logstart << "conflict name '" << n << "' in command '" << sp->allname << "'" << logend;
			}
		}

		return sp;
	}

	void command::add_option(string names, string& val, string desc)
	{
		check_dic(options);
		new_widget<optionwidget>(*options, names, std::move(desc))->setup(val);
	}

	void command::add_flag(string names, bool& val, string desc)
	{
		check_dic(flags);

		new_widget<flagwidget>(*flags, names, std::move(desc))->setup(val);
	}

	commandptr command::add_subcommand(string names, string desc)
	{
		check_dic(subs);
		return new_widget<command>(*subs, names, std::move(desc));
	}

	void command::set_callback(callback c)
	{
		cbk = c;
	}

	bool command::try_flags(cmdstack& stack)
	{
		if (flags && stack.noend())
		{
			const string& name = stack.current();
			auto itr = flags->find(name);
			if (itr != flags->end())
			{
				stack.next();
				itr->second->mark();
				return true;
			}
		}
		return false;
	}

	bool command::try_options(cmdstack& stack)
	{
		if (options&& stack.noend())
		{
			const string& name = stack.current();
			auto itr = options->find(name);
			if (itr != options->end() && stack.hasnext())
			{
				stack.next();
				itr->second->parse(stack.current());
				return true;
			}
		}
		return false;
	}

	bool command::try_subs(cmdstack& stack)
	{
		if (subs && stack.noend())
		{
			command* cmd = find_widget<command>(*(subs), stack.current());
			if (cmd)
			{
				stack.next();
				cmd->apply(stack);
				return true;
			}
		}
		return false;
	}

	void command::apply(cmdstack& stack)
	{
		if (options)
		{
			for (auto itr : *options)
			{
				itr.second->reset();
			}
		}
		if (flags)
		{
			for (auto itr : *flags)
			{
				itr.second->reset();
			}
		}
		if (!try_subs(stack))
		{
			try_self(stack);
		}	
	}

	bool command::try_sys(cmdstack& stack)
	{
		if (stack.noend())
		{
			if (utils::check_help(stack.current()))
			{
				logstart << desc << logend;
				return true;
			}
		}

		return false;
	}

	void command::try_self(cmdstack& stack)
	{
		if (try_sys(stack))
		{
			return;
		}

		while (stack.noend())
		{
			bool found = try_flags(stack);
			if (!found)
			{
				found = try_options(stack);
			}

			stack.next();
		}

		if (cbk)
		{
			cbk();
		}
	}

}
