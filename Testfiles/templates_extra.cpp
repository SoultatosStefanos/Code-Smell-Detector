// templates Extra
namespace temp_exrta{
     template<class T> struct Type{
        int f(){};        
    };

    // template<template <typename> class Cont> struct Template{
    // };


    // template<typename ...P>struct TypePack{
    // };

    // template<template <typename> class ...Cont> struct TemplatePack{
    // };

    template<int N> struct Integral{

    } ; 

    // specialization 
    template<> struct Integral <2020> {
        int x; 
    };

   template<typename T, int N> struct Combo{

    } ; 
}

using namespace temp_exrta;

int main(){
    const int x = 100;
    Combo<int, 2> t1;               // Type, integral
    Integral<x> t2;                 // integral
    Integral<2+3> t3;            // integral
    return 0;
}