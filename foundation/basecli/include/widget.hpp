/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once

#include <bases.hpp>

namespace Basecli {

	class widget
	{
	public:
		std::vector<string> ns;

		string desc;

	protected:
		widget();
	};

	template<typename valtype>
	class widgeto : public widget
	{
	public:
		valtype* valptr = nullptr;

		void setup(valtype& t)
		{
			valptr = &t;
		}
	};

	using option = widgeto<string>;

	using flag = widgeto<bool>;

	using optionptr = option*;
	using flagptr = flag*;

}
