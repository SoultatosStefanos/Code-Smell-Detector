import { diagram } from "./graphAppearance.js"
import { totalWeight, louvainCommunities } from "./utilities.js"

(async () => {
  // Create graph from json
  const data = await fetch("./Graph/graph.json").then(response => response.json());

  const nodes = data.nodes;
  const edges = data.edges;

  const groupsHolder = [];
  groupsHolder.namespace = [];
  groupsHolder.fileName = [];
  groupsHolder.createGroup = function (type, key, fill = 'rgba(128,128,128,0.33)', visible = false) {
    if (!this[type].some((group) => { return group.key === key })) {
      this[type].push({ key, name: key, isGroup: true, type, fill, visible });
    }
  }

  const nodeDataArray = Object.keys(nodes).map(id => {

    const { id: key, name, namespace, srcInfo, methods, fields, bases, friends } = nodes[id];

    groupsHolder.createGroup('namespace', namespace, 'rgba(238, 255, 170, 0.33)');
    groupsHolder.createGroup('fileName', srcInfo.fileName, 'rgba(105,	196,	47, 0.33)');

    return { key, name, data: { namespace, fileName: srcInfo.fileName, methods, fields, bases, friends } };
  });

  nodeDataArray.splice(0, 0, ...groupsHolder.namespace);
  nodeDataArray.splice(0, 0, ...groupsHolder.fileName);

  const linkDataArray = edges.map(({ from, to, dependencies }) => {
    let weight = totalWeight(dependencies);
    return { from, to, weight, data: { dependencies} };
  });

  diagram.model = new go.GraphLinksModel(nodeDataArray, linkDataArray);
})();
