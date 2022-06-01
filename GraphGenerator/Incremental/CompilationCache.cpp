#include "CompilationCache.h"
#include <cassert>

namespace incremental {

	CompilationCache& GetCompilationCache() {
		static CompilationCache cache;
		return cache;
	}

	void DeserializeCompilationCache(LoadingArchive& from, CompilationCache& cache) {
		static_assert(std::is_same_v<std::string, ID_T>,  "Cannot deserialize non-string type.");

		assert(from.good());
		assert(from.is_open());

		std::string str; 
		while (std::getline(from, str)) 
			cache.Insert(str);
	}

	void SerializeCompilationCache(SavingArchive& to, const CompilationCache& cache) {
		assert(to.good());
		assert(to.is_open());

		for (const auto& symbol : cache)
			to << symbol << '\n';

		assert(to.good());
		assert(to.is_open());
	}

}