namespace friends{
   
    class X;
    struct B{
        void method (B b);
    };
    class A {
        public:
        template<typename T> T method();
    };

     template<typename T> void tempF(){
        }
    


     typedef struct {

    } Anonymous1;
    
    typedef struct {
        Anonymous1 a; 

    } Anonymous2;

    void f(){}
    template<typename T1, typename T2> class template_Z;
    //template<typename T1, typename T2> class template_Z2;
    template<> class template_Z <int, int>;

    struct S {
        friend class X; 
        friend void B::method(B b);
        friend class template_Z <int, int>;
        friend class template_Z <int, float>;
        template<typename T1, typename T2> friend class template_Z;     // ~
        template<typename T1, typename T2> friend class template_Z2;    // only declaretion -> ignored
        template<typename T> friend T A::method();
        template<typename T> friend void tempF();
        friend Anonymous2;
        friend void f();                                                // ignored
    };

    template<typename T1, typename T2> 
    class template_Z {
        friend T1;
    };

    template <> 
    class template_Z <int, int>    
    { };

    template<typename T> T A::method() {
        template_Z<int,int> t;
        B b ; 
    }

    void B::method (B b) {
        A a;
        return; 
    }

}

int main() {
    friends::template_Z <friends::B, float> z;                            // friend with B
    friends::template_Z <int, float> t2; 
    friends::A a; 
    a.method<friends::template_Z <friends::B, float>>();
    return 0;
}