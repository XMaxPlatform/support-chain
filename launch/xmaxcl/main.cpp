/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */

#include <fc/io/json.hpp>
#include <basecli.hpp>
#include <chain_get.hpp>

#include <iostream>

#include <boost/filesystem.hpp>

using namespace Basecli;

namespace math
{
	int64_t to_int64(const std::string& str)
	{
		if (str.empty())
		{
			return 0;
		}
		return fc::to_int64(str);
	}

	int64_t to_int64(const std::string& str, int64_t defval)
	{
		if (str.empty())
		{
			return defval;
		}
		return fc::to_int64(str);
	}
}


int main(int argc, char** argv)
{
	appcli app;

	auto block_app = app.add_command("block", "block commands.");

	{
		auto block_info = block_app->add_subcommand("info", "get block info.");

		block_info->set_callback([&]() {
			std::cout << chain_get::block_struct_info() << std::endl;
		});
	}

	{
		auto list_blocks = block_app->add_subcommand("list", "list blocks info.");

		std::string path;
		std::string beg;
		std::string count;
		bool flag;
		list_blocks->add_option("--path,-p", path, "log path", false);
		list_blocks->add_option("--begin,-b", beg, "begin block number", true);
		list_blocks->add_option("--count,-c", count, "list count of blocks, default is '1'.", false);
		list_blocks->add_flag("--verbose,-v", flag, "list all info of block.");

		list_blocks->set_callback([&]() {
			try {
				const int64_t ibeg = math::to_int64(beg);
				const int64_t icount = math::to_int64(count, 1);

				if (path.empty())
				{
					auto fpath = boost::filesystem::current_path() / "data-dir/blocks";
					path = fpath.string();
				}

				std::cout << chain_get::list_blocks(path, ibeg, icount, flag) << std::endl;
			}
			catch (const std::exception& e)
			{
				std::cout <<"error: "<< e.what() << std::endl;
			}
			catch (const fc::exception& e)
			{
				std::cout << "error: " << e.what() << std::endl;
			}
		
		});
	}



	app.run_commands(argc, argv);
	app.run();

	return 0;
}
