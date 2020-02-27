namespace templates{
    class A{};
    class B{};
    
    template<typename T1, typename T2> class template_Z;
    template<> class template_Z <int, int>  ;

    template<typename T1, typename T2> class template_Z {
      A field;
    };  // ignored (by me) 
   
    template <> class template_Z <int, int> {};

    template <typename T> 
    class template_Z <int, T>   // ignored (by me)
    { };

    template<> class template_Z <int, float> ; //  ignored (automaticly)


    template<typename T1> class template_Z2 : public template_Z <T1, int>{};  // ignored (by me) 

    class I1 : public template_Z <int , int>{};           
    class I2 : public template_Z <A, A>{};              // template_Z<A, A> is created (contains the field)
}

using namespace templates;

int main(){
   
  template_Z<A, B> class_z; 
  template_Z<int, int> int_z; 
  template_Z<float, float> float_z; 

    int main_x;
    return 0;
}
