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
	std::string path = argv[1];
	srcs.push_back(path + "\\member_classes.cpp");
	//srcs.push_back(path + "\\templates.cpp");

	std::cout << "\n-------------------------------------------------------------------------------------\n\n";
	int result = DependenciesMining::CreateClangTool(argc, argv, srcs);
	std::cout << "\n-------------------------------------------------------------------------------------\n\n";

	//return result;
}