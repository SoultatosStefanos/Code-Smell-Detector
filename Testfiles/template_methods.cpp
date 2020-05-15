//#include <string>

namespace temp { 
    // struct X{

    // };
    template <typename T, template <typename> class ...Cont > // pack with templates
    class Matrix{
    public:
    int getSize() const{
        return 1; //return data.size();
    }
    private:
      //  T t;                                                             
    };


    template<class T> struct Y{
        // X* x;
        int f(){};

        
        
    }; 

    // Se aythn thn periptwsh (pou to class den einai template) vazei kanonika ta definitions + templateSpecialization type 
    // struct Z{

    //     template<typename T2> void method(T2 t, int i){
    //         T2 t2; 
    //         Tuple<int> tint;
    //     }

    //     template<> void method(X x, int i){
    //         X x2;
    //     }
    // };

    // template<class T, class ... Types> struct Tuple { 
    //     //Tuple<char>* tup = new Tuple<char>();
    //     Y<char>* y = new Y<char>();
    //     X x;

    //     void No_Templ(X x){ }

    //     template<typename T2> void method(T2 t){
    //         T2 t2; 
    //         //Tuple<int> tint;
    //     }

    //     // template<> void method(X x){
    //     //     X x2;
    //     //     x2 = x; 
    //     //     x2;
    //     // }
        
    // };

    // // tuple<int,int> is a templateInstatiationSpecial 
    // // and method is a templateFullSpecial
    // template<> template<> void Tuple<int, int>::method(Z z){
    //     Z z2;
    // }



    int main(){
        Matrix<int, Y> m; 
        Matrix<Matrix<Y<int>, Y>, Y, Y> m2; 
        
        // Tuple<char, Tuple<int>> t0;           // Types contains no arguments
        // X x;
        // t0.method("sss");
        // t0.method(x);

        // Z z;
        // Y<int> y; 
        // z.method(y, 1);
        // z.method(x, 2); 
        return 0;
    }
    // Tuple<int> t1;        // Types contains one argument: int
    // Tuple<int, float> t2; // Types contains two arguments: int and float



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