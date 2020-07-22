#pragma warning(disable : 4996)
#pragma warning(disable : 4146)
#include <iostream>
#include "SourceLoader.h"
#include "DependenciesMining.h"
#include "GraphGeneration.h"

int main(int argc, const char** argv) {
	std::string path = argv[1];
	sourceLoader::SourceLoader srcLoader(path);
	srcLoader.LoadSources();
	std::vector<std::string> srcs = srcLoader.GetSources();
	//srcLoader.PrintSourceFiles();
	

/*	std::vector<std::string> srcs;
	srcs.push_back(path + "\\classes_simple.cpp");			// OK
	srcs.push_back(path + "\\fields.cpp");					// OK 
	/*srcs.push_back(path + "\\friends.cpp");					// OK 
	srcs.push_back(path + "\\member_classes.cpp");			// den eida polu, fainetai OK
	srcs.push_back(path + "\\methods_args_vars.cpp");			// OK
	srcs.push_back(path + "\\methods.cpp");					// OK
	srcs.push_back(path + "\\namespaces.cpp");				// OK
	srcs.push_back(path + "\\objects_used_on_methods.cpp");	// OK
	srcs.push_back(path + "\\template_methods.cpp");			// OK  - problem me full special mesa se template class
	srcs.push_back(path + "\\template_types.cpp");			// OK
	srcs.push_back(path + "\\templates.cpp");					// OK
	
	/*srcs.push_back(path + "\\test0.cpp");					
	srcs.push_back(path + "\\include.h");						
	srcs.push_back(path + "\\include2.h");		*/
				
	std::cout << "\n-------------------------------------------------------------------------------------\n\n";
	int result = dependenciesMining::CreateClangTool(argc, argv, srcs);
	graph::Graph graph = graphGeneration::GenetareDependenciesGraph(dependenciesMining::structuresTable);
	std::cout << "\n-------------------------------------------------------------------------------------\n\n";


}