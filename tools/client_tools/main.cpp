#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <iostream>

using namespace std;




namespace bpo = boost::program_options;

namespace {
	void InitProgramOptions(bpo::options_description& options) {
		options.add_options()
			("help,h", "Print this help message and exit.");
	}

	void ParseProgramOptions(int argc, char** argv, bpo::options_description& options, bpo::variables_map& vars_map) {
		bpo::store(bpo::parse_command_line(argc, argv, options), vars_map);
		bpo::notify(vars_map);
	}
}


int main(int argc, char** argv)
{
	bpo::options_description options;
	bpo::variables_map vars_map;

	InitProgramOptions(options);
	ParseProgramOptions(argc, argv, options, vars_map);

	if (vars_map.count("help")) {
		cout << options << std::endl;
		return false;
	}



	cout << options << std::endl;

	return 0;
}