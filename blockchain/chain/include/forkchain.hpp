/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <block.hpp>

namespace Xmaxplatform {
namespace Chain {

	class fork_context;

	class forkdatabase
	{
	public:
		forkdatabase(const fc::path& data_dir);
		~forkdatabase();

		void close();

		void set_block(block_pack_ptr block_pack);

		block_pack_ptr get_block(xmax_type_block_id block_id);

	private:
		unique_ptr<fork_context> _context;
	};

}
}