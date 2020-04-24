namespace template_methods{
    struct X;
    template<typename T1, typename T2> struct template_Z;
    template<> struct template_Z <int, int>;
   
    struct B {};
    struct A {
      public:  
      void method(B x){}
    };
    struct S{}; 
    struct X : public A, B {
      template<typename T> void method2(T arg1, T arg2);
    };

    template<typename T1, typename T2> struct template_Z {  
      template<typename T> void method(T2 a, T b);
    };

    template <> 
    struct template_Z <int, int> {
        template<typename T> void method_int(){
            T i; 
        }
    };

    template <typename T> 
    struct template_Z <int, T>   
    { 
        template<typename T1>void m() ; 
    };

    template<> struct template_Z <int, float> ; 


    template<typename T1> struct template_Z2 : public template_Z <T1, int>{
        
    }; 

    int global_var;

    template<typename T> void X::method2(T arg1, T arg2){
        S var_method2_x;
        S *var_method2_px = new S();
    }

    int main(){
        X x; 
        x.method2(1,2);
        template_Z<float, float> float_z; 
        template_Z<int, char> z;
        float_z.method(2.1, x); 
        z.m<A>();
        return 0;
    }
}