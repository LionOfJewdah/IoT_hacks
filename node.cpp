#include <string>
#include <regex>
#include <vector>
#include <iostream>
#include "string_joiner.tpp"

#define STRING_VIEW std::string
#define CPP17CONST const
#define SV ""
#if __cplusplus > 201402L
#define STRING_VIEW std::string_view
#define CPP17CONST constexpr
#define SV sv
#endif

#define CONSTEXPR_STRING_VIEW CPP17CONST STRING_VIEW

#define TEMP_PACKAGE_FILE "package.tmp.json"

constexpr auto filter_template=".\"node-red\".\"nodes\".", temp_package_file = TEMP_PACKAGE_FILE;

// std::string location;

template <typename InputIterator>
std::string make_jq_query (InputIterator begin, InputIterator end) {
	CONSTEXPR_STRING_VIEW pipe = " | " SV, single_quote = "\'" SV;
	return join_strings(begin, end, pipe, single_quote, single_quote);
}

template <typename Container>
std::string make_jq_query_from(const Container& params) {
	return " jq " + std::move(make_jq_query(params.begin(), params.end()));
}

// PASS ME argc-2 and argv + 2!!!
int assign_nodes(int paramc, const char** args, const char* output = temp_package_file) {
	std::vector<std::string> vect;
	CONSTEXPR_STRING_VIEW write_to = " > " SV;
	const std::regex _equals("([\\w-_\\.]+)=([\\w-_\\.]+)");
	const auto quoted_format = "\"$1\"=\"$2\"";
	std::cmatch matches;
	for (auto i = 0; i < paramc; i++) {
		const auto kvpair = args[i];
		auto matched = std::regex_replace(kvpair, _equals, quoted_format, 
			std::regex_constants::format_no_copy); // "key=value" -> "\"key\"=\"value\""
		if (!matched.empty()) {
			vect.emplace_back(filter_template + std::move(matched)); 
			// ."node-red"."nodes"."key"="value", as a string;
		}
	}
	return system(("cat package.json |" + make_jq_query_from(vect) + write_to + output).c_str());
}

CONSTEXPR_STRING_VIEW ASSIGN = "assign" SV, INIT = "init" SV;

int node_red_init() {
	int x = system("npm init");
	int y = system("cat package.json | jq '.\"node-red\".\"nodes\"={}' > " TEMP_PACKAGE_FILE);
	int z = system("mv " TEMP_PACKAGE_FILE " package.json");
	return x || y || z;
}

int main(int argc, char const *argv[])
{
	if (argc < 2)
		return node_red_init();
	if (argv[1] == ASSIGN) {
		if (argc > 2) {
			return assign_nodes(argc - 2, argv + 2);
		} else {
			std::cerr << "No assignments made to node-red nodes :(." << std::endl;
			std::cerr << "Usage: node_red_config assign key1=value1 [, key2=value2, ...]" << std::endl
				<< "\tto assign nodes in the module" << std::endl;
			return 1;
		}
	}
	if (argv[1] == INIT) {
		return node_red_init();
	}
	std::cerr << "Usage: node_red_config <init|assign> [keys]" << std::endl;
	std::cerr << "Usage: node_red_config init to do npm init and add node-red to package.json" << std::endl;
	std::cerr << "node_red_config assign key1=value1 [, key2=value2, ...]" << std::endl
		<< "\tto assign nodes in the module" << std::endl;
	return 2;
}