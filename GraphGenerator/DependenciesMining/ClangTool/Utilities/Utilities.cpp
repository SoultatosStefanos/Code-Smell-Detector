#include "Utilities.h"

using namespace dependenciesMining;

bool dependenciesMining::isStructureOrStructurePointerType(const clang::QualType& type) {
	if (!type->isStructureOrClassType()) {
		if (type->isPointerType() || type->isReferenceType()) {
			if (!type->getPointeeType()->isStructureOrClassType()) {
				return false;
			}
		}
		else {
			return false;
		}
	}
	return true;
}


QualType dependenciesMining::GetTemplateArgType(const TemplateArgument& arg) {
	switch (arg.getKind()) {
	case TemplateArgument::Null:
		assert(0);
		return QualType();
	case TemplateArgument::Type:
		return arg.getAsType();
	case TemplateArgument::Template:
	case TemplateArgument::TemplateExpansion:
		return QualType();// arg.getAsTemplateOrTemplatePattern().getAsQualifiedTemplateName();
	case TemplateArgument::Pack:
		// den tha mpei pote edw giati to kanw handle ekswterika
		assert(0);
		return QualType();
	case TemplateArgument::Integral:
		return arg.getIntegralType();

	case TemplateArgument::Expression:
		return arg.getAsExpr()->getType();

	case TemplateArgument::Declaration:
		return arg.getParamTypeForDecl();

	case TemplateArgument::NullPtr:
		return arg.getNullPtrType();
	}
}

void dependenciesMining::AppendTemplateArgNameCallback(const TemplateArgument& templateArg, std::string* args) {
	if (*args != "<")
		*args += ", ";
	RecordDecl* d = nullptr;
	if (templateArg.getKind() == TemplateArgument::Template) {
		d = (RecordDecl*)templateArg.getAsTemplateOrTemplatePattern().getAsTemplateDecl();
	}
	else if (templateArg.getKind() == TemplateArgument::TemplateExpansion) {
		d = (RecordDecl*)templateArg.getAsTemplateOrTemplatePattern().getAsTemplateDecl();
		std::cout << templateArg.getAsTemplateOrTemplatePattern().getAsQualifiedTemplateName() << "!\n";
	}
	auto argType = GetTemplateArgType(templateArg);
	if (d || argType->isStructureOrClassType()) {
		if (!d)
			d = argType->getAsCXXRecordDecl();
		auto qualifiedName = d->getQualifiedNameAsString();
		auto name = d->getName().str();
		if (templateArg.getKind() == TemplateArgument::Integral) {
			llvm::SmallVector<char> str;
			templateArg.getAsIntegral().toString(str);
			name = "";
			for (auto c : str) {
				name += c;
			}
		}
		if (d->getKind() == d->ClassTemplateSpecialization || d->getKind() == d->ClassTemplatePartialSpecialization) {
			*args += qualifiedName + GetInnerTemplateArgs(d);
		}
		else {
			*args += name;
		}
	}
	else {
		std::string name = argType.getAsString();
		if (templateArg.getKind() == TemplateArgument::Integral) {
			llvm::SmallVector<char> str;
			templateArg.getAsIntegral().toString(str);
			name = "";
			for (auto c : str) {
				name += c;
			}
		}
		*args += name;
	}
}

std::string dependenciesMining::GetInnerTemplateArgs(const RecordDecl* d) {
	auto temp = (ClassTemplateSpecializationDecl*)d;
	std::string args = "<";
	for (unsigned i = 0; i < temp->getTemplateArgs().size(); ++i) {
		TemplateArgsVisit(temp->getTemplateArgs()[i], AppendTemplateArgNameCallback, &args);
	}
	args += ">";
	return args;
}

std::string dependenciesMining::GetFullStructureName(const RecordDecl* d) {
	std::string name;
	if (d->getKind() == d->ClassTemplateSpecialization || d->getKind() == d->ClassTemplatePartialSpecialization) {
		std::string args = GetInnerTemplateArgs(d);
		name = (d->getQualifiedNameAsString() + args + "::" + d->getNameAsString());
	}
	else if (d->isCXXClassMember() && ((CXXRecordDecl*)d)->getInstantiatedFromMemberClass()) {
		name = d->getQualifiedNameAsString() + "::" + d->getNameAsString();
	}
	else {
		name = d->getQualifiedNameAsString();
	}
	name.erase(std::remove_if(name.begin(), name.end(), isspace), name.end());
	return name;
}


std::string dependenciesMining::GetFullMethodName(const CXXMethodDecl* d) {
	std::string name;
	std::string str = d->getType().getAsString();
	std::size_t pos = str.find("(");
	std::string argList = str.substr(pos);
	if (d->getTemplatedKind() == d->TK_FunctionTemplateSpecialization || d->getTemplatedKind() == d->TK_DependentFunctionTemplateSpecialization) {
		std::string templateArgs = "<";
		auto args = d->getTemplateSpecializationArgs()->asArray();
		for (auto it : args) {
			TemplateArgsVisit(it, AppendTemplateArgNameCallback, &templateArgs);
		}
		templateArgs += ">";
		name = d->getQualifiedNameAsString() + templateArgs + argList;
	}
	else if (d->getTemplatedKind() == d->TK_FunctionTemplate) {
		auto funcTempl = d->getDescribedFunctionTemplate();
		auto params = funcTempl->getTemplateParameters()->asArray();// ->getPackAsArray();
		std::string templateArgs = "<";
		for (auto it : params) {
			if (templateArgs != "<")
				templateArgs += ", ";
			templateArgs += it->getDeclKindName();
		}
		templateArgs += ">";
		name = d->getQualifiedNameAsString() + templateArgs + argList;
	}
	else {
		name = d->getQualifiedNameAsString() + argList;
	}
	return name;
}

std::string dependenciesMining::GetFullNamespaceName(const RecordDecl* d) {
	auto* enclosingNamespace = d->getEnclosingNamespaceContext();
	std::string fullEnclosingNamespace = "";
	while (enclosingNamespace->isNamespace()) {
		auto enclosingNamespaceName = ((NamespaceDecl*)enclosingNamespace)->getNameAsString();
		fullEnclosingNamespace = enclosingNamespaceName + "::" + fullEnclosingNamespace;
		enclosingNamespace = enclosingNamespace->getParent()->getEnclosingNamespaceContext();
	}
	return fullEnclosingNamespace;
}

ID_T dependenciesMining::GetIDfromDecl(const RecordDecl* d) {
	return GetFullStructureName(d);
}

ID_T dependenciesMining::GetIDfromDecl(const CXXMethodDecl* d) {
	return GetFullMethodName(d);
}

ID_T dependenciesMining::GetIDfromDecl(const FieldDecl* d) {
	return d->getQualifiedNameAsString();
}

ID_T dependenciesMining::GetIDfromDecl(const VarDecl* d) {
	return d->getQualifiedNameAsString();
}

// -------------------------------------------------------------------------

bool dependenciesMining::isIgnoredDecl(const RecordDecl* d) {
	if (d->isImplicit()) {
		return true;
	}

	if (d->isAnonymousStructOrUnion()) {
		return true;
	}

	if (d->isUnion() || d->isEnum()) {
		return true;
	}

	if (d->isCXXClassMember()) {
		const auto* parent = d->getParent();
		if (isIgnoredDecl((RecordDecl*)parent)) {
			return true;
		}
	}

	return false;
}