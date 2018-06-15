/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/composite_key.hpp>

#include <fc/io/fstream.hpp>

#include <block_pack.hpp>
#include <forkchain.hpp>

namespace Xmaxplatform {
namespace Chain {


	using namespace boost::multi_index;
	using boost::multi_index_container;

	struct by_block_id;
	//struct by_block_num;
	struct by_pre_id;


	typedef multi_index_container <
		block_pack_ptr,
		indexed_by<
		hashed_unique< tag<by_block_id>, member<block_raw, xmax_type_block_id, &block_raw::block_id>, std::hash<xmax_type_block_id> >,
		ordered_non_unique< tag<by_pre_id>, const_mem_fun<block_raw, const xmax_type_block_id&, &block_raw::prev_id> >
		>
		> block_pack_container;





	class fork_context
	{
	public:
		block_pack_container	packs;
		block_pack_ptr			head;
		fc::path				datadir;
	};


	forkdatabase::forkdatabase(const fc::path& data_dir)
		: _context(std::make_unique<fork_context>())
	{
		_context->datadir = data_dir;

		if (!fc::is_directory(_context->datadir))
			fc::create_directories(_context->datadir);

		auto abs_path = boost::filesystem::absolute(_context->datadir / "fork_memory.bin");
		if (fc::exists(abs_path)) {
			string content;
			fc::read_file_contents(abs_path, content);

			fc::datastream<const char*> ds(content.data(), content.size());
			unsigned_int size; fc::raw::unpack(ds, size);
			for (uint32_t i = 0, n = size.value; i < n; ++i) {
				block_pack s;
				fc::raw::unpack(ds, s);
				add_block(std::make_shared<block_pack>(std::move(s)));
			}
			xmax_type_block_id head_id;
			fc::raw::unpack(ds, head_id);

			_context->head = get_block(head_id);

			fc::remove(abs_path);
		}


	}

	void forkdatabase::close()
	{

	}

	forkdatabase::~forkdatabase()
	{
		close();
	}


	void forkdatabase::add_block(block_pack_ptr block_pack)
	{
		auto result = _context->packs.insert(block_pack);
		FC_ASSERT(block_pack->block_id == block_pack->new_header.id());

		FC_ASSERT(result.second, "unable to insert block state, duplicate state detected");

		if (!_context->head)
		{
			_context->head = block_pack;
		}
		else if (_context->head->block_num < block_pack->block_num)
		{
			_context->head = block_pack;
		}
	}

	void forkdatabase::add_confirmation(const block_confirmation& conf, uint32_t skip)
	{
		auto block_pack = get_block(conf.block_id);

		block_pack->add_confirmation(conf, skip);
	}

	block_pack_ptr forkdatabase::get_block(xmax_type_block_id block_id)
	{
		auto itr = _context->packs.find(block_id);
		if (itr != _context->packs.end())
			return *itr;
		return block_pack_ptr();
	}




}
}