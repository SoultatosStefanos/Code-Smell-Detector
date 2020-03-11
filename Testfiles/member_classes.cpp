/*
    members
*/

namespace members{
    /*class A{
        class M {                   // OK
        };
    };

    struct B{
        struct M{                  // OK
        };
    };*/

    template<typename T1, typename T2> class T {  
        public:
           class M {
               T1 x;
           };
           M m;
    };

    template <> class T <int, int> {
         class M_INT {                      // OK
        };
    };

    template<typename T1, typename T2> class T_2 {  
        template<typename T3> class M_2 {  
        }; 
    };

    // https://en.cppreference.com/w/cpp/language/member_template
   /* struct C {
    template<class T> struct B;         // primary member template
    template<class T> struct B<T*> { }; // OK: partial specialization
    //  template<> struct B<int*> { };      // Error: full specialization
    };
    template<> struct C::B<int*> { };       // OK
    template<class T> struct C::B<T&> { };  // OK
*/
}

int main() {
    members::T<int, int> t; 
    members::T_2<float, float> t2; 
  //  members::T<float, float>::M m;
   // members::T<char, char>::M m2;
   members::T<T1,T2>::M m ;
}
