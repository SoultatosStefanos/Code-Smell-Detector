#pragma once

#include "SymbolTable.h"
#include <fstream>
#include <unordered_set>
#include <string_view>

namespace incremental {

	using SavingArchive = std::ofstream;
	using LoadingArchive = std::ifstream;

	class CompilationCache {
		using SymbolID = ID_T;
		using HashSet = std::unordered_set<SymbolID>;
		using Iter = HashSet::iterator;
		using CIter =  HashSet::const_iterator;

	public:
		bool Contains(const SymbolID& id) const { return cache.find(id) != cache.end(); }

		void Insert(const SymbolID& id) { cache.insert(id); }

		auto begin() noexcept -> Iter { return cache.begin(); }
		auto begin() const noexcept -> CIter { return cache.begin(); }
		auto cbegin() const noexcept -> CIter { return cache.cbegin(); }
		auto end() noexcept -> Iter { return cache.end(); }
		auto end() const noexcept -> CIter { return cache.end(); }
		auto cend() const noexcept -> CIter { return cache.cend(); }

	private:
		HashSet cache;
	};

	CompilationCache& GetCompilationCache();

	bool LoadingArchiveExists(const std::string_view fname);

	void DeserializeCompilationCache(LoadingArchive& from, CompilationCache& cache = GetCompilationCache());

	void SerializeCompilationCache(SavingArchive& to, const CompilationCache& cache = GetCompilationCache());

}