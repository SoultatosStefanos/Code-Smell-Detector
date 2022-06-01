#pragma warning(disable : 4996)
#pragma warning(disable : 4146)
#include <iostream>
#include <fstream>
#include "SourceLoader.h"
#include "DependenciesMining.h"
#include "GraphGeneration.h"
#include "GraphToJson.h"
#include "json/writer.h"
#include "ImportST.h"

static void PrintMainArgInfo(void) {
	std::cout << "MAIN ARGUMENTS:\n\n";
	std::cout << "argv[1]: \"--src\" to mine whole directory with sources (argv[2]: directory/with/sources)\n";
	std::cout << "argv[1]: \"--cmp-db\" to use compilation database (argv[2]: path/to/compile_commands.json)\n";
	std::cout << "argv[3]: (file path) path/to/ignoredFilePaths\n";
	std::cout << "argv[4]: (file path) path/to/ignoredNamespaces\n";
	std::cout << "argv[5]: (file path) path/to/ST-output\n";
}

static void SetDepedenciesToST(const Json::Value& graph, Json::Value& ST) {
	const Json::Value& all_dependencies = graph["edges"];
	auto& st_dependencies = ST["dependencies"];
	for (const auto& dependencies : all_dependencies) {
		
		Json::Value dependency_pack;
		dependency_pack["types"] = dependencies["dependencies"];
		dependency_pack["from"] = dependencies["from"];
		dependency_pack["to"] = dependencies["to"];

		st_dependencies.append(dependency_pack);
	}
}

static void SetCodeFilesToST(Json::Value& ST, const std::vector<std::string>& srcs, const std::vector<std::string>& headers) {
	for (const auto& path : srcs) {
		ST["sources"].append(path);
	}
	for (const auto& path : headers) {
		ST["headers"].append(path);
	}
}

int main(int argc, char const *argv[]) {
	if (argc < 6) {
		PrintMainArgInfo();
		return 1;
	}

	// ----------- Incremental starts here ------------- //

	incremental::ImportSTIfCached(argv[5],  dependenciesMining::structuresTable);

	// ----------- Incremental ends here ------------- //


	std::string option1 = "--src";
	std::string option2 = "--cmp-db";
	const char* cmpDBPath = nullptr;
	std::vector<std::string> srcs;
	std::vector<std::string> headers;
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
	
	//const char* ignoredFilePaths = (argc >= 4) ? argv[3] : "";
	//const char* ignoredNamespaces = (argc >= 5) ? argv[4] : "";
	const char* ignoredFilePaths = argv[3];
	const char* ignoredNamespaces = argv[4];

	std::string fullPath = std::string(__FILE__);
	std::size_t found = fullPath.find_last_of("/\\");
	/*std::string jsonPath = (argc >= 6) ? argv[5] : fullPath.substr(0, found + 1) + "../../GraphVisualizer/Graph/graph.json";
	std::string jsonSTPath = (argc >= 7) ? argv[6] : fullPath.substr(0, found + 1) + "../../ST0.json";*/
	std::string jsonSTPath = argv[5];
	
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
	int result = dependenciesMining::CreateClangTool(cmpDBPath, srcs, headers, ignoredFilePaths, ignoredNamespaces);
	


	
	Json::Value json_ST;
	structuresTable.AddJsonSymbolTable(json_ST["structures"]);
	std::ofstream jsonSTFile(jsonSTPath);
	
	graph::Graph graph = graphGeneration::GenetareDependenciesGraph(dependenciesMining::structuresTable);
	auto g = graphToJson::GetJson(graph);
	SetDepedenciesToST(g, json_ST);
	SetCodeFilesToST(json_ST, srcs, headers);
	jsonSTFile << json_ST;
	jsonSTFile.close();

	//std::cout << json_ST << std::endl;
	// --------- Phiv ends here -------------------
	/*std::string json_graph_str = graphToJson::GetJsonString(graph);
	
	std::ofstream jsonFile;

	jsonFile.open(jsonPath);
	jsonFile << json_graph_str;
 	jsonFile.close();*/
	std::cout << "\nCOMPILATION FINISHED\n";
}