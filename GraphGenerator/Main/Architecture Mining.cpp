#pragma warning(disable : 4996)
#pragma warning(disable : 4146)
#include <iostream>
#include <fstream>
#include "SourceLoader.h"
#include "DependenciesMining.h"
#include "GraphGeneration.h"
#include "GraphToJson.h"
#include "json/writer.h"

static void PrintMainArgInfo(void) {
	std::cout << "MAIN ARGUMENTS:\n\n";
	std::cout << "argv[1]: \"--src\" to mine whole directory with sources (argv[2]: directory/with/sources)\n";
	std::cout << "argv[1]: \"--cmp-db\" to use compilation database (argv[2]: path/to/compile_commands.json)\n";
	std::cout << "argv[3]: (file path) path/to/ignoredFilePaths\n";
	std::cout << "argv[4]: (file path) path/to/ignoredNamespaces\n";
	std::cout << "argv[5]: (file path) path/to/output\n";
	std::cout << "argv[6]: (file path) path/to/ST-output\n";
}

int main(int argc, const char** argv) {
	if (argc < 2) {
		PrintMainArgInfo();
		return 1;
	}
	std::string option1 = "--src";
	std::string option2 = "--cmp-db";
	const char* cmpDBPath = nullptr;
	std::vector<std::string> srcs;
	if (option1 == argv[1]) { // --src
		sourceLoader::SourceLoader srcLoader(argv[2]);
		srcLoader.LoadSources();
		srcs = srcLoader.GetSources();
	}
	else if (option2 == argv[1]) { //--cmp-db
		cmpDBPath = argv[2];
	}
	else {
		PrintMainArgInfo();
		return 1;
	}
	const char* option = argv[1]; // option = "--src", option = "--cmp-db"
	
	const char* ignoredFilePaths = (argc >= 4) ? argv[3] : "";
	const char* ignoredNamespaces = (argc >= 5) ? argv[4] : "";
	
	std::string fullPath = std::string(__FILE__);
	std::size_t found = fullPath.find_last_of("/\\");
	std::string jsonPath = (argc >= 6) ? argv[5] : fullPath.substr(0, found + 1) + "../../GraphVisualizer/Graph/graph.json";
	std::string jsonSTPath = (argc >= 7) ? argv[6] : fullPath.substr(0, found + 1) + "../ST.json";
	
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
	


	//graph::Graph graph = graphGeneration::GenetareDependenciesGraph(dependenciesMining::structuresTable);
	Json::Value jsonObj;
	jsonObj["structures"] = structuresTable.GetJsonSymbolTable();
	std::cout << jsonObj << std::endl;
	std::ofstream jsonSTFile(jsonSTPath);
	jsonSTFile << jsonObj;
	jsonSTFile.close();
	//std::string json = graphToJson::GetJson(graph);

	//std::ofstream jsonFile;

	//jsonFile.open(jsonPath);
	//jsonFile << json;
 	//jsonFile.close();
	std::cout << "\n-------------------------------------------------------------------------------------\n\n";
}