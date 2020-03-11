namespace templates{
    class A{};
    class B{};
    class C{};
    
    template<typename T1, typename T2> class template_Z;
    template<> class template_Z <int, int>  ;

    template<typename T1, typename T2> class template_Z {
      A a;
    }; 
   
    template <> class template_Z <int, int> {
      B b;
    };

    template <typename T> 
    class template_Z <int, T> 
    {
      C c;
     };

    template<> class template_Z <int, float> ; //  ignored (automaticly)

    template<typename T1,typename T2> class template_Z2 : public template_Z <T1, T2>{};  // ignored (by me) 
    template<typename T1> class template_Z3 : public template_Z <T1, int>{};  // ignored (by me) 

    class I1 : public template_Z <int , int>{};           
    class I2 : public template_Z <A, A>{};              // template_Z<A, A> is created (contains the field)
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
  template_Z<float, float> float_z; 

  //Point_traits<Point> p;

    int main_x;
    return 0;
}
