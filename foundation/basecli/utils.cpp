/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#include <utils.hpp>
#include <iostream>

namespace Basecli {
	namespace utils
	{
		static const string default_delims = " \t\r\n";

		void trim(string& str, const string& delims)
		{
			str.erase(str.find_last_not_of(delims) + 1); // trim right
			str.erase(0, str.find_first_not_of(delims)); // trim left
		}

		void trim(string& str)
		{
			trim(str, default_delims);
		}

		std::vector<string> split(const string& str, const string& delims)
		{
			std::vector<string> ns;

			size_t pcp = 0;
			size_t pc = 0;
			while ((pc = str.find_first_of(delims, pcp)) != string::npos)
			{
				string res = str.substr(pcp, pc - pcp);
				trim(res);
				if (res.size())
				{
					ns.emplace_back(std::move(res));
				}
				pcp = pc + 1;
			}

			string res = str.substr(pcp);
			trim(res);
			if (res.size())
			{
				ns.emplace_back(std::move(res));
			}
			return ns;
		}
		std::vector<string> split(const string& str)
		{
			return split(str, default_delims);
		}

		std::vector<string> parse_names(const string& names)
		{
			static const string dl = ",";

			return split(names, dl);
		}

		void printlog(const char* log)
		{
			std::cout << log << std::endl;
		}


		bool check_help(const string& name)
		{
			if (helpflag == name)
			{
				return true;
			}
			if (helpflag2 == name)
			{
				return true;
			}
			return false;
		}
	}
}