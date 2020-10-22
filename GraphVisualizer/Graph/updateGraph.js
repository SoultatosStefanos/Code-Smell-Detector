import { diagram } from "./Appearance/graphAppearance.js"
import { obs } from "../Observer/observer.js"
import { totalWeight } from "./utilities.js"
import louvainCommunities from "./clusteringAlgorithms/louvain.js"

function showAllEdges(value) {
  if (value) {
    diagram.model.commit(function (m) {
      m.linkDataArray.forEach((linkData) => {
        m.set(linkData, 'visibleLink', true);
      });
    });
  }
};

function weightFilter(value) {
  console.log('weightFilter');
  diagram.model.commit(function (m) {
    m.linkDataArray.forEach((linkData) => {
      if (linkData.weight < value) {
        m.set(linkData, 'visibleLink', false);
      }
      else {
        m.set(linkData, 'visibleLink', true);
      }
    });
  }, 'weightFilter');
}


function showWeights(value) {
  diagram.model.commit(function (m) {
    m.linkDataArray.forEach((linkData) => {
      m.set(linkData, 'visibleWeight', value);
    });
  }, 'showWeights');

};

function highlight(depKind) {
  return function () {
    diagram.model.commit(function (m) {
      if (depKind === 'None') {
        m.linkDataArray.forEach((linkData) => { m.set(linkData, 'color', '#555555'); });
      } else {
        m.linkDataArray.forEach((linkData) => {
          if (linkData.data.dependencies[depKind] !== undefined) 
            m.set(linkData, 'color', 'OrangeRed');
           else 
            m.set(linkData, 'color', '#555555');
        });
      }
    }, 'highlight');
  }
}


function groupingByNamespace() {
  diagram.model.commit(function (m) {
    m.nodeDataArray.forEach((nodeData) => {
      if (!nodeData.isGroup)
        m.set(nodeData, 'group', nodeData.data.namespace);
      else if (nodeData.type === 'namespace')
        m.set(nodeData, 'visible', true);
      else
        m.set(nodeData, 'visible', false)
    }, 'groupingByNamespace');
  });
}


function groupingByFileName() {
  diagram.model.commit(function (m) {
    m.nodeDataArray.forEach((nodeData) => {
      if (!nodeData.isGroup)
        m.set(nodeData, 'group', nodeData.data.srcInfo.fileName);
      else if (nodeData.type === 'fileName')
        m.set(nodeData, 'visible', true)
      else
        m.set(nodeData, 'visible', false)
    }, 'groupingByFileName');
  });
}

function groupingByLouvain() {
  diagram.model.commit(function (m) {
    // remove the old Louvain groups and make invisible the other groups
    m.nodeDataArray.forEach(nodeData => {
      if (nodeData.isGroup && nodeData.type === 'louvain')
        m.removeNodeData(nodeData);
      else if (nodeData.isGroup)
        m.set(nodeData, 'visible', false)
    });

    const communities = louvainCommunities(m.nodeDataArray, m.linkDataArray);

    const groupsArray = [];
    m.nodeDataArray.forEach(nodeData => {
      const key = communities[nodeData.key];
      if (!nodeData.isGroup && !groupsArray.some((group) => { return group.key === key })) {
        groupsArray.push({ key, name: key, isGroup: true, type: 'louvain', visible: true });
        // m.addNodeData({ key, name: key, isGroup: true, type: 'louvain', visible: true });
      }
      m.set(nodeData, 'group', key);
    });
    groupsArray.splice(0, 0, ...m.nodeDataArray);
    m.mergeNodeDataArray(groupsArray);
  });
}


function groupingByNone() {
  diagram.model.commit(function (m) {
    m.nodeDataArray.forEach((nodeData) => {
      if (!nodeData.isGroup)
        m.set(nodeData, 'group', undefined);
      else
        m.set(nodeData, 'visible', false)
    }, 'groupingByNone');
  });
}

function dependenciesConfig(data) {
  diagram.model.commit(function (m) {
    m.linkDataArray.forEach((linkData) => {
      m.set(linkData, 'weight', totalWeight(linkData.data.dependencies, data));
    });
  }, 'dependenciesConfig');
}


obs.install('allEdges', showAllEdges);
obs.install('weightFilter', weightFilter);
obs.install('showWeights', showWeights);
obs.install('highlightNone', highlight('None'));
obs.install('highlightClassField', highlight('ClassField'));
obs.install('highlightClassTemplateParent', highlight('ClassTemplateParent'));
obs.install('highlightClassTemplateArg', highlight('ClassTemplateArg'));
obs.install('highlightInherite', highlight('Inherite'));
obs.install('highlightFriend', highlight('Friend'));
obs.install('highlightNestedClass', highlight('NestedClass'));
obs.install('highlightMethodArg', highlight('MethodArg'));
obs.install('highlightMethodDefinition', highlight('MethodDefinition'));
obs.install('highlightMethodReturn', highlight('MethodReturn'));
obs.install('highlightMethodTemplateArg', highlight('MethodTemplateArg'));
obs.install('highlightMemberExpr', highlight('MemberExpr'));

obs.install('groupingBynamespace', groupingByNamespace);
obs.install('groupingByfileName', groupingByFileName);
obs.install('groupingBylouvain', groupingByLouvain);
obs.install('groupingBynone', groupingByNone);

obs.install('dependenciesConfig', dependenciesConfig); 
