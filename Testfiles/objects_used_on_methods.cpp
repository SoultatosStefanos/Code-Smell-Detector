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
		B* b2; 

		B method(){
			/*int my = 1+2;
			if(my==3){
				return;
			}*/
			C cc = b2->c; 
			B bb; 
			cc = bb.c;
			return b;
		}
	};

	int main(){
		A a; 
		a.method();
		return 0;
	}
}