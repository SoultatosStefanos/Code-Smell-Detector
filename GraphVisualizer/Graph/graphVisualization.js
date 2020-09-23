const $ = go.GraphObject.make;
export const diagram =
  $(go.Diagram, "diagramDiv",
    {
      "undoManager.isEnabled": true,
      layout: $(go.ForceDirectedLayout)
    }
  );

(async () => {

  diagram.nodeTemplate =
    $(go.Node, "Auto",
      { locationSpot: go.Spot.Center, background: "lightblue", visible: true },
      new go.Binding("location", "loc", go.Point.parse),
      new go.Binding("visible", "visible"),
      $(go.Shape,
        "RoundedRectangle", { fill: "lightblue", stroke: "gray", strokeWidth: 2 }),
      $(go.TextBlock,
        "default",
        { margin: 12, stroke: "gray", font: "bold 16px sans-serif" },
        new go.Binding("text", "name")),

      {
        selectionChanged: function (part) {
          var shape = part.elt(0);
          shape.fill = part.isSelected ? "lightyellow" : "lightblue";
        }
      }
    );

  diagram.groupTemplate = $(go.Group, "Vertical",
    $(go.Panel, "Auto",
      $(go.Shape, "RoundedRectangle",  // surrounds the Placeholder
        {
          // parameter1: 14,
          fill: "rgba(128,128,128,0.33)",
          visible: true
        },
        new go.Binding("visible", "visible"),
      ),
      $(go.Placeholder,
        { padding: 5 })
    ),
    $(go.TextBlock,         // group title
      { alignment: go.Spot.Default, margin: 10, font: "Bold 12pt sans-serif" },
      new go.Binding("text", "name"))
  );

  diagram.linkTemplate =
    $(go.Link,
      { toShortLength: 6, toEndSegmentLength: 20, curve: go.Link.Bezier },
      new go.Binding("visible", "visibleLink"),
      $(go.Shape,                                           // link
        { strokeWidth: 0.5 },
        new go.Binding("strokeWidth", "thick")),
      $(go.Shape,                                           // arrow
        { toArrow: "Standard", stroke: null, scale: 0.9 }),
      $(go.TextBlock, { margin: 5, font: "bold 12px sans-serif", visible: true },                       // this is a Link label
        new go.Binding("text", "weight"),
        new go.Binding("visible", "visibleWeight")),
    );

  // Create graph from json 
  const data = await fetch("Graph/graph.json").then(response => response.json());
  const nodes = data.nodes;
  const edges = data.edges;

  const namespacesGroups = [];
  const nodeDataArray = Object.keys(nodes).map(id => {
    const { id: key, name, namespace } = nodes[id];
    if (!namespacesGroups.some((group) => { return group.key === namespace })) {
      namespacesGroups.push({ key: namespace, name: namespace, isGroup: true, type: "namespace"});
    }
    return { key, name, data: { namespace }, group: namespace };
  });

  nodeDataArray.splice(0, 0, ...namespacesGroups);

  const linkDataArray = edges.map(({ from, to, dependencies }) => {
    let weight = Object.values(dependencies).reduce((counter, curr) => counter + curr, 0);
    return { from, to, weight, data: { dependencies, weight } };
  });

  diagram.model = new go.GraphLinksModel(nodeDataArray, linkDataArray);
  //go.Diagram.fromDiv(document.getElementById("diagramDiv"))

})();
