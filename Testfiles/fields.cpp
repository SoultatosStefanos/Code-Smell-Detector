/*
    Fields
*/
namespace fields{
        class class_A {
            char c;                                 // no
        };

        struct struct_S {
            int i;                                  // no 
            class_A a;                              // yes
        };
        
        template<typename T1, typename T2> class template_Z {  
            class_B temp_b;                                 // no
        };

        template <> 
        class template_Z <int, int>    
        { 
            class_A temp_a;                 // yes
        };
}


using namespace fields;

int main() {
    template_Z<float, float> z;        // tepm_b (TODO CHECK)
    return 0;
}

