namespace m_args_vars{
    class X;
    template<typename T1, typename T2> class template_Z;
    template<> class template_Z <int, int>;
   
    class B {};

    class A {
      public:  
      struct Inner {
          B method(B x){
            return x;
          }
      };
    };
    struct S{}; 
    class X : public A, B {
      int method2(int arg1, float arg2);
    };

    template<typename T1, typename T2> class template_Z {  
      void method(T2 a);
    };

    template <> 
    class template_Z <int, int> {
         void method_int(){
             int i; 
             A* a; 
         }
    };

    template <typename T> 
    class template_Z <int, T>   
    { };

    template<typename T1> class template_Z2 : public template_Z <T1, int>{}; 
}

int global_var;

using namespace m_args_vars;
int X::method2(int arg1, float arg2){
     S var_method2_x;
     S *var_method2_px = new S();
     return 1; 
}

int main(){
    template_Z<float, float> float_z; 
    template_Z2<char> z2;
}