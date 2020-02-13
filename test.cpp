//#include <iostream>
class class_A {};
class class_B {};
struct struct_S {
    int i;
};
class class_X : public class_A, class_B {
        void method() { int var_method_x; };
        void method2(int arg1, float arg2);
        /*  {struct_S var_method2_x;
          if(1){
            struct_S *var_method2_px = new struct_S();
          }}
        }*/
        int method_NOT();
        int field_x; 
        int *field_px;
};

template<typename T1, typename T2> class template_Z {       //getDescribedClassTemplate
};

template <> 
class template_Z <int, int>     // ClassTemplateSpecialization
{ };

template <typename T> 
class template_Z <int, T>   // ClassTemplatePartialSpecialization
{ };

template<> class template_Z <int, float> ; // instatiation - ClassTemplateSpecialization


template<typename T1> class template_Z2 : public template_Z <T1, int>{  //getDescribedClassTemplate

};

int global_var;

void class_X::method2(int arg1, float arg2){
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
int main(){
    class_A a; 
    class_A *pa = new class_A;

    struct_S s; 
    struct_S *ps = new struct_S;

    class_X x;  
    class_X *px = new class_X;  
    
   // template_Z<int, int> int_z; 
   // template_Z<float> float_z; 

    int main_x;
    return 0;
}
