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
	using namespace Xmaxplatform::Chain;
	typedef uint32_t block_end_eof_type;
	static const block_end_eof_type block_end_eof = 0xb10cedef;
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

	enum IO_Code
	{
		IO_Read,
		IO_Write,
	};

	static void stream_seek(std::fstream& stream, std::fstream::off_type pos, std::ios_base::seekdir dir, IO_Code code)
	{
		switch (code)
		{
		case block_detail::IO_Read:
		{
			stream.seekp(pos, dir);
		}
			break;
		case block_detail::IO_Write:
		{
			stream.seekg(pos, dir);
		}
			break;
		default:
			break;
		}
	}

	static void stream_seek(std::fstream& stream, std::fstream::off_type pos, IO_Code code)
	{
		stream_seek(stream, pos, std::ios::beg, code);
	}

	static void stream_end(std::fstream& stream, IO_Code code)
	{
		stream_seek(stream, 0, std::ios::end, code);
	}

	static void to_last_block_desc(std::fstream& stream, IO_Code code)
	{
		stream_seek(stream, sizeof(block_end_eof_type) + sizeof(block_desc), std::ios::end, code);
	}
	static void to_block_index(std::fstream& stream, IO_Code code)
	{
		stream_seek(stream, sizeof(block_index), std::ios::end, code);
	}
	static void to_end_eof(std::fstream& stream, IO_Code code)
	{
		stream_seek(stream, sizeof(block_end_eof_type), std::ios::end, code);
	}

	static void read_eof(std::fstream& stream, block_end_eof_type& eof)
	{
		stream.read((char*)&eof, sizeof(block_end_eof_type));
	}
	static void write_eof(std::fstream& stream)
	{
		stream.write((char*)&block_end_eof, sizeof(block_end_eof_type));
	}

	static void read_block_desc(std::fstream& stream, block_desc& val)
	{
		stream.read((char*)&val, sizeof(block_desc));
	}

	static void write_block_desc(std::fstream& stream, const block_desc& val)
	{
		stream.write((char*)&val, sizeof(block_desc));
	}

	static void read_block_index(std::fstream& stream, block_index& val)
	{
		stream.read((char*)&val, sizeof(block_index));
	}

	static void write_block_index(std::fstream& stream, const block_index& val)
	{
		stream.write((char*)&val, sizeof(block_index));
	}

	static uint64_t write_block(std::fstream& block_stream, std::fstream& index_stream, const signed_block_ptr& blockptr)
	{
		auto data = fc::raw::pack(*blockptr);

		block_stream.seekg(0, std::ios::end);
		uint64_t pos = block_stream.tellg();

		block_desc desc;
		block_index index;
		desc.num = blockptr->block_num();
		desc.pos = pos;
		desc.size = data.size();

		index.pos = pos;

		uint64_t idx_pos = index_stream.tellp();

		FC_ASSERT(idx_pos == sizeof(block_index) * (desc.num - 1),
			"Append to index file occuring at wrong position.",
			("position", idx_pos)
			("expected", (desc.num - 1) * sizeof(uint64_t)));


		block_stream.seekg(desc.pos);

		block_stream.write(data.data(), data.size());
		write_block_desc(block_stream, desc);

		block_stream.flush();

		write_eof(block_stream);

		block_stream.flush();

		write_block_index(index_stream, index);
		index_stream.flush();

		return pos;
	}

	static signed_block_ptr read_block(std::fstream& stream, block_desc& desc)
	{
		stream_seek(stream, desc.pos, IO_Read);
		signed_block_ptr block_ptr = std::make_shared<signed_block>();

		fc::raw::unpack(stream, block_ptr);
		return block_ptr;
	}
}

static void open_file_stream(const fc::path& file, std::fstream& stream)
{
	stream.open(file.generic_string().c_str(), LOG_WRITE);
}

static void clear_file_stream(const fc::path& file, std::fstream& stream)
{
	stream.close();
	fc::remove_all(file);
	open_file_stream(file, stream);
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
			open_file_stream(block_file, block_stream);
			open_file_stream(index_file, index_stream);

			uint64_t log_size = fc::file_size(block_file);
			uint64_t index_size = fc::file_size(index_file);

			if (0 == log_size && 0 < index_size)
			{
				ilog("Index is nonempty, remove and recreate it");
				clear_file_stream(index_file, index_stream);
			}
			else if (0 == index_size && 0 < log_size)
			{
				ilog("Block is nonempty, remove and recreate it");
				clear_file_stream(block_file, block_stream);
			}
			else if (0 < index_size && 0 < log_size)
			{
				//fc::raw::pack(
				// check match between index and log.
				// empty now.

				block_detail::to_end_eof(block_stream, block_detail::IO_Read);
				block_detail::block_end_eof_type eof;
				block_detail::read_eof(block_stream, eof);

				FC_ASSERT(eof == block_detail::block_end_eof, "Block log is corrupted");

				block_detail::to_last_block_desc(block_stream, block_detail::IO_Read);
				block_detail::to_block_index(index_stream, block_detail::IO_Read);

				block_detail::block_desc desc;
				block_detail::block_index index;

				block_detail::read_block_desc(block_stream, desc);
				block_detail::read_block_index(index_stream, index);

				FC_ASSERT(desc.pos == index.pos, "Block log is corrupted");
			}
		}

		uint64_t append_block(const signed_block_ptr& blockptr)
		{

			uint64_t pos = block_detail::write_block(block_stream, index_stream, blockptr);

			head_block = blockptr;

			return pos;
		}

		signed_block_ptr get_head() const
		{
			std::fstream& stream = const_cast<std::fstream&>(block_stream);

			block_detail::to_last_block_desc(stream, block_detail::IO_Read);

			block_detail::block_desc desc;

			block_detail::read_block_desc(stream, desc);

			return block_detail::read_block(stream, desc);
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
	signed_block_ptr chain_stream::get_head() const
	{
		return stream_impl->get_head();
	}
}
}