#pragma once
#include <map>
#include <list>
#include <set>
#include <string>
#include "../DependenciesMining/SymbolTable.h"

using namespace DependenciesMining; 

namespace DependenciesGraph {
    class Node; 

    class Edge {
    public:
        enum class DependencyType {
            Undefined = -1,
            Inherite,
            Friend,
            NestedClass,
            ClassField,
            ClassTemplateParent,
            ClassTemplateArg,
            MethodReturn,
            MethodArg,
            MethodDefinition,
            MemberExpr,
            MethodTemplateArg
        };

        using Cardinality = unsigned;

    private:
        std::map<DependencyType, Cardinality> dependencies;
        Node* to = nullptr;

    public:
        Edge(Node* to) : to(to) { assert(to); };
        Edge(const Edge& edge);

        Node* GetTo() const ;
        Cardinality GetCardinality(const DependencyType& depType) const;
        std::map<DependencyType, Cardinality> GetDependencies() const;

        void RemoveDependency(const DependencyType& depType);

        void AddDependency(const DependencyType& depType, Cardinality card = 1);
    };

    class Node {
        Symbol* curr = nullptr;
        std::list<Edge*> outEdges;
    public:
        Node(Symbol* symbol) : curr(symbol) {};
        Node(const Node& node);

        Symbol* GetSymbol() const;
        unsigned EdgesSize() const;
        void AddEdge(Edge* edge);

        bool isAnalyzed() const;

        // iterator for edges
    };

    class Graph {
        std::set<Node*> nodes;
        std::map<ID_T, Node*> bySymbolID;

        // itarator for nodes
        Node* GetOrCreateNode(Symbol* symbol); 
        std::map<ID_T, Edge*> AnalyzeST(const SymbolTable& st, const Edge::DependencyType& depType = Edge::DependencyType::Undefined);
        std::map<ID_T, Edge*>& AddEdges(std::map<ID_T, Edge*>& to, const std::map<ID_T, Edge*>& from);
        std::map<ID_T, Edge*>& AddEdge(std::map<ID_T, Edge*>& m, const Edge& edge);
    public:
        Graph(const SymbolTable& st);
    };
}