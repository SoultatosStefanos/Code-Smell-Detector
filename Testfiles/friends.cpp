namespace friends{
    class X;
    class B{};
    class A {
        public:
        void method (B b);
    };
    void f(){}
    template<typename T1, typename T2> class template_Z;
    template<> class template_Z <int, int>;

    struct S {
        friend class X; 
        friend void A::method(B b);
        friend class template_Z <int, int>;
        friend class template_Z <int, float>;
        template<typename T1, typename T2> friend class template_Z;     // ignored 
        friend void f();                                                // ignored
    };

    template<typename T1, typename T2> 
    class template_Z {};

    template <> 
    class template_Z <int, int>    
    { };
}

int main() {
   friends::template_Z <int, float> z;
    return 0;
}