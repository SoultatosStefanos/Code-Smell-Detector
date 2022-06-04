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

		template <typename T>
		constexpr bool EqualOrNullptr(const T* lhs, const T* rhs) {
			return (!lhs or !rhs) ? (!lhs and !rhs) : AreEqual(*lhs, *rhs);
		}

	} // namespace

	static bool AreEqual(const Definition& lhs, const Definition& rhs) {
		return	EqualOrNullptr(lhs.GetType(), rhs.GetType())
				and lhs.GetFullType() == rhs.GetFullType();
	}

	static bool AreEqual(const Method& lhs, const Method& rhs) { // NOTE: No check for GetMemberExpr
		return  lhs.GetMethodType() == rhs.GetMethodType()
				and EqualOrNullptr(lhs.GetReturnType(), rhs.GetReturnType())
				and AreEqual(lhs.GetArguments(), rhs.GetArguments())
				and AreEqual(lhs.GetDefinitions(), rhs.GetDefinitions())
				and AreEqual(lhs.GetTemplateArguments(), rhs.GetTemplateArguments())
				and lhs.GetLiterals() == rhs.GetLiterals()
				and lhs.GetStatements() == rhs.GetStatements()
				and lhs.GetBranches() == rhs.GetBranches()
				and lhs.GetLoops() == rhs.GetLoops()
				and lhs.GetMaxScopeDepth() == rhs.GetMaxScopeDepth()
				and lhs.GetLineCount() == rhs.GetLineCount();
	}

	static bool AreEqual(const Structure& lhs, const Structure& rhs) {
		return 	lhs.GetStructureType() == rhs.GetStructureType()
				and EqualOrNullptr(lhs.GetTemplateParent(), rhs.GetTemplateParent())
				and EqualOrNullptr(lhs.GetNestedParent(), rhs.GetNestedParent())
				and AreEqual(lhs.GetMethods(), rhs.GetMethods())
				and AreEqual(lhs.GetFields(), rhs.GetFields())
				and AreEqual(lhs.GetBases(), rhs.GetBases())
				and AreEqual(lhs.GetContains(), rhs.GetContains())
				and AreEqual(lhs.GetFriends(), rhs.GetFriends())
				and AreEqual(lhs.GetTemplateArguments(), rhs.GetTemplateArguments());
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
		return 	lhs.GetClassType() == rhs.GetClassType()
				and lhs.GetClassType() != rhs.GetClassType()
				and lhs.GetAccessType() == rhs.GetAccessType()
				and lhs.GetID() == rhs.GetID() 
				and lhs.GetName() == rhs.GetName() 
				and lhs.GetNamespace() == rhs.GetNamespace() 
				and lhs.GetSourceInfo() == rhs.GetSourceInfo()
				and DispatchAreEqual(lhs, rhs);
		
	}

	bool AreEqual(const SymbolTable& lhs, const SymbolTable& rhs) {
		if (lhs.GetSize() != rhs.GetSize())
			return false;

		return std::all_of(std::begin(lhs), std::end(lhs), [&rhs](const auto& lpair) {
			return std::find_if(std::begin(rhs), std::end(rhs), [&lpair](const auto& rpair) {
				assert(lpair.second and rpair.second);
				return AreEqual(*lpair.second, *rpair.second);
			}) != std::end(rhs);
		});
	}

} // tests::utils