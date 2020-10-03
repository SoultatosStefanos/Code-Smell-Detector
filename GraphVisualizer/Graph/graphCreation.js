import { diagram } from "./graphAppearance.js"

(async () => {
  // Create graph from json
  const data = await fetch("./Graph/graph.json").then(response => response.json());
  const nodes = data.nodes;
  const edges = data.edges;

  const namespacesGroups = [];
  const fileNameGroups = [];
  const nodeDataArray = Object.keys(nodes).map(id => {
    const { id: key, name, namespace, srcInfo, methods, fields, bases, friends} = nodes[id];
    if (!namespacesGroups.some((group) => { return group.key === namespace })) {
      namespacesGroups.push({ key: namespace, name: namespace, isGroup: true, type: "namespace", fill: "rgba(213, 213, 144, 0.33)", visible: false });
    }
    if (!fileNameGroups.some((group) => { return group.key === srcInfo.fileName })) {
      fileNameGroups.push({ key: srcInfo.fileName, name: srcInfo.fileName, isGroup: true, type: "fileName", visible: false });
    }
    return { key, name, data: { namespace, fileName: srcInfo.fileName, methods, fields, bases, friends } };
  });

  nodeDataArray.splice(0, 0, ...namespacesGroups);
  nodeDataArray.splice(0, 0, ...fileNameGroups);

  const linkDataArray = edges.map(({ from, to, dependencies }) => {
    let weight = Object.values(dependencies).reduce((counter, curr) => counter + curr, 0);
    return { from, to, weight, data: { dependencies, weight } };
  });

  diagram.model = new go.GraphLinksModel(nodeDataArray, linkDataArray);
})();
