/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <block.hpp>
#include <block_pack.hpp>

namespace Xmaxplatform {
namespace Chain {

	class fork_context;

	class forkdatabase
	{
	public:
		forkdatabase(const fc::path& data_dir);
		~forkdatabase();

		void close();

		void add_block(block_pack_ptr block_pack);

		void add_confirmation(const block_confirmation& conf, uint32_t skip);

		block_pack_ptr get_block(xmax_type_block_id block_id);

	private:
		unique_ptr<fork_context> _context;
	};

}
}