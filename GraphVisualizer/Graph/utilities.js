export function totalWeight(dependencies, data) {
    let dependenciesKeys = Object.keys(dependencies);
    let total = 0;
    dependenciesKeys.forEach((depKind) => {
        if (data === undefined || data[depKind] === undefined || data[depKind] === 'default')
            total += dependencies[depKind];
        else if (data[depKind] === 'weighted')
            total += data[depKind + 'Weight'] * dependencies[depKind];
        else
            total += 1;
    });
    return total;
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


