/*
    Classes 
    Namespaces
    Inheritence
*/
namespace CS{
  namespace CS_1{
    class class_X;  
    class class_B;  
    class class_B {class_B* b;};
    class class_A { class_B b;};    
    struct struct_S {};
    class class_X : public class_A, class_B {}; 
    }
}

namespace N_2 {
  class class_A{};
}
using namespace CS::CS_1;

int main(){
  class_A a; 
  class_A *pa = new class_A;

  struct_S s; 
  struct_S *ps = new struct_S;

  class_X x;  
  class_X *px = new class_X;  

   return 0;
}
