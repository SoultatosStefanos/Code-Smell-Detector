#pragma warning(disable : 4996)
#pragma warning(disable : 4146)
#include <iostream>
#include <fstream>
#include "SourceLoader.h"
#include "DependenciesMining.h"
#include "GraphGeneration.h"
#include "GraphToJson.h"

static void PrintMainArgInfo(void) {
	std::cout << "MAIN ARGUMENTS:\n\n";
	std::cout << "argv[1]: \"--dir\" to mine whole directory (argv[2]: directory)\n";
	std::cout << "argv[1]: \"--cmp_db\" to use compilation database (argv[2]: path/to/compile_commands.json)\n";
	std::cout << "argv[3]: (file path) path/to/ignoredFilePaths\n";
	std::cout << "argv[4]: (file path) path/to/ignoredNamespaces\n";
	std::cout << "argv[5]: (file path) path/to/output\n";
}

int main(int argc, const char** argv) {
	if (argc < 2) {
		PrintMainArgInfo();
		return 1;
	}
	std::string option1 = "--dir";
	std::string option2 = "--cmp_db";
	const char* cmpDBPath = nullptr;
	std::vector<std::string> srcs;
	if (option1 == argv[1]) { // --dir
		sourceLoader::SourceLoader srcLoader(argv[2]);
		srcLoader.LoadSources();
		srcs = srcLoader.GetSources();
	}
	else if (option2 == argv[1]) { //--cmp_db
		cmpDBPath = argv[2];
	}
	else {
		PrintMainArgInfo();
		return 1;
	}
	const char* option = argv[1]; // option = "--dir", option = "--cmp_db"
	
	const char* ignoredFilePaths = (argc >= 4) ? argv[3] : "";
	const char* ignoredNamespaces = (argc >= 5) ? argv[4] : "";
	
	std::string fullPath = std::string(__FILE__);
	std::size_t found = fullPath.find_last_of("/\\");
	std::string jsonPath = (argc >= 6) ? argv[5] : fullPath.substr(0, found + 1) + "../../GraphVisualizer/Graph/graph.json";
	
	/*std::vector<std::string> srcs;
	srcs.push_back(path + "\\classes_simple.cpp");			
	srcs.push_back(path + "\\fields.cpp");					
	srcs.push_back(path + "\\friends.cpp");					
	srcs.push_back(path + "\\member_classes.cpp");			
	srcs.push_back(path + "\\methods_args_vars.cpp");		
	srcs.push_back(path + "\\methods.cpp");					
	srcs.push_back(path + "\\namespaces.cpp");			
	//srcs.push_back(path + "\\objects_used_on_methods.cpp");	
	srcs.push_back(path + "\\template_methods.cpp");		
	srcs.push_back(path + "\\template_types.cpp");			
	srcs.push_back(path + "\\templates.cpp");				

	srcs.push_back(path + "\\test0.cpp");					
	srcs.push_back(path + "\\include.h");						
	srcs.push_back(path + "\\include2.h");*/
				
	std::cout << "\n-------------------------------------------------------------------------------------\n\n";
	int result = dependenciesMining::CreateClangTool(cmpDBPath, srcs, ignoredFilePaths, ignoredNamespaces);
	graph::Graph graph = graphGeneration::GenetareDependenciesGraph(dependenciesMining::structuresTable);
	std::string json = graphToJson::GetJson(graph);

	std::ofstream jsonFile;

	jsonFile.open(jsonPath);
	jsonFile << json;
 	jsonFile.close();
	std::cout << "\n-------------------------------------------------------------------------------------\n\n";
}