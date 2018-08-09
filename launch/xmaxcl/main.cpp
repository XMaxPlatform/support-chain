/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */

#include <basecli.hpp>


using namespace Basecli;

int main(int argc, char** argv)
{
	appcli app;

	auto cmd_test = app.add_command("test", "this is a test cmd.");

	bool hello;
	cmd_test->add_flag("--hello, -o", hello, "hello world.");

	app.run();

	return 0;
}
