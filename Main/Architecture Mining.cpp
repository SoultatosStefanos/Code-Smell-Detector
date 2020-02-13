#pragma warning(disable : 4996)
#pragma warning(disable : 4146)
#include <iostream>
#include "../SourceLoader/SourceLoader.h"

int main(int argc, const char** argv) {
	sourceLoader::SourceLoader srcLoader("C:\\Users\\user\\Desktop\\demo");
	srcLoader.LoadSources();
	std::vector<std::string> srcs = srcLoader.GetSources();
	srcLoader.PrintSourceFiles();

	return result;
}