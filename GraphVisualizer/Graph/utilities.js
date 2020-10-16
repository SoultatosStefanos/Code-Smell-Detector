export function totalWeight(dependencies, inData) {
    let dependenciesKeys = Object.keys(dependencies);
    let total = 0;
    dependenciesKeys.forEach((depKind) => {
        let data; 
        if(inData !== undefined && depKind.includes("Method"))
            data = inData["Methods"];
        else 
            data = inData;
            
        if (data === undefined || data[depKind] === undefined)
            total += dependencies[depKind];
        else if (data[depKind].custom === false) {
            if (data[depKind].countOnce === undefined || data[depKind].countOnce === false)
                total += dependencies[depKind];
            else
                total += 1;
        }
        else {
            if (data[depKind].countOnce === undefined || data[depKind].countOnce === false)
                total += data[depKind].weight * dependencies[depKind];
            else
                total += Number(data[depKind].weight);
        }
    });
    return Math.round((total + Number.EPSILON) * 100) / 100;
}


export function louvainCommunities(nodeData, edgeData) {
    const nodes = nodeData.map((node) => { if (!node.isGroup) return node.key }).filter(key => key !== undefined);
    const edges = edgeData.map(({ from, to, weight, data }) => {
        if (weight !== 0) {
            // remove the edges that has weight 0
            return { source: from, target: to, weight };
        }
    }).filter(key => key !== undefined);

    const communities = jLouvain()
        .nodes(nodes)
        .edges(edges)();

    return communities;
}


