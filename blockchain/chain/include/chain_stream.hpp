/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <fc/filesystem.hpp>
#include <block.hpp>

namespace block_detail
{
	//
	// ----------- block stream -------------------------
	// [block 1][desc 1][eof][block 2][desc 2][eof] ... [block n][desc n][eof]
	// ------------ index stream ------------------------
	// [index 1][index 2] ... [index n]
	//


	using namespace Xmaxplatform::Chain;
	typedef uint32_t block_end_eof_type;
	static const block_end_eof_type block_end_eof = 0xb10cedef;

	struct alignas(64) block_desc
	{
		uint64_t num;
		uint64_t pos;
		uint64_t size;
		block_desc()
			: num(0)
			, pos(0)
			, size(0)
		{

		}
	};

	struct alignas(64) block_index
	{
		uint64_t num;
		uint64_t pos;
		uint64_t size;
		xmax_type_block_id id;

		block_index()
			: pos(0)
			, num(0)
			, size(0)
		{

		}
	};
}

FC_REFLECT(block_detail::block_desc, (num)(pos)(size))

FC_REFLECT(block_detail::block_index, (num)(pos)(size)(id))

namespace Xmaxplatform { namespace Chain {

	class chain_stream_impl;

	class chain_stream
	{
	public:
		chain_stream(const fc::path& data_dir);

		~chain_stream();

		uint64_t append_block(const signed_block_ptr& block);

		signed_block_ptr get_head() const;

		signed_block_ptr read_by_num(uint32_t num) const;

		int64_t last_block_num() const;

		std::vector<block_detail::block_index> read_indices(int64_t begin_num, int64_t block_count) const;

	private:
		std::unique_ptr<chain_stream_impl> stream_impl;

	};


}
}