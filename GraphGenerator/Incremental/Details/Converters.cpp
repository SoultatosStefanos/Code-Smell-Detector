// Contains from string, to dependenciesMining::enums, converter functions.
// Soultatos Stefanos 2022

#include "Converters.h"
#include <unordered_map>
#include <cassert>

namespace incremental::details {

	using namespace dependenciesMining;

	namespace {

		template <typename Enum>
		using EnumTable = std::unordered_map<std::string, Enum>;

		// Safe access
		template <typename EnumTable>
		inline auto Get(const EnumTable& table, const std::string& str) {
			assert(table.find(str) != std::end(table));
			return table.at(str);
		} 

	} // namespace

	AccessType ToAccessType(const std::string& str) {
		static const EnumTable<AccessType> table{{ "public", AccessType::_public },
												 { "protected", AccessType::_protected },
												 { "private", AccessType::_private },
												 { "unknown", AccessType::unknown } };

		return Get(table, str);
	}

	StructureType ToStructureType(const std::string& str) {
		static const EnumTable<StructureType> table{{ "Class", StructureType::Class },
													{ "Struct", StructureType::Struct },
													{ "TemplateDefinition", StructureType::TemplateDefinition },
													{ "TemplateFullSpecialization", StructureType::TemplateFullSpecialization },
													{ "TemplateInstantiationSpecialization", StructureType::TemplateInstantiationSpecialization },
													{ "TemplatePartialSpecialization", StructureType::TemplatePartialSpecialization},
													{ "Undefined", StructureType::Undefined} };

		return Get(table, str);
	}

	MethodType ToMethodType(const std::string& str) {
		static const EnumTable<MethodType> table {{"Constructor_UserDefined", MethodType::Constructor_UserDefined },
												  {"Constructor_Trivial", MethodType::Constructor_Trivial },
												  {"Destructor_UserDefined", MethodType::Constructor_UserDefined },
												  {"Destructor_Trivial", MethodType::Destructor_Trivial },
												  {"OverloadedOperator_UserDefined", MethodType::OverloadedOperator_UserDefined },
												  {"OverloadedOperator_Trivial", MethodType::OverloadedOperator_Trivial },
												  {"UserMethod", MethodType::UserMethod },
												  {"TemplateDefinition", MethodType::TemplateDefinition },
												  {"TemplateFullSpecialization", MethodType::TemplateFullSpecialization },
												  {"TemplateInstantiationSpecialization", MethodType::TemplateInstantiationSpecialization },
												  {"Undefined", MethodType::Undefined } };

		return Get(table, str);
	}

} // incremental::details