//#include <string>

namespace template_methods {
    struct B {
        int i ;
     };
    
    struct A {
        B b; 
    };
    
    struct C{
        A a ; 

    };

    struct S{
        // Type
        template <typename T> void method() {
            T t;
            t.a.b ;
            B b;  
        };

         template <typename T, int A> void method() {
             C c; 
         }


        template <> void method<A>(){
            A a; 
            a.b; 
        }

        // Integral 
        template<int N> void Integral() {
            B b; 
            b.i = N; 
        }

        // Pack
        template<typename ...P>void  Pack(){

        }
    };

}


using namespace template_methods;
int main(){
    S s; 
    s.method<C>();
    s.method<C, 2>();
    s.Integral<'a'>(); 
    return 0;
    
}



/*
namespace template_methods{
    struct X;
    template<typename T1, typename T2> struct template_Z;
    template<> struct template_Z <int, int>;
   
    struct B {

    };
    struct A {
      public:  
      B b;
      void method(B x){}
    };
    struct S{}; 
    struct X : public A, B {
      template<typename T> A* method2(T arg1, T arg2);
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
        template<typename T1>void m(){
            T1 t ;
            t.method2(1,2)->b;
        } 
    };

    template<> struct template_Z <int, float> ; 


    template<typename T1> struct template_Z2 : public template_Z <T1, int>{
        
    }; 

    int global_var;

    template<typename T> A* X::method2(T arg1, T arg2){
        S var_method2_x;
        S *var_method2_px = new S();
        return this; 
    }

    int main(){
        X x; 
        x.method2(1,2);
        template_Z<float, float> float_z; 
        template_Z<int, char> z;
        float_z.method(2.1, x); 
        z.m<X>();
        return 0;
    }
}*/

/* - Change GetStructureName + GetMethodName (+ insert extra template types (not ready yet))
- Remove from templateInstatiationSpecialization Classes the User and templateDefinition Methods (Not fullSpecialization because they can have there own fullspecialization method)
- */