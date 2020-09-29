

const $ = go.GraphObject.make;
export const diagram =
  $(go.Diagram, "diagramDiv",
    {
      "undoManager.isEnabled": true,
      initialAutoScale: go.Diagram.Uniform,  // an initial automatic zoom-to-fit
      contentAlignment: go.Spot.Center,
      layout:
        $(go.ForceDirectedLayout,
          { defaultSpringLength: 30, defaultElectricalCharge: 100 }),
      "SelectionMoved": function (e) { e.diagram.layout.invalidateLayout(); }
    }
  );

diagram.nodeTemplate =
  $(go.Node, "Auto",
    { locationSpot: go.Spot.Center, background: "lightblue", visible: true },
    new go.Binding("location", "loc", go.Point.parse),
    new go.Binding("visible", "visible"),
    $(go.Shape,
      "RoundedRectangle",
      {
        fill: "lightblue",
        stroke: "gray",
        strokeWidth: 2
      }),
    $(go.TextBlock,
      "default",
      {
        margin: 12,
        stroke: "gray",
        font: "bold 16px sans-serif"
      },
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
      new go.Binding("fill", "fill"),
    ),
    $(go.Placeholder,
      { padding: 7 })
  ),
  $(go.TextBlock,         // group title
    {
      alignment: go.Spot.Default,
      margin: 10,
      font: "Bold 12pt sans-serif",
      visible: true
    },
    new go.Binding("text", "name")),
  new go.Binding("visible", "visible")
);

diagram.linkTemplate =
  $(go.Link,
    { toShortLength: 6, toEndSegmentLength: 20, curve: go.Link.Bezier },
    new go.Binding("visible", "visibleLink"),
    $(go.Shape,                                           // link
      {
        strokeWidth: 0.6,
        stroke: "#555555"
      },
      new go.Binding("strokeWidth", "thick")),
    $(go.Shape,                                           // arrowhead
      {
        toArrow: "standard",
        stroke: "#555555",
        scale: 0.9
      }),

    $(go.Panel, "Auto",
      $(go.Shape,  // the label background, which becomes transparent around the edges
        {
          fill: $(go.Brush, "Radial", { 0: "rgb(215, 220, 223)", 0.3: "rgb(215, 220, 223)", 1: "rgba(215, 220, 223, 0)" }),
          stroke: null
        }),
      $(go.TextBlock,  // the label text
        {
          textAlign: "center",
          font: " 10pt helvetica, arial, sans-serif",
          stroke: "#777",
          margin: 4,
          visible: true
        },
        new go.Binding("text", "weight"),
        new go.Binding("visible", "visibleWeight"))
    )
  );


(async () => {
  // Create graph from json
  const data = await fetch("Graph/graph.json").then(response => response.json());
  const nodes = data.nodes;
  const edges = data.edges;

  const namespacesGroups = [];
  const fileNameGroups = [];
  const nodeDataArray = Object.keys(nodes).map(id => {
    const { id: key, name, namespace, srcInfo } = nodes[id];

    if (!namespacesGroups.some((group) => { return group.key === namespace })) {
      namespacesGroups.push({ key: namespace, name: namespace, isGroup: true, type: "namespace", fill: "rgba(213, 213, 144, 0.33)", visible: false });
    }
    if (!fileNameGroups.some((group) => { return group.key === srcInfo.fileName })) {
      fileNameGroups.push({ key: srcInfo.fileName, name: srcInfo.fileName, isGroup: true, type: "fileName", visible: false });
    }

    return { key, name, data: { namespace, fileName: srcInfo.fileName } };
  });

  nodeDataArray.splice(0, 0, ...namespacesGroups);
  nodeDataArray.splice(0, 0, ...fileNameGroups);

  const linkDataArray = edges.map(({ from, to, dependencies }) => {
    let weight = Object.values(dependencies).reduce((counter, curr) => counter + curr, 0);
    return { from, to, weight, data: { dependencies, weight } };
  });

  diagram.model = new go.GraphLinksModel(nodeDataArray, linkDataArray);
})();
