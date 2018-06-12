/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#include <fstream>
#include <fc/io/raw.hpp>
#include <chain_stream.hpp>

#define LOG_READ  (std::ios::in | std::ios::binary)
#define LOG_WRITE (std::ios::out | std::ios::binary | std::ios::app)


namespace block_detail
{
	static const uint32_t block_end_eof = 0xb10cedef;
	struct block_desc
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

	struct block_index
	{
		uint64_t pos;

		block_index()
			: pos(0)
		{

		}
	};
}

FC_REFLECT(block_detail::block_desc, (num)(pos))

FC_REFLECT(block_detail::block_index, (num)(pos))

namespace Xmaxplatform { namespace Chain {

	class chain_stream_impl
	{
	public:
		std::fstream             block_stream;
		std::fstream             index_stream;
		fc::path                 block_file;
		fc::path                 index_file;
		signed_block_ptr         head_block;

		~chain_stream_impl()
		{
			if (block_stream.is_open())
			{
				block_stream.close();
			}
			if (index_stream.is_open())
			{
				index_stream.close();
			}
		}

		void init()
		{
			block_stream.exceptions(std::fstream::failbit | std::fstream::badbit);
			index_stream.exceptions(std::fstream::failbit | std::fstream::badbit);
		}

		void open(const fc::path& data_dir)
		{
			if (block_stream.is_open())
				block_stream.close();
			if (index_stream.is_open())
				index_stream.close();

			if (!fc::is_directory(data_dir))
				fc::create_directories(data_dir);
			block_file = data_dir / "blocks.log";
			index_file = data_dir / "blocks.index";

			//ilog("Opening block log at ${path}", ("path", block_file.generic_string()));
			block_stream.open(block_file.generic_string().c_str(), LOG_WRITE);
			index_stream.open(index_file.generic_string().c_str(), LOG_WRITE);


			uint64_t log_size = fc::file_size(block_file);
			uint64_t index_size = fc::file_size(index_file);


			if (0 == log_size && 0 < index_size)
			{
				ilog("Index is nonempty, remove and recreate it");
				index_stream.close();
				fc::remove_all(index_file);
				index_stream.open(index_file.generic_string().c_str(), LOG_WRITE);
			}
			else if (0 == index_size && 0 < log_size)
			{
				ilog("Block is nonempty, remove and recreate it");
				block_stream.close();
				fc::remove_all(block_file);
				block_stream.open(block_file.generic_string().c_str(), LOG_WRITE);
			}
			else if (0 < index_size && 0 < log_size)
			{
				//fc::raw::pack(
				// check match between index and log.
				// empty now.
			}

		}

		uint64_t append_block(const signed_block_ptr& blockptr)
		{

			auto data = fc::raw::pack(*blockptr);

			uint64_t pos = block_stream.tellp();

			block_detail::block_desc desc;
			block_detail::block_index index;
			desc.num = blockptr->block_num();
			desc.pos = pos;
			desc.size = data.size();

			index.pos = pos;


			uint64_t idx_pos = index_stream.tellp();

			FC_ASSERT(idx_pos == sizeof(block_detail::block_index) * (desc.num - 1),
				"Append to index file occuring at wrong position.",
				("position", idx_pos)
				("expected", (desc.num - 1) * sizeof(uint64_t)));


			block_stream.seekg(desc.pos);

			block_stream.write(data.data(), data.size());
			block_stream.write((char*)&desc, sizeof(desc));
			block_stream.flush();
			block_stream.write((char*)&block_detail::block_end_eof, sizeof(block_detail::block_end_eof));
			block_stream.flush();

			index_stream.write((char*)&index, sizeof(index));
			index_stream.flush();

			head_block = blockptr;

			return pos;
		}
	};


	chain_stream::chain_stream(const fc::path& data_dir)
		:stream_impl(std::make_unique<chain_stream_impl>()) {
		stream_impl->init();
		stream_impl->open(data_dir);
	}

	chain_stream::~chain_stream()
	{
		if (stream_impl) {
			stream_impl.reset();
		}
	}

	uint64_t chain_stream::append_block(const signed_block_ptr& block)
	{
		return stream_impl->append_block(block);
	}

}
}