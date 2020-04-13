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

	struct A {
		B b;
		B* b2; 

		B method(){
			C* pc = b2->pc; 
			C c = b.c;
			B bb; 
			c = bb.c;

			X x = bb.c.x;
			X* px = &(pc->x); 

			px->m1(); 
			x.m2().k; 		// MaterializeTemporaryExpr 						
			return b; 		
		}
	};

	int main(){
		A a; 
		a.method();
		return 0;
	}
}