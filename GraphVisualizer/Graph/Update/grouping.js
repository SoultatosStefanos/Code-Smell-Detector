import { diagram } from "../Appearance/graphAppearance.js"
import { obs } from "../../Observer/observer.js"
import louvainCommunities from "../clusteringAlgorithms/louvain.js"
import jLayeredLabelPropagation from "../ClusteringAlgorithms/layeredLabelPropagation.js"
import jLouvain from "../clusteringAlgorithms/jlouvain.js"

var groupEdges_flag = false;

function groupEdges(value) {
    groupEdges_flag = value;
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
                    groupEdges[fromGroup][toGroup] = linkData.weight;
                } else {
                    if (groupEdges[fromGroup][toGroup] === undefined)
                        groupEdges[fromGroup][toGroup] = linkData.weight;
                    else
                        groupEdges[fromGroup][toGroup] += linkData.weight;
                }
            });

            const groupEdgesArray = [];
            let count = 0;
            Object.keys(groupEdges).forEach((from) => {
                Object.keys(groupEdges[from]).forEach((to) => {
                    groupEdgesArray.push({ from, to, weight: groupEdges[from][to], type: 'groupEdge', data: { dependencies: {} } });
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
            };
        }
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
    const old_groupEdges_flag = groupEdges_flag;
    groupEdges(false);
    groupEdges_flag = old_groupEdges_flag;
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
    groupEdges(groupEdges_flag);
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
    groupEdges(groupEdges_flag);
}

function clusteringGrouping(communities, type, m, fill = 'rgba(128,128,128,0.33)') {
    const communityKey = (key) => { return type + '_' + key };

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
    groupEdges(groupEdges_flag);
}

function clusteringGroupingWithSubGroups(communities, type, m, fill = 'rgba(238, 255, 170, 0.33)') {
    const communityKey = (path, index) => {
        let commKey = type;
        for (let j = 0; j <= index; ++j) {
            commKey += '_' + path[j];
        }
        return commKey;
    };

    // const communitiesFiles = {};
    // Object.values(communities).forEach((key) => {
    //     communitiesFiles[communityKey(key)] = {};
    // });

    const groupsArray = [];
    m.nodeDataArray.forEach(nodeData => {
        if (!nodeData.isGroup) {
            let path = communities[nodeData.key].path;
            let parentCommKey = undefined;
            for (let i = 0; i < path.length - 1; ++i) {        // ignore the last group since contains just the node
                const commKey = communityKey(path, i);
                // if (!nodeData.isGroup) {
                //     if (communitiesFiles[commKey][nodeData.data.srcInfo.cleanFileName] === undefined)
                //         communitiesFiles[commKey][nodeData.data.srcInfo.cleanFileName] = 1;
                //     else
                //         communitiesFiles[commKey][nodeData.data.srcInfo.cleanFileName] += 1;
                // }

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

    // groupsArray.forEach((group) => {
    //     const groupFiles = communitiesFiles[group.key];
    //     const name = Object.keys(groupFiles).reduce((a, b) => groupFiles[a] > groupFiles[b] ? a : b);
    //     group.name = name;
    // });

    groupsArray.splice(0, 0, ...m.nodeDataArray);
    m.mergeNodeDataArray(groupsArray);
    groupEdges(groupEdges_flag);
}

function groupingByLouvain() {
    diagram.model.commit(function (m) {
        for (let i = 0; i < m.nodeDataArray.length; ++i) {
            let nodeData = m.nodeDataArray[i];
            if (nodeData.isGroup && nodeData.type === 'louvain') {
                m.removeNodeData(nodeData);
                --i;
            }
            else if (nodeData.isGroup)
                m.set(nodeData, 'visible', false)
        };

        const communities = louvainCommunities(m.nodeDataArray, m.linkDataArray);
        clusteringGrouping(communities, 'louvain', m);
    });
}

function groupingByLouvain2() {
    diagram.model.commit(function (m) {
        for (let i = 0; i < m.nodeDataArray.length; ++i) {
            let nodeData = m.nodeDataArray[i];
            if (nodeData.isGroup && nodeData.type === 'louvain2') {
                m.removeNodeData(nodeData);
                --i;
            }
            else if (nodeData.isGroup)
                m.set(nodeData, 'visible', false)
        }

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

function groupingByInfomap(mode = '-d --two-level --silent') {
    diagram.model.commit(function (m) {
        for (let i = 0; i < m.nodeDataArray.length; ++i) {
            let nodeData = m.nodeDataArray[i];
            if (nodeData.isGroup && nodeData.type === 'infomap') {
                m.removeNodeData(nodeData);
                i--;
            }
            else if (nodeData.isGroup)
                m.set(nodeData, 'visible', false)
        };

        const nodes = m.nodeDataArray.map((node) => { if (!node.isGroup) return node.key }).filter(key => key !== undefined);
        let network = `*Vertices ` + nodes.length + '\n';
        nodes.forEach((nodeName, index) => {
            network += index + ' \"' + nodeName + '\"\n';
        });
        network += '*Edges ' + m.linkDataArray.length + '\n';
        m.linkDataArray.forEach(({ from, to, weight }) => {
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

        // infomap.run(network, " -d --silent");
        infomap.run(network, mode);
    });
}

function infomapMultiLevels(multi) {
    if (multi)
        groupingByInfomap('-d --silent');
    else
        groupingByInfomap('-d --two-level --silent');
}

function groupingByLayeredLabelPropagation(gamma = 0) {
    diagram.model.commit(function (m) {
        for (let i = 0; i < m.nodeDataArray.length; ++i) {
            let nodeData = m.nodeDataArray[i];
            if (nodeData.isGroup && nodeData.type === 'llp') {
                m.removeNodeData(nodeData);
                --i;
            }
            else if (nodeData.isGroup)
                m.set(nodeData, 'visible', false)
        };

        const nodes = m.nodeDataArray.map((node) => { if (!node.isGroup) return node.key }).filter(key => key !== undefined);
        const edges = m.linkDataArray.map(({ from, to, weight }) => {
            if (weight !== 0) {
                // remove the edges that has weight 0
                return { source: from, target: to, value: weight };
            }
        }).filter(key => key !== undefined);

        let communities = jLayeredLabelPropagation(nodes, edges, gamma);
        clusteringGrouping(communities, 'llp', m); //, 'rgba(105, 196,	47, 0.33)');
    });
}

function layeredLabelPropagationGamma(gamma) {
    groupingByLayeredLabelPropagation(gamma);
}

obs.install('groupEdges', groupEdges);

obs.install('groupingBynone', groupingByNone);
obs.install('groupingBynamespace', groupingByNamespace);
obs.install('groupingByfileName', groupingByFileName);
obs.install('groupingBylouvain', groupingByLouvain);
obs.install('groupingBylouvain2', groupingByLouvain2);
obs.install('groupingByinfomap', groupingByInfomap);
obs.install('infomapMultiLevels', infomapMultiLevels);
obs.install('groupingBylayeredLabelPropagation', groupingByLayeredLabelPropagation);
obs.install('layeredLabelPropagationGamma', layeredLabelPropagationGamma);