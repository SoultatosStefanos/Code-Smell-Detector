import { diagram } from "../Appearance/graphAppearance.js"

function ClusteringAlgorithmsUtilities() {
    let clusteringAlgorithmType; 

    function setClusteringAlgorithmType(type){
        clusteringAlgorithmType = type;
    }

    function graphCleanFromGroups(type = clusteringAlgorithmType) {
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
    function applyTwoLevelClustering(communities, type = clusteringAlgorithmType, fill = 'rgba(128,128,128,0.33)', m = diagram.model) {
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
    function applyMultiLevelClustering(communities, type = clusteringAlgorithmType, fill = 'rgba(238, 255, 170, 0.33)', m = diagram.model) {
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
    }

    return {
        setClusteringAlgorithmType,
        graphCleanFromGroups,
        applyTwoLevelClustering, 
        applyMultiLevelClustering
    }
}

let clusteringAlgorithmsUtils = new ClusteringAlgorithmsUtilities();

export default clusteringAlgorithmsUtils;