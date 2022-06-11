#include "SourceLoader.h"

using namespace sourceLoader;

// FIXME

bool SourceLoader::IsSourceFile(const fs::path& p) {
	if (p.extension() == ".cpp" || p.extension() == ".h" || p.extension() == ".hh" || 
		p.extension() == ".CPP" || p.extension() == ".H" || p.extension() == ".HH")
		return true;
	return false;
}

SourceLoader::SourceLoader(std::string _path) : path(_path) {}


void SourceLoader::LoadSources() {
	if (!sources.empty())
		sources.clear();
	for (auto& p : fs::recursive_directory_iterator(path)) {
		if (IsSourceFile(p.path()))
			sources.push_back(p.path().string());
	}
}


std::vector<std::string> SourceLoader::GetSources() {
	if (sources.empty())
		LoadSources();
	return sources;
}


void SourceLoader::PrintSourceFiles() {
	for (auto& src : sources) {
		std::cout << src << '\n';
	}
}
