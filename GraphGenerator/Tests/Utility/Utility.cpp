// Contains testing utilities.
// Soultatos Stefanos 2022

#include "Utility.h"
#include <algorithm>

namespace tests::utility {

	using namespace dependenciesMining;

	std::string ResolvePath(std::string from, const std::string& to) {
		const auto file_directory = std::string(dirname(from.data()));
		auto resolved_path = file_directory + "/" + to;
	
		assert((!from.empty() or !to.empty()) ? resolved_path.size() >= 2 : true);
		return resolved_path;
	}

	namespace {

		inline auto Sort(const SymbolTable& table) {
			using Range = std::vector<std::pair<ID_T, Symbol*>>;

			Range range{std::begin(table), std::end(table)};
			std::sort(std::begin(range), std::end(range), [](const auto& lhs, const auto& rhs) {
				return lhs.first < rhs.first; 
			});

			return range;
		}

		template <typename T>
		constexpr bool AreEqualOrNullptr(const T* lhs, const T* rhs) {
			return (!lhs or !rhs) ? (!lhs and !rhs) : AreEqual(*lhs, *rhs);
		}

		inline bool AreIDBasedEqual(const Structure* lhs, const Structure* rhs) {
			assert(lhs and rhs);
			return lhs->GetID() == rhs->GetID();
		}

		inline bool AreIDBasedEqualOrNullptr(const Structure* lhs, const Structure* rhs) {
			return (!lhs or !rhs) ? (!lhs and !rhs) : AreIDBasedEqual(lhs, rhs);
		}

	} // namespace

	static bool AreEqual(const Definition& lhs, const Definition& rhs) {
		return	AreIDBasedEqualOrNullptr(lhs.GetType(), rhs.GetType())
				and lhs.GetFullType() == rhs.GetFullType();
	}

	static bool AreEqual(const Method& lhs, const Method& rhs) { // NOTE: No check for GetMemberExpr
		return  lhs.GetMethodType() == rhs.GetMethodType()
				and AreIDBasedEqualOrNullptr(lhs.GetReturnType(), rhs.GetReturnType())
				and AreEqual(lhs.GetArguments(), rhs.GetArguments())
				and AreEqual(lhs.GetDefinitions(), rhs.GetDefinitions())
				and AreEqualIDBased(lhs.GetTemplateArguments(), rhs.GetTemplateArguments())
				and lhs.GetLiterals() == rhs.GetLiterals()
				and lhs.GetStatements() == rhs.GetStatements()
				and lhs.GetBranches() == rhs.GetBranches()
				and lhs.GetLoops() == rhs.GetLoops()
				and lhs.GetMaxScopeDepth() == rhs.GetMaxScopeDepth()
				and lhs.GetLineCount() == rhs.GetLineCount();
	}

	static bool AreEqual(const Structure& lhs, const Structure& rhs) {
		return 	lhs.GetStructureType() == rhs.GetStructureType()
				and AreEqualOrNullptr(lhs.GetTemplateParent(), rhs.GetTemplateParent())
				and AreEqualOrNullptr(lhs.GetNestedParent(), rhs.GetNestedParent())
				and AreEqual(lhs.GetMethods(), rhs.GetMethods())
				and AreEqual(lhs.GetFields(), rhs.GetFields())
				and AreEqualIDBased(lhs.GetBases(), rhs.GetBases())
				and AreEqual(lhs.GetContains(), rhs.GetContains())
				and AreEqualIDBased(lhs.GetFriends(), rhs.GetFriends())
				and AreEqualIDBased(lhs.GetTemplateArguments(), rhs.GetTemplateArguments());
	}

	static bool DispatchAreEqual(const Symbol& lhs, const Symbol& rhs) {
		switch (lhs.GetClassType()) {
		case ClassType::Definition:
			return AreEqual((const Definition&) lhs, (const Definition&) rhs);

		case ClassType::Method:
			return AreEqual((const Method&) lhs, (const Method&) rhs);

		case ClassType::Structure:
			return AreEqual((const Structure&) lhs, (const Structure&) rhs);
		
		case ClassType::Undefined:
			[[fallthrough]];
		default:
			assert(false); 
			return false;
		}
	}

	bool AreEqual(const Symbol& lhs, const Symbol& rhs) {
		return  	lhs.GetClassType() == rhs.GetClassType()
				and lhs.GetAccessType() == rhs.GetAccessType()
				and lhs.GetID() == rhs.GetID()
				and lhs.GetName() == rhs.GetName()
				and lhs.GetNamespace() == rhs.GetNamespace()
				and lhs.GetSourceInfo() == rhs.GetSourceInfo()
				and DispatchAreEqual(lhs, rhs);
		
	}

	bool AreEqual(const SymbolTable& lhs, const SymbolTable& rhs) {
		const auto lhsSorted = Sort(lhs);
		const auto rhsSorted = Sort(rhs);

		return 	lhsSorted.size() == rhsSorted.size() ? 
				std::equal(std::begin(lhsSorted), std::end(lhsSorted), std::begin(rhsSorted), [](const auto& lpair, const auto& rpair) {
					return lpair.first == rpair.first and AreEqual(*lpair.second, *rpair.second);
				}) :
				false;
	}

	bool AreEqualIDBased(const SymbolTable& lhs, const SymbolTable& rhs) {
		const auto lhsSorted = Sort(lhs);
		const auto rhsSorted = Sort(rhs);

		return 	lhsSorted.size() == rhsSorted.size() ? 
				std::equal(std::begin(lhsSorted), std::end(lhsSorted), std::begin(rhsSorted), [](const auto& lpair, const auto& rpair) {
					return lpair.first == rpair.first;
				}) :
				false;
	}

} // tests::utils