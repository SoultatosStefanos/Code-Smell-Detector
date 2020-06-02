namespace friends{
   
    class X;
    class B{};
    class A {
        public:
        void method (B b);
        template<typename T> void method(){
        }
    };

     template<typename T> void tempF(){
        }
    
    void A::method (B b){
        A a;
        return; 
    }

     typedef struct {

    } Anonymous1;
    
    typedef struct {
        Anonymous1 a; 

    } Anonymous2;

    void f(){}
    template<typename T1, typename T2> class template_Z;
    template<> class template_Z <int, int>;

    struct S {
        friend class X; 
        friend void A::method(B b);
        friend class template_Z <int, int>;
        friend class template_Z <int, float>;
        template<typename T1, typename T2> friend class template_Z;     // ~
        template<typename T> friend void A::method();
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
}

int main() {
    friends::template_Z <friends::B, float> z;                            // friend with B
    friends::template_Z <int, float> t2; 
    return 0;
}