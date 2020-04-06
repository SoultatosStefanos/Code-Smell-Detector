#include "Utilities.h"

template<typename T> void PrintLocation(T d, const MatchFinder::MatchResult& result) {
	auto& sm = *result.SourceManager;
	auto loc = d->getLocation().printToString(sm);
	std::cout << "\t" << loc << "\n\n";
}

std::string GetFullTemplateArgsName(const RecordDecl* d) {
	if (d->getKind() == d->ClassTemplateSpecialization || d->getKind() == d->ClassTemplatePartialSpecialization) {
		auto temp = (ClassTemplateSpecializationDecl*)d;
		auto qualifiedName = d->getQualifiedNameAsString();
		auto name = d->getNameAsString();
		std::string args = "<";
		bool lastArgTemplateSpecial = false;															// gia kapoio logo an to last arg einai template pecial afhnei ena keno
		for (unsigned i = 0; i < temp->getTemplateArgs().size(); ++i) {

			if (args != "<")
				args += ", ";
			if (temp->getTemplateArgs()[i].getAsType()->isStructureOrClassType()) {
				auto fullName = GetFullTemplateArgsName(temp->getTemplateArgs()[i].getAsType()->getAsCXXRecordDecl());
				if (fullName != temp->getTemplateArgs()[i].getAsType()->getAsCXXRecordDecl()->getQualifiedNameAsString())
					lastArgTemplateSpecial = true;
				else
					lastArgTemplateSpecial = false;
				args += fullName;
			}
			//else if (temp->getTemplateArgs()[i].getAsType()->isTemplateTypeParmType())				// an to valw etsi tha prepei na allaksw kai to onoma twn fields + methods gia na einai uniform 
			//	args += "-T-";
			else {
				args += temp->getTemplateArgs()[i].getAsType().getAsString();
				lastArgTemplateSpecial = false;
			}
		}
		if (lastArgTemplateSpecial)
			args += " ";
		args += ">";
		return (qualifiedName + args);
	}
	else {
		return d->getQualifiedNameAsString();
	}
}


std::string GetFullStructureName(const RecordDecl* d) {
	if (d->getKind() == d->ClassTemplateSpecialization || d->getKind() == d->ClassTemplatePartialSpecialization) {
		auto temp = (ClassTemplateSpecializationDecl*)d;
		auto qualifiedName = d->getQualifiedNameAsString();
		auto name = d->getNameAsString();
		std::string args = "<";
		bool lastArgTemplateSpecial = false;															// gia kapoio logo an to last arg einai template pecial afhnei ena keno
		for (unsigned i = 0; i < temp->getTemplateArgs().size(); ++i) {

			if (args != "<")
				args += ", ";
			if (temp->getTemplateArgs()[i].getAsType()->isStructureOrClassType()) {
				auto fullName = GetFullTemplateArgsName(temp->getTemplateArgs()[i].getAsType()->getAsCXXRecordDecl());
				if (fullName != temp->getTemplateArgs()[i].getAsType()->getAsCXXRecordDecl()->getQualifiedNameAsString())
					lastArgTemplateSpecial = true;
				else
					lastArgTemplateSpecial = false;
				args += fullName;
			}
			//else if (temp->getTemplateArgs()[i].getAsType()->isTemplateTypeParmType())				// an to valw etsi tha prepei na allaksw kai to onoma twn fields + methods gia na einai uniform 
			//	args += "-T-";
			else {
				args += temp->getTemplateArgs()[i].getAsType().getAsString();
				lastArgTemplateSpecial = false;
			}
		}
		if (lastArgTemplateSpecial)
			args += " ";
		args += ">";
		return (qualifiedName + args + "::" + name);
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