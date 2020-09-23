import { diagram } from "./graphVisualization.js"
import { obs } from "../Observer/observer.js"

function showAllEdges(data) {
  if (data === true) {
    diagram.model.commit(function (m) {
      m.linkDataArray.forEach((linkData) => {
        m.set(linkData, "visibleLink", true);
      });
    });
  }
};

function weightFilter(data) {
  diagram.model.commit(function (m) {
    m.linkDataArray.forEach((linkData) => {
      if (linkData.data.weight < data) {
        m.set(linkData, "visibleLink", false);
      }
      else {
        m.set(linkData, "visibleLink", true);
      }
    });
  }, "weightFilter");
}

function showWeights(data) {
  diagram.model.commit(function (m) {
    m.linkDataArray.forEach((linkData) => {
      m.set(linkData, "visibleWeight", data);
    });
  }, "showWeights");

};

function namespaceGrouping(data) {
  diagram.model.commit(function (m) {
    m.nodeDataArray.forEach((nodeData) => {
      // TO FIX
      if (data) {
        // console.log("Namespace Grouping");
        if (!nodeData.isGroup)
          m.set(nodeData, "group", nodeData.namespace);
        else
          m.set(nodeData, "visible", true)
      }
      else {
        // console.log("No Namespace Grouping");
        if (!nodeData.isGroup)
          m.set(nodeData, "group", undefined);
        else
          m.set(nodeData, "visible", data)
      }
    });
  });
}

function cyclesTracer(data) {
  if (data === true) {
    console.log("lets trace cycles");
    m.set(linkData, "thick", 0.2);
  } else {
    console.log("Ignore cycles");
  }
}


obs.install("allEdges", showAllEdges);
obs.install("weightFilter", weightFilter);
obs.install("showWeights", showWeights);

obs.install("namespace", namespaceGrouping);

obs.install("cycles", cyclesTracer); 