/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#include <utils.hpp>

namespace Basecli {
	namespace utils
	{

		void trim(string& str)
		{
			static const string delims = " \t\r";
			str.erase(str.find_last_not_of(delims) + 1); // trim right
			str.erase(0, str.find_first_not_of(delims)); // trim left
		}

		std::vector<string> parse_names(const string& names)
		{
			std::vector<string> ns;

			size_t pcp = 0;
			size_t pc = 0;
			while ((pc = names.find_first_of(',', pcp)) != string::npos)
			{
				string res = names.substr(pcp, pc - pcp);
				trim(res);
				if (res.size())
				{
					ns.emplace_back(std::move(res));
				}
				pcp = pc + 1;
			}

			string res = names.substr(pcp);
			trim(res);
			if (res.size())
			{
				ns.emplace_back(std::move(res));
			}
			return ns;
		}
	}
}