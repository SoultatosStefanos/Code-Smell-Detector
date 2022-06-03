// TODO  Import after exporting

// inline void ExportST(SymbolTable& table, const std::string_view jsonPath) {
// 		assert(!std::filesystem::exists(jsonPath));

// 		Json::Value jsonST;
// 		table.AddJsonSymbolTable(jsonST["structures"]);
// 		std::ofstream jsonSTFile{jsonPath.data()};
// 		jsonSTFile << jsonST;

// 		assert(std::filesystem::exists(jsonPath));
// 	}