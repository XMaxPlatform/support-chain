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

	void command::check_params()
	{
		if (!params)
		{
			params = std::make_unique<paramters>();
		}
	}

	void command::add_option(string names, string& val, string desc, bool required)
	{
		check_params();
		optionptr ow = new_widget<optionwidget>(params->options, names, std::move(desc));
		ow->setup(val, required);

		if (required)
		{
			params->requireds.push_back(ow);
		}
	}

	void command::add_flag(string names, bool& val, string desc)
	{
		check_params();

		new_widget<flagwidget>(params->flags, names, std::move(desc))->setup(val);
	}

	commandptr command::add_subcommand(string names, string desc)
	{
		if (!subs)
		{
			subs = std::make_unique<dic<commandinstptr>>();
		}
		return new_widget<command>(*subs, names, std::move(desc));
	}

	void command::set_callback(callback c)
	{
		cbk = c;
	}

	bool command::try_flags(cmdstack& stack)
	{
		if (!params->flags.empty() && stack.noend())
		{
			const string& name = stack.current();
			auto itr = params->flags.find(name);
			if (itr != params->flags.end())
			{
				stack.next();
				itr->second->mark();
				return true;
			}
		}
		return false;
	}

	void command::log_help()
	{
		logstart << desc << logend;
	}

	bool command::try_options(cmdstack& stack)
	{
		if (!params->options.empty()&& stack.noend())
		{
			const string& name = stack.current();
			auto itr = params->options.find(name);
			if (itr != params->options.end() && stack.hasnext())
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
		if (params)
		{
			for (auto itr : params->options)
			{
				itr.second->reset();
			}
			for (auto itr : params->flags)
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
				log_help();			
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

		while (params && stack.noend())
		{
			bool found = try_flags(stack);
			if (!found)
			{
				found = try_options(stack);
			}

			stack.next();
		}

		if (params)
		{
			for (const auto& itr : params->requireds)
			{
				if (itr->required && ! itr->bset)
				{
					log_help();
					return;
				}
			}
		}

		if (cbk)
		{
			cbk();
		}
	}

}
