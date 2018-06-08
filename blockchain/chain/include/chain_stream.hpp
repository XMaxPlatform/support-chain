/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include <fc/filesystem.hpp>
#include <block.hpp>
namespace Xmaxplatform { namespace Chain {

	class chain_stream_impl;

	class chain_stream
	{
	public:
		chain_stream(const fc::path& data_dir);

		~chain_stream();


		uint64_t append_block(const signed_block_ptr& block);
		void flush();


	private:
		std::unique_ptr<chain_stream_impl> stream_impl;

	};


}
}