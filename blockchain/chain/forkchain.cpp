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
#include <fc/reflect/reflect.hpp>

#include <block_pack.hpp>
#include <forkchain.hpp>

namespace Xmaxplatform {
namespace Chain {
	struct fork_db_head
	{
	public:
		uint32_t size;
		xmax_type_block_id head_id;

		fork_db_head()
		{
			size = 0;
		}
	};
}
}
FC_REFLECT(Xmaxplatform::Chain::fork_db_head, (size)(head_id))


namespace Xmaxplatform {
namespace Chain {


	using namespace boost::multi_index;
	using boost::multi_index_container;

	struct by_block_id;
	struct by_oldest_block;
	struct by_pre_id;
	struct by_longest_num;

	typedef multi_index_container <
		block_pack_ptr,
		indexed_by<
			hashed_unique< tag<by_block_id>, member<block_raw, xmax_type_block_id, &block_raw::block_id>, std::hash<xmax_type_block_id> >,
			ordered_non_unique< tag<by_pre_id>, const_mem_fun<block_raw, const xmax_type_block_id&, &block_raw::prev_id> >,
			ordered_non_unique < tag<by_oldest_block>,
				composite_key< block_raw,
					member<block_raw, uint32_t, &block_raw::block_num>
				>,
				composite_key_compare< std::less<uint32_t> >
			>,
			ordered_non_unique< tag<by_longest_num>,
				composite_key<block_raw,
					member<block_raw, uint32_t, &block_raw::last_confired_num>,
					member<block_raw, uint32_t, &block_raw::block_num>
				>,
				composite_key_compare< std::greater<uint32_t>, std::greater<uint32_t> >
			>
		>
		> block_pack_index;

	class fork_context
	{
	public:
		block_pack_index		packs;
		block_pack_ptr			head;
		fc::path				datadir;
		irreversible_block_handle irreversible;

		block_pack_ptr get_block(xmax_type_block_id block_id) const
		{
			auto itr = packs.find(block_id);
			if (itr != packs.end())
				return *itr;
			return block_pack_ptr();
		}
		void add_block(block_pack_ptr block_pack)
		{
			auto result = packs.insert(block_pack);
			FC_ASSERT(block_pack->block_id == block_pack->new_header.id());

			FC_ASSERT(result.second, "unable to insert block state, duplicate state detected");

			head = *packs.get<by_longest_num>().begin();

			auto oldest = *packs.get<by_oldest_block>().begin();
			// check update confirmed block.

			if (oldest->block_num < head->last_confired_num)
			{
				on_last_confirmed_grow(head->last_confired_id, head->last_confired_num);
			}
		}
		void set_last_confirmed(xmax_type_block_id last_id)
		{
			auto itr = packs.find(last_id);

			FC_ASSERT(itr != packs.end(), "Unknown block id ${id}", ("id", last_id));

			uint32_t block_num = (*itr)->block_num;

			packs.modify(itr, [&](auto& val)
			{
				val->last_block_num = block_num;
				val->last_confired_num = block_num;
				val->last_confired_id = last_id;
			});

			// flash all pre blocks.
			xmax_type_block_id pre_id = (*itr)->prev_id();
			block_pack_index::iterator pre_it = packs.find(pre_id);
			while (pre_it != packs.end() && pre_id != empty_chain_id)
			{
				pre_id = (*pre_it)->prev_id();

				packs.modify(pre_it, [&](auto& val) {
					val->last_block_num = block_num;
					val->last_confired_num = block_num;
					val->last_confired_id = last_id;
				});

				pre_it = packs.find(pre_id);
			};
		}

		void on_last_confirmed_grow(xmax_type_block_id last_confirmed_id,uint32_t last_confirmed_num)
		{
			std::vector<xmax_type_block_id> remlist;
			std::vector<xmax_type_block_id> confiremdlist;
			auto& idx = packs.get<by_oldest_block>();

			auto it = idx.begin();
			for (auto it = idx.begin(); (it != idx.end() && (*it)->block_num < last_confirmed_num); ++it)
			{
				remlist.push_back((*it)->block_id);
				if ((*it)->last_confired_id == last_confirmed_id)
				{
					confiremdlist.push_back((*it)->block_id);
				}
			}

			for (auto& it : confiremdlist)
			{
				auto ptr = get_block(it);

				irreversible(ptr);
			}

			for (auto& it : remlist)
			{
				auto itr = packs.find(it);
				if (itr != packs.end())
				{
					packs.erase(itr);
				}
			}

		}
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
			fork_db_head head; 
			fc::raw::unpack(ds, head);
			for (uint32_t i = 0, n = head.size; i < n; ++i) {
				block_pack s;
				fc::raw::unpack(ds, s);

				auto result = _context->packs.insert(std::make_shared<block_pack>(std::move(s)));
			}

			_context->head = get_block(head.head_id);

			fc::remove(abs_path);
		}


	}

	void forkdatabase::close()
	{
		if (_context->packs.size() == 0) 
			return;
		auto abs_path = boost::filesystem::absolute(_context->datadir / "fork_memory.bin");
		std::ofstream file(abs_path.generic_string().c_str(), std::ios::out | std::ios::binary | std::ofstream::trunc);

		fork_db_head head;
		head.size = _context->packs.size();
		head.head_id = _context->head->block_id;
		fc::raw::pack(file, head);
		for (auto b : _context->packs)
		{
			fc::raw::pack(file, *b);
		}
	
	}

	forkdatabase::~forkdatabase()
	{
		close();
	}


	void forkdatabase::add_block(block_pack_ptr block_pack)
	{
		_context->add_block(block_pack);
	}

	void forkdatabase::add_confirmation(const block_confirmation& conf, uint32_t skip)
	{
		auto block_pack = get_block(conf.block_id);
		FC_ASSERT(block_pack, "Unknown block id ${id}", ("id", conf.block_id));
		if (block_pack)
		{
			block_pack->add_confirmation(conf, skip);
			if (block_pack->enough_confirmation())
			{
				if (block_pack->last_confired_num < block_pack->block_num)
				{
					_context->set_last_confirmed(block_pack->block_id);
				}
			}
		}
	}

	block_pack_ptr forkdatabase::get_block(xmax_type_block_id block_id) const
	{
		return _context->get_block(block_id);
	}

	block_pack_ptr forkdatabase::get_head() const
	{
		return _context->head;
	}

	irreversible_block_handle& forkdatabase::get_irreversible_handle()
	{
		return _context->irreversible;
	}


}
}