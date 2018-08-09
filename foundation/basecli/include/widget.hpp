/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once

#include <bases.hpp>
#include <utils.hpp>
#include <vector>
namespace Basecli {

	class widget
	{
	public:
		friend class command;

		virtual ~widget(){}
	protected:
		std::vector<string> ns;
		string allname;
		string desc;

		widget() {}
	};

	class valwidget : public widget
	{
	public:
		virtual void reset() = 0;
	};


	class optionwidget : public valwidget
	{
	public:
		using valtype = string;
		valtype* valptr = nullptr;

		void setup(valtype& t)
		{
			valptr = &t;
		}

		void parse(const string& strval)
		{
			CLI_ASSERT(valptr);
			*valptr = strval;
		}

		virtual void reset() override
		{
			valptr->clear();
		}
	};

	class flagwidget : public valwidget
	{
	public:
		using valtype = bool;
		valtype* valptr = nullptr;

		void setup(valtype& t)
		{
			valptr = &t;
		}

		void mark()
		{
			CLI_ASSERT(valptr);
			*valptr = true;
		}

		virtual void reset() override
		{
			*valptr = false;
		}


	};

	using optionptr = optionwidget*;
	using flagptr = flagwidget*;
	using widgetvalptr = valwidget*;

}
