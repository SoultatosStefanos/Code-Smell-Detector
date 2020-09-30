import { diagram } from "./graphAppearance.js"
import { obs } from "../Observer/observer.js"

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
  diagram.model.commit(function (m) {
    m.linkDataArray.forEach((linkData) => {
      if (linkData.data.weight < value) {
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


obs.install("allEdges", showAllEdges);
obs.install("weightFilter", weightFilter);
obs.install("showWeights", showWeights);

obs.install("groupingBynamespace", groupingByNamespace);
obs.install("groupingByfileName", groupingByFileName);
obs.install("groupingBynone", groupingByNone);

obs.install("cycles", cyclesTracer); 