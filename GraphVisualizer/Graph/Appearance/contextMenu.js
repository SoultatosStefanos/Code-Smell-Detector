
const $ = go.GraphObject.make;

function makeButton(text, action) {
  return $("ContextMenuButton",
    $(go.TextBlock, "default",
      {
        margin: 3,
        stroke: "rgb(63, 62, 62)",
        font: "12px sans-serif",
        alignment: go.Spot.Left,
        text: text
      }, new go.Binding("text", "text")),
    { click: action });
}

const nodeContextMenu =
  $("ContextMenu",
    makeButton("Go to Source Line",
    function (e, obj) {  // OBJ is this Button
      var contextmenu = obj.part;  // the Button is in the context menu Adornment
      var part = contextmenu.adornedPart;  // the adornedPart is the Part that the context menu adorns
      // now can do something with PART, or with its data, or with the Adornment (the context menu)
      window.location.href = 'vscode://file/' + part.data.data.srcInfo.fileName + ':' + part.data.data.srcInfo.line;
    }),
    makeButton('Highlight Edges', (e, obj) => {
      let part = obj.part.adornedPart;
      let links = part.findLinksConnected();
      while (links.next()) {
        if (links.value.findObject("LINK").stroke !== '#ff2e9e')
          links.value.findObject("LINK").stroke = '#ff2e9e';
        else
          links.value.findObject("LINK").stroke = '#555555';
      }
    }),
    makeButton('Properties', (e, obj) => { alert(nodeProperties(obj.part.adornedPart.data)); })
  )

function nodeProperties(d) {
  function dependencyQuantityToText(description, d) {
    return (d ? ('\n' + description + ': ' + (Object.keys(d)).length) : '');
  }

  let content = 'Structure Type: ' + d.data.structureType + '\n';
  content += dependencyQuantityToText('Fields', d.data.fields);
  content += dependencyQuantityToText('Bases', d.data.bases);
  content += dependencyQuantityToText('Friends', d.data.friends);
  content += dependencyQuantityToText('Methods', d.data.methods);
  content += dependencyQuantityToText('templateArguments', d.data.templateArguments);
  content += (d.data.nestedParent ? ('\n' + 'Has Outer Parent') : '');

  return content;
}

function linkProperties(d) {
  let data = "";
  const description = {
    "ClassField": "Class Fields",
    "ClassTemplateParent": "Class Template Parent",
    "ClassTemplateArg": "Class Template Arguments",
    "Inherite": "Class Base",
    "NestedClass": "Outer Class",
    "Friend": "Class Friends",
    "MethodArg": "Method Arguments",
    "MethodDefinition": "Method Local Definitions",
    "MethodReturn": "Method Return Types",
    "MethodTemplateArg": "Method Template Arguments",
    "MemberExpr": "Method Member Access"
  };
  Object.keys(d.data.dependencies).forEach(depKind => { data += description[depKind] + ": " + d.data.dependencies[depKind] + "\n" });
  return data.slice(0, -1);
}

export default {
  nodeContextMenu,
  nodeProperties,
  linkProperties
}