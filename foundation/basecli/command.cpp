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

	template<typename T>
	T* new_widget(command::ws& widgets, const string& names, string&& desc)
	{
		T* sp(new T);
		widget* wd = static_cast<widget*>(sp);
		wd->ns = utils::parse_names(names);
		wd->desc = std::move(desc);

		widgets.emplace_back(sp);

		return sp;
	}

	void command::add_option(string names, string& val, string desc)
	{
		check_dic(opts);
		new_widget<option>(widgets, names, std::move(desc))->setup(val);
	}

	void command::add_flag(string names, bool& val, string desc)
	{
		check_dic(fgs);

		new_widget<flag>(widgets, names, std::move(desc))->setup(val);
	}

	commandptr command::add_subcommand(string names, string desc)
	{
		check_dic(subs);
		return new_widget<command>(widgets, names, std::move(desc));
	}

	//void command::add_delegate(delegate_t d)
	//{

	//}
}
