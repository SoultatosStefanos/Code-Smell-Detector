/*
    Fields
*/
namespace fields{
    template<typename T1, typename T2> class template_Z;
    template<> class template_Z <int, int>;
    struct struct_S;

    class class_A {
        char c;                                 // no
    };

    struct struct_S {
        int i;                                  // no 
        class_A a;                              // yes
    };
      
    template<typename T1, typename T2> class template_Z {  
        struct_S temp_s;                        // yes
    };

    template <> 
    class template_Z <float, int>    
    { 
        class_A temp_a;                         // yes
    };

    template <> 
    class template_Z <int, int>    
    { 
        template_Z <float, int> temp_t;         // yes

    };

    class class_B {
        int* pi;                                // no 
        class_A *a;                             // yes
        struct_S *s;                            // yes
        template_Z <float, int> *z;             // yes
    };
    
}


using namespace fields;

int main() {
    template_Z<float, float> z;                 // tepm_s
    return 0;
}

