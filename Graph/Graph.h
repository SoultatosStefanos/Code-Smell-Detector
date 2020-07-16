#pragma once
#include <map>
#include <list>
#include <set>
#include <string>
#include "SymbolTable.h"
#include "uobject_untyped.h"

using namespace dependenciesMining; 

#define Undefined_dep_t "Undefined"

namespace graph {

    class Node; 

    class Edge {
    public:
      using Cardinality = unsigned;
      using DependencyType = std::string;
    private:
        std::map<DependencyType, Cardinality> dependencies;
        Node* to = nullptr;
    public:
        Edge(Node* to) : to(to) { assert(to); };
        Edge(const Edge& edge);

        Node* GetTo() const ;
        Cardinality GetCardinality(const DependencyType& depType) const;
        std::map<DependencyType, Cardinality> GetDependencies() const;
        void AddDependency(const DependencyType& depType, Cardinality card = 1);
    };


    class Node {
        untyped::Object data;
        std::list<Edge*> outEdges;
        std::map<ID_T, Edge*> byDestinationID; 
    public:
        Node() = default;   
        Node(const Node& node);

        ID_T GetID() const;
        untyped::Object& GetData();
        unsigned EdgesSize() const;
        void AddEdge(Edge* edge);
        void AddEdge(Node* to, const Edge::DependencyType& depType, Edge::Cardinality card = 1);
    };


    class Graph {
        std::set<Node*> nodes;
        std::map<ID_T, Node*> byID;
    public:
        Graph() = default;

        Node* GetNode(ID_T id) const;
        void AddNode(Node* node);
        void AddEdge(Node* from, Node* to, const Edge::DependencyType& depType, Edge::Cardinality card = 1);
    };
}