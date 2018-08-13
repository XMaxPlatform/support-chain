/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */

#include <basecli.hpp>
#include <chain_get.hpp>
#include <iostream>


using namespace Basecli;


void print_block_info()
{
	std::cout << chain_get::block_struct_info() << std::endl;
}


int main(int argc, char** argv)
{
	appcli app;

	auto block_info = app.add_command("block-info", "get block info.");

	block_info->set_callback([&]() {
		print_block_info();
	});

	app.run_commands(argc, argv);
	app.run();

	return 0;
}
