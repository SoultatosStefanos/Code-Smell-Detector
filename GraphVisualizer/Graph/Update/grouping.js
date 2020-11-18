import { diagram } from "../Appearance/graphAppearance.js"
import obs from "../../Observer/observer.js"
import louvainCommunities from "../clusteringAlgorithms/louvain.js"
import jLayeredLabelPropagation from "../ClusteringAlgorithms/layeredLabelPropagation.js"
import jLouvain from "../clusteringAlgorithms/jlouvain.js"
import config from "./configValues.js"

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
                        visibleWeight: config.showWeightsFlag
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
        config.recover.groupEdges(value);
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
        config.recover.groupingByNone();
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
    config.recover.groupingBy();
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
    config.recover.groupingBy();
}

function cleanFromGroups(type) {
    diagram.model.commit(function (m) {
        for (let i = 0; i < m.nodeDataArray.length; ++i) {
            let nodeData = m.nodeDataArray[i];
            if (nodeData.isGroup && nodeData.type === type) {
                m.removeNodeData(nodeData);
                --i;
            }
            else if (nodeData.isGroup)
                m.set(nodeData, 'visible', false)
        };
    });
}

/* 
    Given communities with a standard format create groups and insert them in the graph 
    {
        node_id1 : group_id1, 
        node_id2 : group_id2
    }
    node_idi: is the key of the node i 
    group_idi: is the group that node i own to 
*/
function clusteringGrouping(communities, type, m = diagram.model, fill = 'rgba(128,128,128,0.33)') {
    const communityKey = (key) => { return type + '_' + key };

    console.log(communities);

    const communitiesFiles = {};
    Object.values(communities).forEach((key) => {
        communitiesFiles[communityKey(key)] = {};
    });

    const groupsArray = [];
    m.nodeDataArray.forEach(nodeData => {
        const commKey = communityKey(communities[nodeData.key]);
        if (!nodeData.isGroup) {
            if (communitiesFiles[commKey][nodeData.data.srcInfo.cleanFileName] === undefined)
                communitiesFiles[commKey][nodeData.data.srcInfo.cleanFileName] = 1;
            else
                communitiesFiles[commKey][nodeData.data.srcInfo.cleanFileName] += 1;
        }
        if (!nodeData.isGroup && !groupsArray.some((group) => { return group.key === commKey })) {
            groupsArray.push({ key: commKey, name: commKey, isGroup: true, type, visible: true, fill });
        }
        m.set(nodeData, 'group', commKey);
    });

    groupsArray.forEach((group) => {
        const groupFiles = communitiesFiles[group.key];
        const name = Object.keys(groupFiles).reduce((a, b) => groupFiles[a] > groupFiles[b] ? a : b);
        group.name = name;
    });

    groupsArray.splice(0, 0, ...m.nodeDataArray);
    m.mergeNodeDataArray(groupsArray);
    config.recover.groupingBy();
}

/*
Given multi-level communities with a standard format create groups and insert them in the graph 
[
    node_id1 : {
        "nodeId": node_id1, 
        "path": [..path..]
    }, 
    node_id2 : ...
]

node_idi: is the key of the node i 
path: is the path from the outer to the inner cluster (ex. [1, 2, 3])

*/
function clusteringGroupingWithSubGroups(communities, type, m = diagram.model, fill = 'rgba(238, 255, 170, 0.33)') {
    const communityKey = (path, index) => {
        let commKey = type;
        for (let j = 0; j <= index; ++j) {
            commKey += '_' + path[j];
        }
        return commKey;
    };

    const communitiesFiles = {};
    const groupsArray = [];
    m.nodeDataArray.forEach(nodeData => {
        if (!nodeData.isGroup) {
            let path = communities[nodeData.key].path;
            let parentCommKey = undefined;
            for (let i = 0; i < path.length - 1; ++i) {        // ignore the last group since contains just the node
                const commKey = communityKey(path, i);
                if (!nodeData.isGroup) {
                    if (communitiesFiles[commKey] === undefined)
                        communitiesFiles[commKey] = {};
                    if (communitiesFiles[commKey][nodeData.data.srcInfo.cleanFileName] === undefined)
                        communitiesFiles[commKey][nodeData.data.srcInfo.cleanFileName] = 1;
                    else
                        communitiesFiles[commKey][nodeData.data.srcInfo.cleanFileName] += 1;
                }

                if (!groupsArray.some((group) => { return group.key === commKey })) {
                    groupsArray.push({ key: commKey, name: commKey, isGroup: true, type, visible: true, group: parentCommKey, fill });
                }
                if (i == path.length - 2) {
                    m.set(nodeData, 'group', commKey);
                }
                parentCommKey = commKey;
            }
        }
    });

    groupsArray.forEach((group) => {
        const groupFiles = communitiesFiles[group.key];
        const name = Object.keys(groupFiles).reduce((a, b) => groupFiles[a] > groupFiles[b] ? a : b);
        group.name = name;
    });

    groupsArray.splice(0, 0, ...m.nodeDataArray);
    m.mergeNodeDataArray(groupsArray);
    config.recover.groupingBy();
}

