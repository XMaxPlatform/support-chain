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
		{
			signed_block emptyblock;

			std::vector<char> data = fc::raw::pack(emptyblock);

			info << "\n empty block size: " << data.size() << " bytes";
		}
		{
			transaction_receipt emptyreceipt;

			std::vector<char> data = fc::raw::pack(emptyreceipt);
			info << "\n empty transaction size: " << data.size() << " bytes";
		}

		{
			Xmaxplatform::Basetypes::message emptymessage;

			std::vector<char> data = fc::raw::pack(emptymessage);
			info << "\n empty message size: " << data.size() << " bytes";
		}

		return info.str();
	}

	std::string list_blocks(const std::string& block_log_dir, int64_t begin_num, int64_t block_count, bool verbose)
	{
		std::stringstream info;

		info << "List " << block_count << " block digests, starting from block num " << begin_num << " ......";


		if (begin_num <= 0 || block_count < 1)
		{
			info << "\nError input.";
			return info.str();
		}

		chain_stream stream(block_log_dir);

		int64_t last_num = stream.last_block_num();

		int64_t cc = last_num - begin_num + 1;

		if (cc < 1)
		{
			info << "\nNo num " << begin_num << " of block."
				<< "\nThe last block num is " << last_num << ".";
			return info.str();
		}

		if (cc < block_count)
		{
			info << "\nNo more blocks, " << "starting from block " << begin_num << "."
				<< "\nThe last block num is " << last_num << ".";

			return info.str();
		}
		info << "\n Last block num: " << last_num;


		std::vector<block_detail::block_index> idxs = stream.read_indices(begin_num, block_count);

		for ( const block_detail::block_index idx : idxs)
		{		
			info << "\n\nBlock [num: " << idx.num << ", size: " << idx.size << "]";
			if (verbose)
			{
				auto block = stream.read_by_num(idx.num);

				int msgs = 0;

				for (const transaction_receipt& trx : block->receipts)
				{
					if (trx.trx.contains<transaction_package>())
					{
						const transaction_package& pck = trx.trx.get<transaction_package>();
						msgs += pck.body.messages.size();
					}

				}
				info << "\n{" << "id: " << idx.id.str() << "}";
				info << "\n{" << "time: " << block->timestamp.time_point().operator fc::string() << "}";
				info << "\n{" << "builder: " << block->builder.to_string() << "}";
				info << "\n{" << "trx: " << block->receipts.size() << "}";
				info << "\n{" << "msg: " << msgs << "}";


			}

		}

		return info.str();
	}
}