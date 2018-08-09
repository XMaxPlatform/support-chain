/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once

#include <bases.hpp>
#include <vector>
#include <iostream>

namespace Basecli {
	namespace utils
	{

		static const string helpflag = "--help";
		static const string helpflag2 = "-h";


		void trim(string& str, const string& delims);

		void trim(string& str);

		std::vector<string> split(const string& str, const string& delims);

		std::vector<string> split(const string& str);

		std::vector<string> parse_names(const string& names);

		void printlog(const char* log);

		bool check_help(const string& name);
	}

#define clilog(lg) utils::printlog(lg);
#define logstart std::cout
#define logend std::endl
}