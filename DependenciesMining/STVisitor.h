#pragma once
#include "uobject_untyped.h"

namespace DependenciesMining {

	class Symbol;
	class Structure;
	class Method;
	class Definition;

	class STVisitor {
	//protected:
	//	untyped::Object data;
	public:
		//virtual void VisitSymbol(Symbol* s) = 0;
		virtual void VisitStructure(Structure* s) = 0;
		virtual void VisitMethod(Method* m) = 0;
		virtual void VisitDefinition(Definition* s) = 0;
		//virtual untyped::Object& GetData();
	};


	/*class DeepSTVisitor : public STVisitor {
	public:
		virtual void VisitSymbol(Symbol* s);
		virtual void VisitStructure(Structure* s);
		virtual void VisitMethod(Method* m);
		virtual void VisitDefinition(Definition* s);
	};

	class ShallowSTVisitor : public STVisitor {
	public:
		virtual void VisitSymbol(Symbol* s);
		virtual void VisitStructure(Structure* s);
		virtual void VisitMethod(Method* m);
		virtual void VisitDefinition(Definition* s);
	};*/

}