import { diagram, contextMenu } from "./Appearance/graphAppearance.js"

/*
    2 types of layout are supported: 
    - ForceDirectedLayout
    - PackedLayout
*/
function LayoutHolder(goObj, layout = 'ForceDirectedLayout') {
    let go_layout = goObj;

    function setLayout(layout) {
        let $ = go.GraphObject.make;
        if (layout === 'PackedLayout') {
            go_layout = $(PackedLayout, {
                packShape: PackedLayout.Rectangular,
                spacing: 50
            });
            this.layout = layout;
        } else if (layout === 'ForceDirectedLayout') {
            go_layout = $(go.ForceDirectedLayout);
            this.layout = layout;
        } else {
            throw `"` + layout + `" is not a valid layout. 
                Supported layouts: 'PackedLayout', 'ForceDirectedLayout'`
        }
    }

    function getLayout() { return this.layout; }

    function getGoObject() { return go_layout; }

    return {
        setLayout,
        getLayout,
        getGoObject
    }
}


function NodeContextMenu() {
    let go_contextMenu = contextMenu.nodeContextMenu;

    function addProperty(description, action) {
        go_contextMenu.add(makeButton(description, (e, obj) => { action(obj.part.adornedPart, obj); }));
    }

    function getGoObject() { return go_contextMenu; }

    return { addProperty, getGoObject }
}


