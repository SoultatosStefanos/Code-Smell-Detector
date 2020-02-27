/*
    members
*/

namespace members{
    class A{
        class M {
        };
    };

    struct B{
        struct M{
        };
    };

    template<typename T1, typename T2> class T {  
        class M {
        };
    };

    template<typename T1, typename T2> class T_2 {  
        template<typename T3> class M {  
        }; 
    };

}

int main() {
    members::T<int, int> t; 
    members::T_2<float, float> t2; 
}
