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

	using irreversible_block_handle = std::function<void(block_pack_ptr)>;

	using branch_blocks = std::vector<block_pack_ptr>;

	class forkdatabase
	{
	public:
		forkdatabase(const fc::path& data_dir);
		~forkdatabase();

		void close();

		void add_block(block_pack_ptr block_pack);

		void add_block(signed_block_ptr block);

		void add_confirmation(const block_confirmation& conf, uint32_t skip);

		block_pack_ptr get_block(xmax_type_block_id block_id) const;

		block_pack_ptr get_main_block_by_num(uint32_t num) const;

		block_pack_ptr get_head() const;

		void remove_chain(const xmax_type_block_id& begin_id);

	
		// get two branch, sort by each head block to fork begin block of each branch.
		std::pair<branch_blocks, branch_blocks> 
			fetch_branch_from_fork(const xmax_type_block_id& firstid, const xmax_type_block_id& secondid) const;

		template<typename T, typename F>
		void bind_irreversible(T* obj, F func)
		{
			get_irreversible_handle() = std::bind(func, obj, std::placeholders::_1);
		}

	private:

		irreversible_block_handle& get_irreversible_handle();

		unique_ptr<fork_context> _context;
	};

}
}