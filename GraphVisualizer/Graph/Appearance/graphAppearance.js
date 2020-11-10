import contextMenu from "./contextMenu.js"
import { PackedLayout } from "../../node_modules/gojs/extensionsJSM/PackedLayout.js"

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
      // "SelectionMoved": function (e) { e.diagram.layout.invalidateLayout(); }
    }
  );


// Node
diagram.nodeTemplate =
  $(go.Node, "Auto",
    {
      locationSpot: go.Spot.Center, 
      background: "lightblue", 
      visible: true,
      layoutConditions: go.Part.LayoutStandard & ~go.Part.LayoutNodeSized
    },
    new go.Binding("location", "loc", go.Point.parse),
    new go.Binding("visible", "visible"),
    $(go.Shape,
      "RoundedRectangle",
      {
        // fill: "lightblue",
        // stroke: "gray",
        fill: "rgb(188, 229, 183)",
        stroke: "rgb(56, 160, 85)",
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
        shape.stroke = part.isSelected ? "rgb(244, 128, 116)" : "rgb(56, 160, 85)";
      },
      toolTip:
        $("ToolTip",
          $(go.TextBlock, {
            margin: 3,
            stroke: "gray",
            font: "12px sans-serif"
          },
            new go.Binding("text", "", contextMenu.nodeProperties))
        ),
      contextMenu: contextMenu.nodeContextMenu
    }
  );


// Group
diagram.groupTemplate = $(go.Group, "Vertical", { layoutConditions: go.Part.LayoutStandard & ~go.Part.LayoutNodeSized },
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
  new go.Binding("visible", "visible"),
  {
    layout:
      $(PackedLayout,
        {
          packShape: PackedLayout.Rectangular,
          // packMode: PackedLayout.ExpandToFit,
          spacing: 15,
          aspectRatio: 2,
          isInitial: false,
          isOngoing: false
        })
  }
);


// Link
diagram.linkTemplate =
  $(go.Link,
    { toShortLength: 6, toEndSegmentLength: 20, curve: go.Link.Bezier },
    new go.Binding("visible", "visibleLink"),
    $(go.Shape,                                           // link
      {
        strokeWidth: 1,
        stroke: "#555555",
        name: "LINK"
      },
      new go.Binding("strokeWidth", "thick"),
      new go.Binding("stroke", "color").makeTwoWay()),
    $(go.Shape,                                           // arrowhead
      {
        toArrow: "standard",
        stroke: "#555555",
        fill: "#555555",
        scale: 0.9,
      },
      new go.Binding("stroke", "color").makeTwoWay(),
      new go.Binding("fill", "color").makeTwoWay()),

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
        new go.Binding("visible", "visibleWeight"),
        new go.Binding("stroke", "color"))
    ),
    {
      toolTip:
        $("ToolTip",
          $(go.TextBlock, {
            margin: 3,
            stroke: "gray",
            font: "12px sans-serif"
          },
            new go.Binding("text", "", contextMenu.linkProperties))
        )
    }
  );

