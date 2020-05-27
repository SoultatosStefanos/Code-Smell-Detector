//#include <string>

namespace template_methods {
    struct A; 
    template <typename T1, typename T2> struct Temp; 
    
    struct B {
        int i ;
     };
    
    struct A {
        B b; 
        template <typename T> void method_A() {
            T t;
            t.i ;
            B b;  
        };
    };
    
    struct C{
        A a ; 
    };

    template <typename T1, typename T2> struct Temp : public A{
        // Type
        template <typename T> void method() {
            T t;
            t.a.b ;
            B b;  
        };

         template <typename T, int A> void method() {
             C c; 
         }


        // Den to blepei ws template specialization (alla ws user method)
        // ClassScopeFunctionSpecialization (apo AST eida)
        template <> void method<A, 2>(){
            A a; 
            a.b; 
        }

        // template <> void method<B, 100>();

        // Integral 
        template<int N> void Integral();    // ignores the declerations
        

        // Pack
        template<typename ...P>void  Pack(){

        }

    };

    // method definition out of the struct 
    template<typename T1, typename T2> template<int N> void Temp<T1,T2>::Integral() {
        B b; 
        b.i = N; 
    }

    //cannot specialize (with 'template<>') a member of an unspecialized template --> Clang Error
    // template<typename T1, typename T2> template<> void Temp<T1,T2>::method<B, 100>(){
    //         A a; 
    //         a.b; 
    // }
    
    // Method Full Specialization 
    template<> template<> void Temp<A, A>::Integral<12>() {
        A* a; 
        a->method_A<B>(); 
    }

    // Method Partial Specialization ---> is not allowed (by c++)
   /* template<> template<typename T> void Temp<A, A>::method<T, 22>() {
        A* a; 
        a->method_A<B>(); 
    }*/


    // Full Specialization 
    template <> struct Temp<B, B>{
        B b; 
        template<int ...P> B Int_Pack(){
            B b; 
            return b;
        }
    };

    // Partial Specialization 
    template <typename T> struct Temp<T, B*>{
        B b; 
        template<typename T1, typename T2> T2 method_partial(T1 a, T2 b){ 
            return b;
        }
    };


   
    //11 methods
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
        template<int N> void Integral();    // ignores the declerations
        

        // Pack
        template<typename ...P>void  Pack(){

        }
    };

    template<int N> void S::Integral() {
            B b; 
            b.i = N; 
    }

}


using namespace template_methods;
int main(){
    S s; 
    s.method<C>();
    s.method<C, 2>();
    s.Integral<'a'>();

    Temp<A, B> t; 
    t.method<C>(); 
    t.method<C, 2>();
    t.method<A, 2>();       // full specialized --> to vazei ws instatiation special sto Temp<A, B> alla me swsto body
                            // sto Temp to vazei ws user method xwris body  
    t.Integral<'a'>();

    Temp<B, B> tb; 
    B b = tb.Int_Pack<1,2,3>();

    Temp<int, B*> ti; 
    ti.method_partial(t, tb); 
    return 0;  
}
