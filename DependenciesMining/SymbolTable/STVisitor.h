#pragma once
#include "SymbolTable.h"

namespace dependenciesMining {

	class STVisitor {
	public:
		virtual void VisitStructure(Structure* s) = 0;
		virtual void VisitMethod(Method* m) = 0;
		virtual void VisitDefinition(Definition* s) = 0;
	};
}