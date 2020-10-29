/*
    Object Used on Methods
*/
namespace memberExpr{
	struct K {};

	struct Y {
		K k;
		Y& operator=(const Y& y){
			return *this;
		}
	};
	 
	struct X{
		int i;
		Y* m1(int i){
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

	B f(B b) { 
		return b;
	}

	template<typename T>struct A {
		B b;
		B* b2; 
		B m_b(int i) {
			B b;
			return b;
		};

		A<T>* m_a(){
			return new A<T>();
		}

		B method(){
			B* pb = &b; 
			pb->pc->x;
			
			C* pc = b2->pc; 
			C c = b.c;
			T bb; 
			c = bb.c;

			X x = bb.c.x;
			X* px = &(pc->x); 

			px->m1(1);	//->k; 
			x.m2().k;	
			(*(px->m1(1))).k;

			//(&bb).c;
			bb.c.x.m1(1)->k;		

			m_b(2).c.x;			// den ta bazei mesa sto template
			m_a()->b2->pc;		// den ta vazei mesa sto template // den vazei to a pou kanei return h m_a() giati h method anoikei sto this
			auto auto_c = f(b).c;

			bb.c.i + b.c.i;

			A<B> a;
			a.b2; 
			if(a.b.c.i == 1){
				a.b2;
			}
			return b; 		
		}
	};

	int main(){
		A<B> a; 
		a.method();
		return 0;
	}
}