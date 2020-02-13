#pragma once
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

namespace sourceLoader {

	class SourceLoader {
		std::string path;
		std::vector<std::string> sources;

		bool IsSourceFile(const fs::path& p);
	public:
		SourceLoader(std::string path);

		void LoadSources();
		std::vector<std::string> GetSources();
		void PrintSourceFiles();
	};
}