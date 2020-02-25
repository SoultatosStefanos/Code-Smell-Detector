namespace methods{
    class X;
    template<typename T1, typename T2> class template_Z;
    template<> class template_Z <int, int>;
   
    class B {};
    class A {
      public:  
      void method(B x){}
    };
    struct S{};
    class X : public A, B {
      void method2(int arg1, float arg2);
    };

    template<typename T1, typename T2> class template_Z {  
      void method();
    };

    template <> 
    class template_Z <int, int> {
         void method();
    };

    template <typename T> 
    class template_Z <int, T>   
    { };

    template<> class template_Z <int, float> ; 


    template<typename T1> class template_Z2 : public template_Z <T1, int>{}; 
}

int global_var;

void methods::X::method2(int arg1, float arg2){
     S var_method2_x;
     S *var_method2_px = new S();
}
