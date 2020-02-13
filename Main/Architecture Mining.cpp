#pragma warning(disable : 4996)
#pragma warning(disable : 4146)
#include <iostream>
#include "../SourceLoader/SourceLoader.h"
#include "../DependenciesMining/DependenciesMining.h"

int main(int argc, const char** argv) {
	/*sourceLoader::SourceLoader srcLoader("C:\\Users\\user\\Desktop\\demo");
	srcLoader.LoadSources();
	std::vector<std::string> srcs = srcLoader.GetSources();
	srcLoader.PrintSourceFiles();
	*/

	std::vector<std::string> srcs;
	srcs.push_back(argv[1]);

	std::cout << "\n-------------------------------------------------------------------------------------\n\n";

	int result = CreateClangTool(argc, argv, srcs);

	std::cout << "\n-------------------------------------------------------------------------------------\n\n";

	return result;
}