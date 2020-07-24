#include "GraphToJson.h"
#define TO_ID_T() ToString()

using namespace graphToJson;

Json::Value GraphToJsonVisitor::StructureBuilding(const untyped::Object& data) {
	Json::Value curr;

	curr["id"] = data["id"].TO_ID_T();
	curr["name"] = data["name"].ToString();
	curr["namespace"] = data["namespace"].ToString();

	Json::Value srcInfo;
	const untyped::Object& srcInfoObj = data["srcInfo"].ToObject();
	srcInfo["fileName"] = srcInfoObj["fileName"].ToString();
	srcInfo["line"] = srcInfoObj["line"].ToNumber();
	srcInfo["column"] = srcInfoObj["column"].ToNumber();
	curr["srcInfo"] = srcInfo;

	curr["structureType"] = data["structureType"].ToString();

	if (data.In("templateParent"))
		curr["templateParent"] = data["templateParent"].TO_ID_T();

	if (data.In("nestedParent"))
		curr["nestedParent"] = data["nestedParent"].TO_ID_T();

	Json::Value bases;
	data["bases"].ToObject().ForEach([&bases](const untyped::Value& key, const untyped::Value& value) {
		bases[(int)key.ToNumber()] = value.ToString();
		});
	curr["bases"] = bases;

	Json::Value friends;
	data["friends"].ToObject().ForEach([&friends](const untyped::Value& key, const untyped::Value& value) {
		friends[(int)key.ToNumber()] = value.ToString();
		});
	curr["friends"] = friends;

	Json::Value templArgs;
	data["templateArguments"].ToObject().ForEach([&templArgs](const untyped::Value& key, const untyped::Value& value) {
		templArgs[(int)key.ToNumber()] = value.ToString();
		});
	curr["templateArguments"] = templArgs;

	Json::Value fields;
	data["fields"].ToObject().ForEach([&fields, this](const untyped::Value& key, const untyped::Value& value) {
		fields[key.TO_ID_T()] = DefinitionBuilding(value.ToObject());
		});
	curr["fields"] = fields;

	Json::Value methods; 
	data["methods"].ToObject().ForEach([&methods, this](const untyped::Value& key, const untyped::Value& value) {
		methods[key.TO_ID_T()] = MethodBuilding(value.ToObject());
		});
	curr["methods"] = methods;

	return curr;
}

Json::Value GraphToJsonVisitor::MethodBuilding(const untyped::Object& data) {
	Json::Value curr;
	curr["id"] = data["id"].TO_ID_T();
	curr["name"] = data["name"].ToString();
	curr["namespace"] = data["namespace"].ToString();

	Json::Value srcInfo;
	const untyped::Object& srcInfoObj = data["srcInfo"].ToObject();
	srcInfo["fileName"] = srcInfoObj["fileName"].ToString();
	srcInfo["line"] = srcInfoObj["line"].ToNumber();
	srcInfo["column"] = srcInfoObj["column"].ToNumber();
	curr["srcInfo"] = srcInfo;

	curr["methodType"] = data["methodType"].ToString();
	
	if (data.In("returnType"))
		curr["returnType"] = data["returnType"].TO_ID_T();

	Json::Value args;
	data["arguments"].ToObject().ForEach([&args, this](const untyped::Value& key, const untyped::Value& value) {
		args[key.TO_ID_T()] = DefinitionBuilding(value.ToObject());
		});
	curr["arguments"] = args;

	Json::Value defs;
	data["definitions"].ToObject().ForEach([&defs, this](const untyped::Value& key, const untyped::Value& value) {
		defs[key.TO_ID_T()] = DefinitionBuilding(value.ToObject());
		});
	curr["definitions"] = args;

	Json::Value templArgs;
	data["templateArguments"].ToObject().ForEach([&templArgs](const untyped::Value& key, const untyped::Value& value) {
		templArgs[(int)key.ToNumber()] = value.ToString();
		});
	curr["templateArguments"] = templArgs;

	Json::Value memberExprs; 
	int index = 0;
	auto& memberExprsObj = data["memberExprs"].ToObject();
	memberExprsObj.ForEach([&memberExprs, &index](const untyped::Value& key, const untyped::Value& value) {
		Json::Value memberExpr;
		auto& memberExprObj = value.ToObject(); 
		memberExpr["expr"] = memberExprObj["expr"].ToString();

		Json::Value srcInfo;
		const untyped::Object& srcInfoObj = memberExprObj["srcInfo"].ToObject();
		srcInfo["fileName"] = srcInfoObj["fileName"].ToString();
		srcInfo["line"] = srcInfoObj["line"].ToNumber();
		srcInfo["column"] = srcInfoObj["column"].ToNumber();
		memberExpr["srcInfo"] = srcInfo;

		Json::Value members;
		auto& membersObj = memberExprObj["members"].ToObject();
		membersObj.ForEach([&members](const untyped::Value& key, const untyped::Value& value) {
			Json::Value member;
			auto& memberObj = value.ToObject();
			member["name"] = memberObj["name"].ToString();
			member["type"] = memberObj["type"].TO_ID_T();

			Json::Value locEnd;
			const untyped::Object& locEndObj = memberObj["locEnd"].ToObject();
			locEnd["fileName"] = locEndObj["fileName"].ToString();
			locEnd["line"] = locEndObj["line"].ToNumber();
			locEnd["column"] = locEndObj["column"].ToNumber();
			member["locEnd"] = locEnd;

			members[(int)key.ToNumber()] = member;

			});
		memberExpr["members"] = members;

		memberExprs[index++] = memberExpr;
		});

	curr["memberExprs"] = memberExprs;

	return curr;
}

Json::Value GraphToJsonVisitor::DefinitionBuilding(const untyped::Object& data) {
	Json::Value curr;
	curr["id"] = data["id"].TO_ID_T();
	curr["name"] = data["name"].ToString();
	curr["namespace"] = data["namespace"].ToString();

	Json::Value srcInfo;
	const untyped::Object& srcInfoObj = data["srcInfo"].ToObject();
	srcInfo["fileName"] = srcInfoObj["fileName"].ToString();
	srcInfo["line"] = srcInfoObj["line"].ToNumber();
	srcInfo["column"] = srcInfoObj["column"].ToNumber();
	curr["srcInfo"] = srcInfo;

	curr["type"] = data["type"].TO_ID_T();
	return curr;
}


void GraphToJsonVisitor::VisitNode(Node* node) {
	untyped::Object data = node->GetData();
	ID_T id = node->GetID();
	json["nodes"][id] = StructureBuilding(data);

	node->ForEachEdge([&id, this](Edge* edge) {
		Json::Value edgeJson; 
		edgeJson["from"] = id; 
		json["edges"][edgesIndex] = edgeJson;
		VisitEdge(edge); 
		});
}

void GraphToJsonVisitor::VisitEdge(Edge* edge) {
	Json::Value& edgeJson = json["edges"][edgesIndex];

	edgeJson["to"] = edge->GetTo()->GetID();

	Json::Value dependencies;
	for (auto it : edge->GetDependencies()) {
		auto depType = it.first;
		auto cardinality = it.second; 
		dependencies[depType] = cardinality;
	}
	edgeJson["dependencies"] = dependencies;

	edgesIndex++;
}

std::string GraphToJsonVisitor::GetJsonAsString() const {
	return json.toStyledString();
}

std::string graphToJson::GetJson(const Graph& graph) {
	GraphToJsonVisitor visitor;
	graph.Accept(&visitor);
	return visitor.GetJsonAsString();
}
