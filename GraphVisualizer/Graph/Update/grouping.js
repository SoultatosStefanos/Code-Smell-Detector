import { diagram } from "../Appearance/graphAppearance.js"
import { obs } from "../../Observer/observer.js"
import louvainCommunities from "../clusteringAlgorithms/louvain.js"
import jInfomap from "../clusteringAlgorithms/infoMap.js"
import jlouvain from "../clusteringAlgorithms/jlouvain.js"
import jLayeredLabelPropagation from "../ClusteringAlgorithms/layeredLabelPropagation.js"
import jLouvain from "../clusteringAlgorithms/jlouvain.js"

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

function clusteringGrouping(communities, type, m, fill = 'rgba(128,128,128,0.33)') {
    const communitiesFiles = {};
    Object.values(communities).forEach((commynityKey) => {
        communitiesFiles[commynityKey] = {};
    });

    const groupsArray = [];
    m.nodeDataArray.forEach(nodeData => {
        const key = communities[nodeData.key];
        if (!nodeData.isGroup) {
            if (communitiesFiles[key][nodeData.data.srcInfo.cleanFileName] === undefined)
                communitiesFiles[key][nodeData.data.srcInfo.cleanFileName] = 1;
            else
                communitiesFiles[key][nodeData.data.srcInfo.cleanFileName] += 1;
        }
        if (!nodeData.isGroup && !groupsArray.some((group) => { return group.key === key })) {
            groupsArray.push({ key, name: key, isGroup: true, type, visible: true, fill });
        }
        m.set(nodeData, 'group', key);
    });

    groupsArray.forEach((group) => {
        const groupFiles = communitiesFiles[group.key];
        const name = Object.keys(groupFiles).reduce((a, b) => groupFiles[a] > groupFiles[b] ? a : b);
        group.name = name;
    });

    groupsArray.splice(0, 0, ...m.nodeDataArray);
    m.mergeNodeDataArray(groupsArray);
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
        clusteringGrouping(communities, 'louvain', m);
    });
}

function groupingByLouvain2() {
    diagram.model.commit(function (m) {
        m.nodeDataArray.forEach(nodeData => {
            if (nodeData.isGroup && nodeData.type === 'louvain2')
                m.removeNodeData(nodeData);
            else if (nodeData.isGroup)
                m.set(nodeData, 'visible', false)
        });

        const nodes = m.nodeDataArray.map((node) => { if (!node.isGroup) return node.key }).filter(key => key !== undefined);
        const edges = m.linkDataArray.map(({ from, to, weight }) => {
            if (weight !== 0) {
                // remove the edges that has weight 0
                return { source: from, target: to, value: weight };
            }
        }).filter(key => key !== undefined);


        let communities = jLouvain(nodes, edges, 0.0000001);

        console.log(communities);
        clusteringGrouping(communities, 'louvain2', m);

    });
}

function groupingByInfomap() {
    diagram.model.commit(function (m) {
        m.nodeDataArray.forEach(nodeData => {
            if (nodeData.isGroup && nodeData.type === 'infomap')
                m.removeNodeData(nodeData);
            else if (nodeData.isGroup)
                m.set(nodeData, 'visible', false)
        });

        const nodes = m.nodeDataArray.map((node) => { if (!node.isGroup) return node.key }).filter(key => key !== undefined);
        const edges = m.linkDataArray.map(({ from, to, weight }) => {
            if (weight !== 0) {
                // remove the edges that has weight 0
                return { source: from, target: to, value: weight };
            }
        }).filter(key => key !== undefined);

        let communities = jInfomap(nodes, edges, 0.0000001);
        clusteringGrouping(communities, 'infomap', m, 'rgba(238, 255, 170, 0.33)',);
    });
}

function groupingByLayeredLabelPropagation() {
    diagram.model.commit(function (m) {
        m.nodeDataArray.forEach(nodeData => {
            if (nodeData.isGroup && nodeData.type === 'llp')
                m.removeNodeData(nodeData);
            else if (nodeData.isGroup)
                m.set(nodeData, 'visible', false)
        });

        const nodes = m.nodeDataArray.map((node) => { if (!node.isGroup) return node.key }).filter(key => key !== undefined);
        const edges = m.linkDataArray.map(({ from, to, weight }) => {
            if (weight !== 0) {
                // remove the edges that has weight 0
                return { source: from, target: to, value: weight };
            }
        }).filter(key => key !== undefined);

        let communities = jLayeredLabelPropagation(nodes, edges, 0);
        clusteringGrouping(communities, 'llp', m); //, 'rgba(105, 196,	47, 0.33)');
    });
}

obs.install('groupingBynone', groupingByNone);
obs.install('groupingBynamespace', groupingByNamespace);
obs.install('groupingByfileName', groupingByFileName);
obs.install('groupingBylouvain', groupingByLouvain);
obs.install('groupingBylouvain2', groupingByLouvain2);
obs.install('groupingByinfomap', groupingByInfomap);
obs.install('groupingBylayeredLabelPropagation', groupingByLayeredLabelPropagation);