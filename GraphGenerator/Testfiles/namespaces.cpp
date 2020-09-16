/*
    Insert("a::b::c");
    Insert("b");
*/
namespace a{
    namespace b{
        class B{};
        namespace c{
            class C{
                void method(){};
            };
        }
    }

    namespace b{
        class A2{
            void m(){};
        };
    }

    class A{};

}

namespace b {
  class class_A{};
}
