/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once

#include <bases.hpp>

namespace Basecli {
	class command;
	using cmdvec = std::vector<string>;

	class cmdstack
	{
	public:
		cmdstack(const cmdvec& c)
			: cmds(c)
			, pc(0)
			, count((int)c.size())
		{

		}

		const string& current() const
		{
			return cmds[pc];
		}

		bool noend() const
		{
			return pc < count;
		}

		bool hasnext() const
		{
			return pc + 1 < count;
		}

		bool end() const
		{
			return pc >= count;
		}

		void next()
		{
			++pc;
		}
	private:
		const cmdvec& cmds;
		int pc = -1;
		int count = 0;
	};

}