/*
    members
    (All the created classes are inserted)
*/

namespace members{
    class A{
        class M {                   
        };
        M m ;
    };

    struct B{
        struct M{                  
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
         class M_INT {                     
        };
        M_INT m ;
    }; 

    template<typename T1, typename T2> class T_2 {  
        public:
        template<typename T3> struct M_2 {  
            T3 t; 
        }; 
    };

    template<typename T1> struct T_3
    {
        template<typename T2>struct M_3{};
        M_3<T1> m;
    };
    
    struct T_4{
        template<typename T> struct M_4{};
    };

    struct S : public T<float, float>::M {

    };

    struct T_5 {
        struct M_5 : public A, public T_2<float, float>::M_2<A>{};
    };

    struct S_2 : public T_2<float, float>::M_2<A> {

    };

    struct S_3 : public A{

    };

    template<typename T1> struct S_4 : public T_2<float, float>::M_2<T1>{

    };

}

int main() {
    members::T<int, int> t; 
    members::T_2<float, float> t2; 
    members::T_2<char, char>::M_2<char> m2; 
    
    members::T_4::M_4<char> m4; 
    members::T_4::M_4<int> m_4;
    members::T_3<float>::M_3<int> m3; 
    members::T_5::M_5 m5;

    members::T_2<members::A,members::A>::M_2<members::T_2<members::B,members::B>> t_templ_2;
    members::T_2<members::A,members::A>::M_2<members::T_2<members::B,members::B>::M_2<members::B>> m_templ_2;
    m_templ_2.t;

    members::S_4<members::B> s;

  //  members::T<float, float>::M m;
   // members::T<char, char>::M m2;
}