function Graph() {
    let go_diagram = diagram;
    let nodeDataArray = diagram.model.nodeDataArray;
    let edgeDataArray = diagram.model.linkDataArray;
    let nodeContextMenu = new NodeContextMenu();
    let layouts = {
        group: new LayoutHolder(go_diagram.groupTemplate.layout, 'PackedLayout'),
        outer: new LayoutHolder(go_diagram.layout, 'ForceDirectedLayout')
    };

    let m = diagram.model;

    // Get Nodes/Edges Arrays
    function getNodesArray() { return nodeDataArray; }
    function getEdgesArray() { return edgeDataArray; }
    function getNodeContextMenu() { return nodeContextMenu; }
    function getLayouts() { return layouts; }

    // Add/Remove Nodes/Edges from the graph
    function addNode(node) {
        let nodes = [node];
        nodes.splice(0, 0, ...m.nodeDataArray);
        m.mergeNodeDataArray(nodes);
    }

    function addNodes(nodes) {
        nodes.splice(0, 0, ...m.nodeDataArray);
        m.mergeNodeDataArray(nodes);
    }

    function removeNodeByKey(nodeKey) {
        let node = go_diagram.findNodeForKey(nodeKey);
        if (!node) return;
        m.removeNodeData(node.data);
    }

    function removeNodeByData(nodeData) {
        m.removeNodeData(nodeData);
    }

    function addEdge(edge) {
        let edges = [edge];
        edges.splice(0, 0, ...m.linkDataArray);
        m.linkDataArray = edges;
    }

    function addEdges(edges) {
        edges.splice(0, 0, ...m.linkDataArray);
        m.linkDataArray = edges;
    }

    function removeEdgeByData(edgeData) {
        m.removeLinkData(edgeData);
    }

    // Find a Node/Node Edges in the Graph
    function findNode(nodeKey) {
        let node = go_diagram.findNodeForKey(nodeKey);
        if (node)
            return node.data;
        else
            return undefined
    }

    function findNodeEdges(nodeKey) {
        let node = go_diagram.findNodeForKey(nodeKey);
        if (!node) return undefined;
        let links = node.findLinksConnected();
        let edgesArray = [];
        while (links.next()) {
            edgesArray.push(links.value.data);
        }
        return edgesArray;
    }

    // Insert a node in a group
    // groupKey == undefined if it has no group
    function setGroupToNodeByKey(nodeKey, groupKey) {
        let node = go_diagram.findNodeForKey(nodeKey);
        if (!node) return;
        m.set(node.data, 'group', groupKey);
    }

    function setGroupToNodeByData(nodeData, groupKey) {
        m.set(nodeData, 'group', groupKey);
    }

    function getGroupOfNodeByKey(nodeKey) {
        let node = go_diagram.findNodeForKey(nodeKey);
        if (!node) return;
        return node.data.group;
    }

    function getGroupOfNodeByData(nodeData) {
        return nodeData.group;
    }

    function nodeIsGroupByKey(nodeKey) {
        let node = go_diagram.findNodeForKey(nodeKey);
        if (!node) return;
        return node.data.isGroup;
    }

    function nodeIsGroupByData(nodeData) {
        return nodeData.isGroup;
    }

    // Hide/Show Node/Edge
    function hideNodeByKey(nodeKey) {
        let node = go_diagram.findNodeForKey(nodeKey);
        if (!node) return;
        m.set(node.data, 'visible', false);
    }

    function hideNodeByData(nodeData) {
        m.set(nodeData, 'visible', false);
    }

    function showNodeByKey(nodeKey) {
        let node = go_diagram.findNodeForKey(nodeKey);
        if (!node) return;
        m.set(node.data, 'visible', true);
    }

    function showNodeByData(nodeData) {
        m.set(nodeData, 'visible', true);
    }

    function hideEdgeByData(edgeData) {
        m.set(edgeData, 'visibleLink', false);
    }

    function showEdgeByData(edgeData) {
        m.set(edgeData, 'visibleLink', true);
    }

    // Hide/Show Edge Weight 
    function hideEdgeWeightByData(linkData) {
        m.set(linkData, 'visibleWeight', false);
    }

    function showEdgeWeightByData(linkData) {
        m.set(linkData, 'visibleWeight', true);
    }

    // Set Node/Edge Color 
    function setNodeColorByKey(nodeKey, color) {
        let node = go_diagram.findNodeForKey(nodeKey);
        if (!node) return;
        m.set(node.data, 'fill', color);
    }

    function setNodeColorByData(nodeData, color) {
        m.set(nodeData, 'fill', color);
    }

    function setEdgeColorByData(edgeData, color) {
        m.set(edgeData, 'color', color);
    }

    // Group/Outer Layout
    function setGroupLayout(layout) {
        layouts.group.setLayout(layout);
    }

    function setOuterLayout() {
        layouts.outer.setLayout(layout);
    }

    // nodeContextMenu
    function nodeContextMenuAddProperty(description, action) {
        nodeContextMenu.addProperty(description, action);
    }

    return {
        // Get Nodes/Edges Arrays
        getNodesArray,
        getEdgesArray,
        // Get node context menu object
        getNodeContextMenu, 
        // Get layouts object
        getLayouts,

        // Add/Remove Nodes/Edges from the graph
        addNode,
        addNodes,
        removeNodeByKey,
        removeNodeByData,
        addEdge,
        addEdges,
        removeEdgeByData,

        // Find a Node/Node Edges in the graph
        findNode,
        findNodeEdges,

        // Insert a node in a group
        // groupKey == undefined if it has no group
        setGroupToNodeByKey,
        setGroupToNodeByData,
        getGroupOfNodeByKey,
        getGroupOfNodeByData,
        nodeIsGroupByKey,
        nodeIsGroupByData,

        // Hide/Show Node/Edge
        hideNodeByKey,
        hideNodeByData,
        showNodeByKey,
        showNodeByData,
        hideEdgeByData,
        showEdgeByData,

        // Hide/Show Edge Weight 
        hideEdgeWeightByData,
        showEdgeWeightByData,

        // Set Node/Edge Color 
        setNodeColorByKey,
        setNodeColorByData,
        setEdgeColorByData,

        // Group/Outer Layout
        setGroupLayout,
        setOuterLayout,

        // nodeContextMenu 
        nodeContextMenuAddProperty
    }

};

let graph = new Graph();

function test() {
    graph.hideNodeByKey("TileColorsHolder");
    graph.setNodeColorByKey("Value", "red");
    graph.getEdgesArray().forEach((edgeData) => { graph.hideEdgeByData(edgeData, "red") });

    graph.addNode({
        key: "TEST NODE!!!",
        name: "TEST NODE!!!",
        data: {
            namespace: "TEST:: ",
            structureType: "",
            srcInfo: {}
        }
    });
    graph.setNodeColorByKey("TEST NODE!!!", "blue");
}

export default graph;