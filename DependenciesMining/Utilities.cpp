#include "Utilities.h"

QualType GetTemplateArgType(TemplateArgument arg) {
	switch (arg.getKind()) {
	case TemplateArgument::Null:
		return QualType();
	case TemplateArgument::Type:
		return arg.getAsType();
	case TemplateArgument::Template:
	case TemplateArgument::TemplateExpansion:
	//	assert(0);
		return QualType();// arg.getAsTemplateOrTemplatePattern().getAsQualifiedTemplateName();
	case TemplateArgument::Pack:
		assert(0);
		return QualType();
	case TemplateArgument::Integral:
		return arg.getIntegralType();

	case TemplateArgument::Expression:
		return arg.getAsExpr()->getType();

	case TemplateArgument::Declaration:
		return arg.getParamTypeForDecl();
		//return arg.getAsDecl()->getType();

	case TemplateArgument::NullPtr:
		return arg.getNullPtrType();
	//arg.getNonTypeTemplateArgumentType();
	}
}

void AppendTemplateArgNameCallback(const TemplateArgument& templateArg, bool* lastArgTemplateSpecial, std::string* args) {
	if (*args != "<")
		*args += ", ";
	RecordDecl* d = nullptr;
	if (templateArg.getKind() == TemplateArgument::Template) {
		d = (RecordDecl*)templateArg.getAsTemplateOrTemplatePattern().getAsTemplateDecl();
	}
	else if(templateArg.getKind() == TemplateArgument::TemplateExpansion){
		d = (RecordDecl*)templateArg.getAsTemplateOrTemplatePattern().getAsTemplateDecl();
		std::cout << templateArg.getAsTemplateOrTemplatePattern().getAsQualifiedTemplateName() << "!\n";
	}
	auto argType = GetTemplateArgType(templateArg);
	if (d || argType->isStructureOrClassType()) {
		if(!d)
			d = argType->getAsCXXRecordDecl();
		auto qualifiedName = d->getQualifiedNameAsString();
		if (d->getKind() == d->ClassTemplateSpecialization || d->getKind() == d->ClassTemplatePartialSpecialization) {
			 *args += qualifiedName + GetInnerTemplateArgs(d);
			 *lastArgTemplateSpecial = true;
		}			
		else {
			*args += qualifiedName;
			*lastArgTemplateSpecial = false;
		}
	}
	else {
		*lastArgTemplateSpecial = false;
		*args += argType.getAsString();
	}
}

std::string GetInnerTemplateArgs(const RecordDecl* d) {
	auto temp = (ClassTemplateSpecializationDecl*)d;
	std::string args = "<";
	bool lastArgTemplateSpecial = false;															// gia kapoio logo an to last arg einai template pecial afhnei ena keno
	for (unsigned i = 0; i < temp->getTemplateArgs().size(); ++i) {
		TemplateArgsVisit(temp->getTemplateArgs()[i], AppendTemplateArgNameCallback, &lastArgTemplateSpecial, &args);
	}
	if (lastArgTemplateSpecial)
		args += " ";
	args += ">";
	return args;
}

std::string GetFullStructureName(const RecordDecl* d) {
	if (d->getKind() == d->ClassTemplateSpecialization || d->getKind() == d->ClassTemplatePartialSpecialization) {
		std::string args = GetInnerTemplateArgs(d);
		return (d->getQualifiedNameAsString() + args + "::" + d->getNameAsString());
	}
	else if (d->isCXXClassMember() && ((CXXRecordDecl*)d)->getInstantiatedFromMemberClass()) {
		return  d->getQualifiedNameAsString() + "::" + d->getNameAsString();
	}
	else {
		return d->getQualifiedNameAsString();
	}
}


std::string GetFullMethodName(const CXXMethodDecl* d) {
	std::string str = d->getType().getAsString();
	std::size_t pos = str.find("(");
	std::string argList = str.substr(pos);
	if (d->getTemplatedKind() == d->TK_FunctionTemplateSpecialization || d->getTemplatedKind() == d->TK_DependentFunctionTemplateSpecialization) {
		std::string tepmlateArgs = "<";
		auto args = d->getTemplateSpecializationArgs()->asArray();
		bool lastArgTemplateSpecial = false; 
		for (auto it : args) {
			TemplateArgsVisit(it, AppendTemplateArgNameCallback, &lastArgTemplateSpecial, &tepmlateArgs);
		}
		if (lastArgTemplateSpecial)
			tepmlateArgs += " ";
			
		tepmlateArgs += ">";
		return d->getQualifiedNameAsString() + tepmlateArgs + argList;
	}
	return d->getQualifiedNameAsString() + argList;
}


bool isStructureOrStructurePointerType(clang::QualType type) {
	if (!type->isStructureOrClassType()) {
		if (type->isPointerType()) {
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