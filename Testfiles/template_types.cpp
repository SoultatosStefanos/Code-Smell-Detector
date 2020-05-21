// templates Extra
namespace temp_exrta{
    // for testing
    struct S1 {};    
    struct S2 {};

    template<class T> struct Type {
        T t; 
        S1 s;
        int method(){ return 1; };        
    };

    // a nontype template param may not have class type
    // template<Type<int> N> struct Integral{

    // Integral
    template<int N> struct Integral{
        S1 s; 
        S2 method(){ return S2; };
    } ; 

        // Full Specialization 
    template<> struct Integral <2020> {
        S2 s; 
        S1 method(){ return S1(); };
    };

    // Template
    template<template <typename> class Cont> struct Template{
        S1 s;  
    };

    template<template <int> class ...Cont> struct Template_Integral{
        S1 s; 
        S1 method(){ return S1(); };
    };

    // Pack
    template<typename ...P>struct Type_Pack{
        S1 s;
        S1 method(){ return S1(); };
    };

    template<template <typename> class ...Cont> struct Template_Pack{
        S1 s; 
        S1 method(){ return S1(); };
    };

    template<template <template <typename> class> class ...Cont> struct Template_Template_Pack{
        S1 s; 
        S1 method(){ return S1(); };
    };

   
   template<typename T, int N> struct Combo{
       T t;
       T method(){ return t; };
    } ; 
}

using namespace temp_exrta;

int main(){
    const int x = 100;
    Type<int> t0;
    Type<S2> ts;
    Combo<Integral<2>, 2> c1;        // Type, integral
    //Integral<x> t2;                // integral
    //Integral<2+3> t3;              // integral
    Template<Type> t1; 
    Template_Integral<Integral> t2;
    Template_Integral<Integral, Integral> t3;
    Type_Pack<Type<int>, Integral<2>> t4;
    Template_Pack<Type, Type> t5;
    Template_Template_Pack<Template, Template> t6;
    return 0;
}