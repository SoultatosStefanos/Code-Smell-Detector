/*
    Object Used on Methods
*/
namespace memberExpr{
	struct K {};

	struct Y {
		K k;
	};
	 
	struct X{
		int i;
		Y* m1(){
			return new Y(); 
		}

		Y m2(){
			Y y;
			return y; 
		}

	};

	struct C{
		int i;
		X x; 
	};
	
	struct B {
		C c; 
		C* pc; 
	};

	B f(B b) { return b;}

	struct A {
		B b;
		B* b2; 
		B m_b() {
			B b;
			return b;
		};

		B method(){
			C* pc = b2->pc; 
			C c = b.c;
			B bb; 
			c = bb.c;

			X x = bb.c.x;
			X* px = &(pc->x); 

			px->m1()->k; 
			x.m2().k;	
			(*(px->m1())).k;

			//(&bb).c;
			bb.c.x.m1()->k;		

			m_b().c.x;
			auto auto_c = f(b).c;

			 bb.c.i + b.c.i;
			return b; 		
		}
	};

	int main(){
		A a; 
		a.method();
		return 0;
	}
}