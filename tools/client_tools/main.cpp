#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <iostream>
#include "basetypes.hpp"
#include "string_utilities.hpp"


using namespace std;




namespace bpo = boost::program_options;
using namespace Xmaxplatform::Basetypes;

namespace {
	void InitProgramOptions(bpo::options_description& options) {
		options.add_options()
			("help,h", "Print this help message and exit.")
			("name2str", bpo::value<name::name_code_type>(), "Convert name type to string");
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
		return 0;
	}

	if (vars_map.count("name2str"))
	{
		auto name_code = vars_map.at("name2str").as<name::name_code_type>();		
		cout << xmax::name_to_string(name_code) << endl;
		return 0;
	}


	cout << options << std::endl;

	return 0;
}