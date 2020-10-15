import { diagram } from "./graphAppearance.js"
import { obs } from "../Observer/observer.js"
import { totalWeight, louvainCommunities } from "./utilities.js"

function showAllEdges(value) {
  if (value) {
    diagram.model.commit(function (m) {
      m.linkDataArray.forEach((linkData) => {
        m.set(linkData, "visibleLink", true);
      });
    });
  }
};

function weightFilter(value) {
  console.log("weightFilter"); 
  diagram.model.commit(function (m) {
    m.linkDataArray.forEach((linkData) => {
      if (linkData.weight < value) {
        m.set(linkData, "visibleLink", false);
      }
      else {
        m.set(linkData, "visibleLink", true);
      }
    });
  }, "weightFilter");
}


function showWeights(value) {
  diagram.model.commit(function (m) {
    m.linkDataArray.forEach((linkData) => {
      m.set(linkData, "visibleWeight", value);
    });
  }, "showWeights");

};


function groupingByNamespace() {
  diagram.model.commit(function (m) {
    m.nodeDataArray.forEach((nodeData) => {
      if (!nodeData.isGroup)
        m.set(nodeData, "group", nodeData.data.namespace);
      else if (nodeData.type === "namespace")
        m.set(nodeData, "visible", true);
      else
        m.set(nodeData, "visible", false)
    }, "groupingByNamespace");
  });
}


function groupingByFileName() {
  diagram.model.commit(function (m) {
    m.nodeDataArray.forEach((nodeData) => {
      if (!nodeData.isGroup)
        m.set(nodeData, "group", nodeData.data.fileName);
      else if (nodeData.type === "fileName")
        m.set(nodeData, "visible", true)
      else
        m.set(nodeData, "visible", false)
    }, "groupingByFileName");
  });
}

function groupingByLouvain() {
  diagram.model.commit(function (m) {
    // remove the old Louvain groups and make invisible the other groups
    m.nodeDataArray.forEach(nodeData => {
      if (nodeData.isGroup && nodeData.type === "louvain")
        m.removeNodeData(nodeData);
      else if (nodeData.isGroup)
        m.set(nodeData, "visible", false)
    });

    const communities = louvainCommunities(m.nodeDataArray, m.linkDataArray);

    const groupsArray = [];
    m.nodeDataArray.forEach(nodeData => {
      const key = communities[nodeData.key];
      if (!nodeData.isGroup && !groupsArray.some((group) => { return group.key === key })) {
        groupsArray.push({ key, name: key, isGroup: true, type: "louvain", visible: true });
       // m.addNodeData({ key, name: key, isGroup: true, type: "louvain", visible: true });
      }
      m.set(nodeData, "group", key);
    });
    groupsArray.splice(0, 0, ...m.nodeDataArray);
     m.mergeNodeDataArray(groupsArray);
  });
}


function groupingByNone() {
  diagram.model.commit(function (m) {
    m.nodeDataArray.forEach((nodeData) => {
      if (!nodeData.isGroup)
        m.set(nodeData, "group", undefined);
      else
        m.set(nodeData, "visible", false)
    }, "groupingByNone");
  });
}


function cyclesTracer(value) {
  if (value) {
    console.log("lets trace cycles");
    m.set(linkData, "thick", 0.2);
  } else {
    console.log("Ignore cycles");
  }
}


function dependenciesConfig(data) {
  diagram.model.commit(function (m) {
    m.linkDataArray.forEach((linkData) => {
      m.set(linkData, "weight", totalWeight(linkData.data.dependencies, data));
    });
  }, "dependenciesConfig");
}



obs.install("allEdges", showAllEdges);
obs.install("weightFilter", weightFilter);
obs.install("showWeights", showWeights);

obs.install("groupingBynamespace", groupingByNamespace);
obs.install("groupingByfileName", groupingByFileName);
obs.install("groupingBylouvain", groupingByLouvain);
obs.install("groupingBynone", groupingByNone);

obs.install("cycles", cyclesTracer);

obs.install("dependenciesConfig", dependenciesConfig); 
