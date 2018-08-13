/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */

#include <fc/io/json.hpp>
#include <basecli.hpp>
#include <chain_get.hpp>
#include <iostream>


using namespace Basecli;



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
		list_blocks->add_option("--path", path, "log path", true);
		list_blocks->add_option("--begin", beg, "begin block number", true);
		list_blocks->add_option("--count", count, "list count of blocks, default is '1'.", false);

		list_blocks->set_callback([&]() {
			try {
				const int64_t ibeg = fc::json::from_string(beg).as<fc::variant>().as_int64();
				const int64_t icount = fc::json::from_string(count).as<fc::variant>().as_int64();

				std::cout << chain_get::list_blocks(path, ibeg, icount) << std::endl;
			}
			catch (const std::exception& e)
			{
				std::cout <<"error: "<< e.what() << std::endl;
			}
		
		});
	}



	app.run_commands(argc, argv);
	app.run();

	return 0;
}
