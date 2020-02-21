class X;

struct A {
	X* x;
	void method(); 
};

class B {};

struct C {
	int c;
};

class X : public C {};

void A::method() {
	x->c;
}

int main() {
    A a; 
    return 0;
}