import { diagram } from "../Appearance/graphAppearance.js"
import louvainCommunities from "../clusteringAlgorithms/louvain.js"
import jLayeredLabelPropagation from "../ClusteringAlgorithms/layeredLabelPropagation.js"
import jLouvain from "../clusteringAlgorithms/jlouvain.js"
import recover from "./groupsRecover.js"
import configApplicator from "../configApplicator.js"
import clusteringAlgorithmsUtils from "../Utilities/clusteringAlgorithmsUtilities.js"

function groupEdges(value) {
    diagram.model.commit(function (m) {
        if (value) {
            for (let i = 0; i < m.linkDataArray.length; ++i) {
                let linkData = m.linkDataArray[i];
                if (linkData.type === 'groupEdge') {
                    m.removeLinkData(linkData);
                    i--;
                }
                else
                    m.set(linkData, 'visibleLink', false);
            };

            const nodesGroups = {};
            m.nodeDataArray.forEach((nodeData) => {
                if (!nodeData.isGroup && nodeData.key != undefined) nodesGroups[nodeData.key] = nodeData.group;
            });

            const groupEdges = {};
            m.linkDataArray.forEach((linkData) => {
                let fromGroup = nodesGroups[linkData.from];
                let toGroup = nodesGroups[linkData.to];
                if (groupEdges[nodesGroups[linkData.from]] === undefined) {
                    groupEdges[fromGroup] = {};
                    groupEdges[fromGroup][toGroup] = {};
                    groupEdges[fromGroup][toGroup].dependencies = { ...linkData.data.dependencies };
                    groupEdges[fromGroup][toGroup].weight = linkData.weight;
                } else {
                    if (groupEdges[fromGroup][toGroup] === undefined) {
                        groupEdges[fromGroup][toGroup] = {};
                        groupEdges[fromGroup][toGroup].dependencies = { ...linkData.data.dependencies };
                        groupEdges[fromGroup][toGroup].weight = linkData.weight;
                    } else {
                        groupEdges[fromGroup][toGroup].weight += linkData.weight;
                        Object.keys(linkData.data.dependencies).forEach((dep) => {
                            if (groupEdges[fromGroup][toGroup].dependencies[dep] === undefined) {
                                groupEdges[fromGroup][toGroup].dependencies[dep] = linkData.data.dependencies[dep];
                            } else {
                                groupEdges[fromGroup][toGroup].dependencies[dep] += linkData.data.dependencies[dep];
                            }
                        });
                    }
                }
            });

            const groupEdgesArray = [];
            let count = 0;
            Object.keys(groupEdges).forEach((from) => {
                Object.keys(groupEdges[from]).forEach((to) => {
                    groupEdgesArray.push({
                        from,
                        to,
                        weight: groupEdges[from][to].weight,
                        type: 'groupEdge',
                        data: { dependencies: groupEdges[from][to].dependencies },
                        visibleWeight: configApplicator.values.showWeightsFlag
                    });
                });
            });

            groupEdgesArray.splice(0, 0, ...m.linkDataArray);
            m.linkDataArray = groupEdgesArray;

        }
        else {
            for (let i = 0; i < m.linkDataArray.length; ++i) {
                let linkData = m.linkDataArray[i];
                if (linkData.type === 'groupEdge') {
                    m.removeLinkData(linkData);
                    i--;
                }
                else
                    m.set(linkData, 'visibleLink', true);
            }
        }
        // recover.groupEdges(value);
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
        // recover.groupingByNone();
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
    //recover.groupingBy();
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
    //recover.groupingBy();
}

function groupingByLouvainOld() {
    clusteringAlgorithmsUtils.setClusteringAlgorithmType('louvainOld');
    clusteringAlgorithmsUtils.graphCleanFromGroups('louvainOld');
    diagram.model.commit(function (m) {
        const communities = louvainCommunities(m.nodeDataArray, m.linkDataArray);
        clusteringAlgorithmsUtils.applyTwoLevelClustering(communities, 'louvainOld');
    });
}

function groupingByLouvain(mode = configApplicator.values.louvainMultiLevels) {
    clusteringAlgorithmsUtils.setClusteringAlgorithmType('louvain');
    clusteringAlgorithmsUtils.graphCleanFromGroups('louvain');
    diagram.model.commit(function (m) {
        const nodes = m.nodeDataArray.map((node) => { if (!node.isGroup) return node.key }).filter(key => key !== undefined);
        const edges = m.linkDataArray.map(({ from, to, weight, type }) => {
            if (weight !== 0 && type === 'nodeEdge') {
                // remove the edges that has weight 0
                return { source: from, target: to, value: weight };
            }
        }).filter(key => key !== undefined);


        let communities = jLouvain(nodes, edges, 0.0000001, mode);

        clusteringAlgorithmsUtils.applyMultiLevelClustering(communities, 'louvain', 'rgba(128,128,128,0.33)');
    });
}

function louvainMultiLevels(multi) {
    let value = multi ? 'multiLevels' : 'twoLevels';
    groupingByLouvain(value);
    // recover.louvainMultiLevels(value);
}

function groupingByInfomap(mode = configApplicator.values.infomapMultiLevels) {
    clusteringAlgorithmsUtils.setClusteringAlgorithmType('infomap');
    clusteringAlgorithmsUtils.graphCleanFromGroups('infomap');
    diagram.model.commit(function (m) {
        const nodes = m.nodeDataArray.map((node) => { if (!node.isGroup) return node.key }).filter(key => key !== undefined);
        let network = `*Vertices ` + nodes.length + '\n';
        nodes.forEach((nodeName, index) => {
            network += index + ' \"' + nodeName + '\"\n';
        });
        network += '*Edges ' + m.linkDataArray.length + '\n';
        m.linkDataArray.forEach(({ from, to, weight, type }) => {
            if (weight !== 0 && type === 'nodeEdge')
                network += nodes.indexOf(from) + ' ' + nodes.indexOf(to) + ' ' + weight + '\n';
        });

        const Infomap = window.infomap.default;
        let infomap = new Infomap()
            .on("data", data => console.log(data))
            .on("error", err => console.warn(err))
            .on("finished", data => {
                const tree = data.tree.split('\n');
                let communities = {};
                let levels = 0;
                for (let i = 8; i < tree.length; ++i) {
                    let res = tree[i].split(' ');
                    let path = res[0].split(':');
                    if (path.length > levels)
                        levels = path.length;
                    communities[nodes[res[3]]] = {
                        path,
                        flow: res[1],
                        nodeID: nodes[res[3]]
                    }
                }
                clusteringAlgorithmsUtils.applyMultiLevelClustering(communities, 'infomap', 'rgba(238, 255, 170, 0.33)');
                recover.groupingBy(configApplicator.values);

            });

        infomap.run(network, mode);
    });
}

function infomapMultiLevels(multi) {
    let value = multi ? '-d --silent' : '-d --two-level --silent';
    groupingByInfomap(value);
    //recover.infomapMultiLevels(value);
}

function groupingByLayeredLabelPropagation(gamma = configApplicator.values.llpGamma) {
    clusteringAlgorithmsUtils.setClusteringAlgorithmType('llp');
    clusteringAlgorithmsUtils.graphCleanFromGroups('llp');
    diagram.model.commit(function (m) {
        const nodes = m.nodeDataArray.map((node) => { if (!node.isGroup) return node.key }).filter(key => key !== undefined);
        const edges = m.linkDataArray.map(({ from, to, weight, type }) => {
            if (weight !== 0 && type === 'nodeEdge')
                return { source: from, target: to, value: weight };
        }).filter(key => key !== undefined);

        let communities = jLayeredLabelPropagation(nodes, edges, gamma);
        clusteringAlgorithmsUtils.applyTwoLevelClustering(communities, 'llp', 'rgba(105, 196, 47, 0.33)');
    });
}

function layeredLabelPropagationGamma(gamma) {
    groupingByLayeredLabelPropagation(gamma);
    // recover.llpGamma(gamma);
}

configApplicator.install('groupEdges', groupEdges, recover.groupEdges);

configApplicator.install('groupingBynone', groupingByNone, recover.groupingByNone);
configApplicator.install('groupingBynamespace', groupingByNamespace, recover.groupingBy);
configApplicator.install('groupingByfileName', groupingByFileName, recover.groupingBy);
configApplicator.install('groupingBylouvain', groupingByLouvain, recover.groupingBy);
configApplicator.install('groupingBylouvainOld', groupingByLouvainOld, recover.groupingBy);
configApplicator.install('louvainMultiLevels', louvainMultiLevels, recover.louvainMultiLevels);
configApplicator.install('groupingByinfomap', groupingByInfomap);
configApplicator.install('infomapMultiLevels', infomapMultiLevels, recover.infomapMultiLevels);
configApplicator.install('groupingBylayeredLabelPropagation', groupingByLayeredLabelPropagation, recover.groupingBy);
configApplicator.install('layeredLabelPropagationGamma', layeredLabelPropagationGamma, recover.llpGamma);

export default {
    groupEdges
}