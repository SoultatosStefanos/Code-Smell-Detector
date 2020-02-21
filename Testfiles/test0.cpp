/*
Classes
Template specialization 
Namespaces
*/
namespace N_1_1{
  namespace N_1{
    class class_X;

    class class_A {};

    
    class class_B {};
    
    struct struct_S {
        int i;
    };
    class class_X : public class_A, class_B {
      void method2(int arg1, float arg2);
    };

    template<typename T1, typename T2> class template_Z {  // ignored (by me) 
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

namespace N_2 {
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
    
  template_Z<float, float> int_z; 
  template_Z<int, int> float_z; 

    int main_x;
    return 0;
}
