/*
Classes
Template specialization 
Namespaces
Friends
*/

/*namespace N_1_1{
  namespace N_1{
    class class_X;
    template<typename T1, typename T2> class template_Z;
    template<> class template_Z <int, int>  ;
   
    class class_B {};
    class class_A {
      public:  
      void method(class_B x){}
    };

 
    
    struct struct_S {
        friend class class_X; 
        friend void class_A::method(class_B x);
        friend class template_Z <int, int> ;
         template<typename T1, typename T2> friend class template_Z;
        friend void f(); 
        class_A i;
    };
    class class_X : public class_A, class_B {
      void method2(int arg1, float arg2);
    };

    template<typename T1, typename T2> class template_Z {  // ignored (by me) 
   //   void method();
    };

    template <> 
    class template_Z <int, int>    
    { };

    template <typename T> 
    class template_Z <int, T>   // ignored (by me)
    { };

    template<> class template_Z <int, float> ; //  ignored (automaticly)


    template<typename T1> class template_Z2 : public template_Z <T1, int>{};  // ignored (by me) 
  }
}

int global_var;

void N_1_1::N_1::class_X::method2(int arg1, float arg2){
     struct_S var_method2_x;
     struct_S *var_method2_px = new struct_S();
}

void f(){};*/

/*
template <class C>
struct A {
	template <class T> void f(const T&) {}
};

static void f() {
	A<std::string> a;
	a.f(std::vector<int>());
}
*/

/*namespace N_2 {
  class class_A{};
}
using namespace N_1_1::N_1;

int main(){
  class_A a; 
  class_A *pa = new class_A;

  struct_S s; 
  struct_S *ps = new struct_S;

  class_X x;  
  class_X *px = new class_X;  
    
  template_Z<class_A, class_B> class_z; 
  template_Z<int, int> int_z; 
  template_Z<float, float> float_z; 

    int main_x;
    return 0;
}
*/


#include "include2.h"

template <typename T> struct  template_T;
template <> struct  template_T<char>;

template <> struct template_T<int>{
  char c; 
};

// template <typename T> struct template_T{
//   int x; 
// };

// void template_T<char>::method(){
//   int x; 
// }


// template <> struct template_T<char>{
//   char c; 
//   void method();
// };

int main(){
  Symbol s; 
  return 0;
};