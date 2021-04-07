#pragma once
#include "GraphVisitor.h"
#include <json/json.h>

using namespace graph;

namespace graphToJson {

	class GraphToJsonVisitor : public GraphVisitor {
		Json::Value json; 
		int edgesIndex = 0;

		Json::Value StructureBuilding(const untyped::Object& data);
		Json::Value MethodBuilding(const untyped::Object& data);
		Json::Value DefinitionBuilding(const untyped::Object& data);
	public:
		GraphToJsonVisitor() {
			json["nodes"] = Json::Value(); 
			json["edges"] = Json::Value(); 
		}

		virtual void VisitNode(Node* node);
		virtual void VisitEdge(Edge* edge);
		std::string GetJsonAsString() const;
		Json::Value GetJson() const;
	};

	std::string GetJsonString(const Graph& graph);
	Json::Value GetJson(const Graph& graph);
}
