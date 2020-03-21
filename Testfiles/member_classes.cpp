/*
    members
*/

namespace members{
    class A{
        class M {                   // OK
        };
        M m ;
    };

    struct B{
        struct M{                  // OK
        };
    };

    template<typename T1, typename T2> class T {  
        public:
           class M {
               T1 x;
               void method() {};
           };
           M m;
           T1 t1;
    };

   template <> class T <int, int> {
         class M_INT {                      // OK
        };
        M_INT m ;
    }; 

    template<typename T1, typename T2> class T_2 {  
        public:
        template<typename T3> class M_2 {  
        }; 
    };

   /* template<typename T1> struct T_3
    {
        template<typename T2>struct M_3{};
        M_3<T1> m;
    };
    
    struct T_4{
        template<typename T> struct M_4{};
    };

    struct S : public members::T<float, float>::M {

    };


    struct S_2 : public members::T_2<float, float>::M_2<A> {

    };

    struct S_3 : public A{

    };*/


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
    /*members::T<int, int> t; 
    members::T_2<float, float> t2; 
    members::T_2<char, char>::M_2<char> m2; 
    
    members::T_4::M_4<char> m4; 
    members::T_4::M_4<int> m_4;
    members::T_3<float>::M_3<int> m3; 
*/
members::T_2<members::B,members::B>::M_2<members::B> a;
    members::T_2<members::A,members::A>::M_2< members::T_2<members::B,members::B>::M_2<members::B>> m_A;


  //  members::T<float, float>::M m;
   // members::T<char, char>::M m2;
}
