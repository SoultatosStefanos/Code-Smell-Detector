(async () => {

const $ = go.GraphObject.make;
const diagram =
  $(go.Diagram, "diagramDiv",
    { "undoManager.isEnabled": true,
    layout: $(go.ForceDirectedLayout)}
  );

diagram.nodeTemplate =
  $(go.Node, "Auto",
    { locationSpot: go.Spot.Center, background: "lightblue" , visible : true}, new go.Binding("location", "loc", go.Point.parse),
    new go.Binding("visible", "visible"),
    $(go.Shape,
        "RoundedRectangle", { fill: "lightblue", stroke: "gray", strokeWidth: 2 }),
    $(go.TextBlock,
      "default", 
      { margin: 12, stroke: "gray", font: "bold 16px sans-serif" },
       new go.Binding("text", "name")), 

      {
        selectionChanged: function(part) {
            var shape = part.elt(0);
            shape.fill = part.isSelected ? "lightyellow" : "lightblue";
          }
      }
  );


diagram.linkTemplate =
  $(go.Link,
    { toShortLength: 6, toEndSegmentLength: 20 , curve: go.Link.Bezier}, 
    $(go.Shape,                                           // link
      { strokeWidth: 0.5 },
      new go.Binding("strokeWidth", "thick")),
    $(go.Shape,                                           // arrow
      { toArrow: "Triangle", stroke: null, scale: 1.5 }), 

  );

// Create graph from json 
const data = await fetch("Graph/graph.json").then(response => response.json());
const nodes = data.nodes;
const edges = data.edges;


const nodeDataArray = Object.keys(nodes).map(id => {                                                                                                                                               
  const {id : key, name} = nodes[id]; 
  let ar = ["Group_1", "Group_2", "Group_3"];
  let index = Math.round(Math.random()*2); 
  return {key, name}; //, group: ar[index]};                                 
});

nodeDataArray.push({key: "Group_1", name: "Group_1",  isGroup: true });
nodeDataArray.push({key: "Group_2", name: "Group_2",  isGroup: true });
nodeDataArray.push({key: "Group_3", name: "Group_3",  isGroup: true });


// nodeDataArray.push({key: "Group_4", name: "Group_4",  isGroup: true });
// nodeDataArray.push({key: "Group_5", name: "Group_5",  isGroup: true });
// nodeDataArray.push({key: "Group_6", name: "Group_6",  isGroup: true });

const linkDataArray = edges.map(({from, to, dependencies}) => {
  let totalDependencies = Object.values(dependencies).reduce((counter, curr) => counter + curr, 0); 
  return { from, to};//, thick: 0.8 + 0.2 * totalDependencies}
});

diagram.model = new go.GraphLinksModel(nodeDataArray, linkDataArray);
// go.Diagram.fromDiv(document.getElementById("diagramDiv"))

})();

// function flash2(name) {
//   // all model changes should happen in a transaction
//   diagram.model.commit(function(m) {
//     var data = m.nodeDataArray[0];  // get the first node data
//     m.set(data, "name", name);
//   }, "flash");
// }