import { diagram } from "./Appearance/graphAppearance.js"
import totalWeight from "./utilities/totalWeight.js"
import srcpathManager from "./utilities/srcpathMangment.js"

(async () => {
  // Create graph from json
  const data = await fetch("./Graph/graph.json").then(response => response.json());

  const nodes = data.nodes;
  const edges = data.edges;

  const groupsHolder = [];
  groupsHolder.namespace = [];
  groupsHolder.fileName = [];
  groupsHolder.createGroup = function (type, key, name = key, fill = 'rgba(128,128,128,0.33)', visible = false) {
    if (!this[type].some((group) => { return group.key === key })) {
      this[type].push({ key, name, isGroup: true, type, fill, visible });
    }
  }
  
  let commonPath = ""; 
  Object.keys(nodes).map(id => {
    commonPath = srcpathManager.commonPathDetection(nodes[id].srcInfo.fileName);
  });

  const nodeDataArray = Object.keys(nodes).map(id => {

    const { id: key, name, namespace, structureType, srcInfo, methods, fields, bases, friends, nestedParent, templateArguments} = nodes[id];

    srcInfo.fileName = srcpathManager.cleanDoubleDots(srcInfo.fileName);
    srcInfo.cleanFileName = srcInfo.fileName.substring(commonPath.length, srcInfo.fileName.length);

    groupsHolder.createGroup('namespace', namespace, namespace, 'rgba(238, 255, 170, 0.33)');
    groupsHolder.createGroup('fileName', srcInfo.fileName, srcInfo.cleanFileName, 'rgba(173, 216, 230, 0.4)');

    return { key, name, data: { 
                                namespace,
                                structureType,
                                srcInfo, 
                                fields, 
                                bases, 
                                friends,
                                methods, 
                                nestedParent,
                                templateArguments
                               } };
  });


  nodeDataArray.splice(0, 0, ...groupsHolder.namespace);
  nodeDataArray.splice(0, 0, ...groupsHolder.fileName);

  const linkDataArray = edges.map(({ from, to, dependencies }) => {
    let weight = totalWeight(dependencies);
    return { from, to, weight, data: { dependencies } };
  });

  diagram.model = new go.GraphLinksModel(nodeDataArray, linkDataArray);
})();
