/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#include <chain_get.hpp>
#include <block.hpp>
#include <chain_stream.hpp>

namespace chain_get
{

	using namespace Xmaxplatform::Chain;

	std::string block_struct_info()
	{
		std::stringstream info;
		info << "block info:";
		signed_block emptyblock;

		std::vector<char> data = fc::raw::pack(emptyblock);

		info << "\n empty block size: " << data.size() << " bytes";

		return info.str();
	}

	std::string list_block_size(const std::string& block_log_dir, int64_t begin_num, int64_t block_count)
	{
		std::stringstream info;

		info << "List " << block_count << " block digests, starting from block num " << begin_num << " ......";


		if (begin_num < 0 || block_count < 1)
		{
			info << "\nError input.";
			return info.str();
		}


		chain_stream stream(block_log_dir);

		int64_t last_num = stream.last_block_num();

		int64_t cc = last_num - begin_num + 1;

		if (cc < 1)
		{
			info << "\nThere is not num " << begin_num << " of block."
				<< "\nThe last block num is " << last_num << ".";

			return info.str();
		}

		if (cc < block_count)
		{
			info << "\n No more blocks, " << "starting from block " << begin_num << "."
				<< "\nThe last block num is " << last_num << ".";

			return info.str();
		}

		std::vector<block_detail::block_index> idxs = stream.read_indices(begin_num, block_count);

		for ( const block_detail::block_index idx : idxs)
		{
			info << "\nBlock£ºnum "<< idx.num << ", size: "<< idx.size << ", id "<< idx.id.str();
		}

		return info.str();
	}
}