function groupingByLouvainOld() {
    cleanFromGroups('louvainOld');
    diagram.model.commit(function (m) {
        const communities = louvainCommunities(m.nodeDataArray, m.linkDataArray);
        clusteringGrouping(communities, 'louvainOld', m);
    });
}

function groupingByLouvain(mode = config.louvainMultiLevels) {
    cleanFromGroups('louvain');
    diagram.model.commit(function (m) {
        const nodes = m.nodeDataArray.map((node) => { if (!node.isGroup) return node.key }).filter(key => key !== undefined);
        const edges = m.linkDataArray.map(({ from, to, weight, type }) => {
            if (weight !== 0 && type === 'nodeEdge') {
                // remove the edges that has weight 0
                return { source: from, target: to, value: weight };
            }
        }).filter(key => key !== undefined);


        let communities = jLouvain(nodes, edges, 0.0000001, mode);

        clusteringGroupingWithSubGroups(communities, 'louvain', m, 'rgba(128,128,128,0.33)');
    });
}

function louvainMultiLevels(multi) {
    let value = multi ? 'multiLevels' : 'twoLevels';
    groupingByLouvain(value);
    config.recover.louvainMultiLevels(value);
}

function groupingByInfomap(mode = config.infomapMultiLevels) {
    cleanFromGroups('infomap');
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
                clusteringGroupingWithSubGroups(communities, 'infomap', m, 'rgba(238, 255, 170, 0.33)');
            });

        infomap.run(network, mode);
    });
}

function infomapMultiLevels(multi) {
    let value = multi ? '-d --silent' : '-d --two-level --silent';
    groupingByLouvain(value);
    config.recover.infomapMultiLevels(value);
}

function groupingByLayeredLabelPropagation(gamma = config.llpGamma) {
    cleanFromGroups('llp');
    diagram.model.commit(function (m) {
        const nodes = m.nodeDataArray.map((node) => { if (!node.isGroup) return node.key }).filter(key => key !== undefined);
        const edges = m.linkDataArray.map(({ from, to, weight, type }) => {
            if (weight !== 0 && type === 'nodeEdge')
                return { source: from, target: to, value: weight };
        }).filter(key => key !== undefined);

        let communities = jLayeredLabelPropagation(nodes, edges, gamma);
        clusteringGrouping(communities, 'llp', m, 'rgba(105, 196, 47, 0.33)');
    });
}

function layeredLabelPropagationGamma(gamma) {
    groupingByLayeredLabelPropagation(gamma);
    config.recover.llpGamma(gamma);
}

obs.install('groupEdges', groupEdges);

obs.install('groupingBynone', groupingByNone);
obs.install('groupingBynamespace', groupingByNamespace);
obs.install('groupingByfileName', groupingByFileName);
obs.install('groupingBylouvain', groupingByLouvain);
obs.install('groupingBylouvainOld', groupingByLouvainOld);
obs.install('louvainMultiLevels', louvainMultiLevels);
obs.install('groupingByinfomap', groupingByInfomap);
obs.install('infomapMultiLevels', infomapMultiLevels);
obs.install('groupingBylayeredLabelPropagation', groupingByLayeredLabelPropagation);
obs.install('layeredLabelPropagationGamma', layeredLabelPropagationGamma);

export default {
    groupEdges
}