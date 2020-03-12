namespace templates{
    class A{};
    class B{};
    class C{};
    
    template<typename T1, typename T2> struct template_Z;
    template<> struct template_Z <int, int>  ;

    template<typename T1, typename T2> struct template_Z {
      A a;
    }; 
   
    template <> struct template_Z <int, int> {
      B b;
    };

    template <typename T> 
    struct template_Z <int, T> 
    {
      C c;
     };

    template<> struct template_Z <int, float> ; //  ignored (automaticly)

    template<typename T1,typename T2> class template_Z2 : public template_Z<T1,T2> {};   
    template<typename T1> class template_Z3 : public template_Z <T1, int>{};  //  
    template<typename T1> class template_Z4 : public template_Z <int, T1>{};  //  

    template<typename T1,typename T2> class T_FS : public template_Z <int, int>{};
    template<typename T1> class T_IS : public template_Z <char, A>{};

    class I1 : public template_Z <int , int>{};          // OK 
    class I2 : public template_Z <A, A>{};              // template_Z<A, A> is created (contains the field) - OK

    template<> struct template_Z <int, float> : public I1{

    };
}
/*
struct Point { float x, y; };

template <typename T> struct Point_traits {
  B b;
  float get_x (const T& p) const { return p.x; }
  float get_y (const T& p) const { return p.y; }
};

struct Vertex2 { float arr[2]; };

template <> struct Point_traits<Vertex2> {
  A a;
  float get_x (const Vertex2& p) const { return p.arr[0]; }
  float get_y (const Vertex2& p) const { return p.arr[1]; }
};
*/
using namespace templates;

int main(){
   
  template_Z<A, B> class_z; 
  template_Z<int, int> int_z; 
  template_Z<int, int> int_z2; 
  template_Z<float, float>* float_z;      // oi pointers den ftiaxnoyn specializations mexri na tous kanoume use 
 // T_IS<A>* tis;
  float_z->a;
  //tis->a;         
  template_Z2<char, char> z2; 
  template_Z3<int> z3; 
  template_Z3<char> z3_2; 
  template_Z4<char> z4; 
  //Point_traits<Point> p;
  int main_x;
  return 0;
}
