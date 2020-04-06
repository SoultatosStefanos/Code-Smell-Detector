/*
    Object Used on Methods
*/
namespace objects{
	struct C{
		int i;
	};
	
	struct B {
		C c; 
	};

	struct A {
		B b;
		B b2; 

		B method(){
			C cc = b.c; 
			B bb; 
			cc = bb.c;
			return b;
			//bb = a.b; */
		}
	};

	int main(){
		A a; 
		a.method();
		return 0;
	}
}