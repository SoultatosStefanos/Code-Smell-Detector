export default function louvainCommunities(nodeData, edgeData) {
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
