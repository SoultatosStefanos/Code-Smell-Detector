import { diagram } from "../Appearance/graphAppearance.js"
import { PackedLayout } from "../../node_modules/gojs/extensionsJSM/PackedLayout.js"
import totalWeight from "../utilities/totalWeight.js"
import configApplicator from "../configApplicator.js"

function filterWeights(value = configApplicator.values.weightFilterValue) {
  diagram.model.commit(function (m) {
    m.linkDataArray.forEach((linkData) => {
      if (linkData.weight < value)
        m.set(linkData, 'visibleLink', false);
    });
  });
}

function weightFilter(value) {
  diagram.model.commit(function (m) {
    let type;
    if (configApplicator.values.groupEdgesFlag)
      type = 'groupEdge'
    else
      type = 'nodeEdge'
    m.linkDataArray.forEach((linkData) => {
      if (linkData.type === type)
        m.set(linkData, 'visibleLink', true);
    });

    filterWeights(value);

    // recover.weightFilter(value);
  });
}

function showWeights(value) {
  diagram.model.commit(function (m) {
    m.linkDataArray.forEach((linkData) => {
      m.set(linkData, 'visibleWeight', value);
    });
  }, 'showWeights');
  // recover.showWeights(value);
};

function highlight(depKind) {
  return function () {
    diagram.model.commit(function (m) {
      if (depKind === 'None') {
        m.linkDataArray.forEach((linkData) => { m.set(linkData, 'color', '#555555'); });
      }
      else {
        m.linkDataArray.forEach((linkData) => {
          if (linkData.data.dependencies[depKind] !== undefined)
            m.set(linkData, 'color', 'rgb(71, 128, 236)');
          else
            m.set(linkData, 'color', '#555555');
        });
      }
    }, 'highlight');
  }
}

function viewOnly(depKind) {
  return function () {
    diagram.model.commit(function (m) {
      if (depKind === 'None') {
        m.linkDataArray.forEach((linkData) => { m.set(linkData, 'visibleLink', true); });
      }
      else if (depKind === 'Nodes') {
        m.linkDataArray.forEach((linkData) => { m.set(linkData, 'visibleLink', false); });
      }
      else {
        m.linkDataArray.forEach((linkData) => {
          if (linkData.data.dependencies[depKind] !== undefined)
            m.set(linkData, 'visibleLink', true);
          else
            m.set(linkData, 'visibleLink', false);
        });
      }
    }, 'viewOnly');

    recover.viewOnly(configApplicator.values, depKind);
  }
}

function dependenciesConfig(data) {
  diagram.model.commit(function (m) {
    m.linkDataArray.forEach((linkData) => {
      m.set(linkData, 'weight', totalWeight(linkData.data.dependencies, data));
    });
  }, 'dependenciesConfig');
}

function outerLayout() {
  let $ = go.GraphObject.make;
  if (diagram.layout instanceof go.ForceDirectedLayout) {
    diagram.layout = $(PackedLayout, {
      packShape: PackedLayout.Rectangular,
      spacing: 50
    });
  } else {
    diagram.layout = $(go.ForceDirectedLayout);
  }
}

let recover = {
  'weightFilter': (configValues, value) => {
    configValues.weightFilterValue = value;
    if (!configValues.groupEdgesFlag)
      viewOnly(configValues.viewOnlyDepType);
  },
  'viewOnly': (configValues, depKind) => {
    if (depKind)
      configValues.viewOnlyDepType = depKind;
    filterWeights();
  },
  'showWeights': (configValues, value) => {
    configValues.showWeightsFlag = value;
  }
}


configApplicator.install('weightFilter', weightFilter, recover.weightFilter);
configApplicator.install('showWeights', showWeights, recover.showWeights);
configApplicator.install('highlightNone', highlight('None'));
configApplicator.install('highlightClassField', highlight('ClassField'));
configApplicator.install('highlightClassTemplateParent', highlight('ClassTemplateParent'));
configApplicator.install('highlightClassTemplateArg', highlight('ClassTemplateArg'));
configApplicator.install('highlightInherite', highlight('Inherite'));
configApplicator.install('highlightFriend', highlight('Friend'));
configApplicator.install('highlightNestedClass', highlight('NestedClass'));
configApplicator.install('highlightMethodArg', highlight('MethodArg'));
configApplicator.install('highlightMethodDefinition', highlight('MethodDefinition'));
configApplicator.install('highlightMethodReturn', highlight('MethodReturn'));
configApplicator.install('highlightMethodTemplateArg', highlight('MethodTemplateArg'));
configApplicator.install('highlightMemberExpr', highlight('MemberExpr'));
configApplicator.install('viewOnlyNone', viewOnly('None'));
configApplicator.install('viewOnlyClassField', viewOnly('ClassField'));
configApplicator.install('viewOnlyClassTemplateParent', viewOnly('ClassTemplateParent'));
configApplicator.install('viewOnlyClassTemplateArg', viewOnly('ClassTemplateArg'));
configApplicator.install('viewOnlyInherite', viewOnly('Inherite'));
configApplicator.install('viewOnlyFriend', viewOnly('Friend'));
configApplicator.install('viewOnlyNestedClass', viewOnly('NestedClass'));
configApplicator.install('viewOnlyMethodArg', viewOnly('MethodArg'));
configApplicator.install('viewOnlyMethodDefinition', viewOnly('MethodDefinition'));
configApplicator.install('viewOnlyMethodReturn', viewOnly('MethodReturn'));
configApplicator.install('viewOnlyMethodTemplateArg', viewOnly('MethodTemplateArg'));
configApplicator.install('viewOnlyMemberExpr', viewOnly('MemberExpr'));
configApplicator.install('viewOnlyNodes', viewOnly('Nodes'));

configApplicator.install('dependenciesConfig', dependenciesConfig);

configApplicator.install('outerLayoutForceDirected', outerLayout);
configApplicator.install('outerLayoutPacked', outerLayout);


export default {
  weightFilter,
  showWeights,
  highlight,
  viewOnly,
  filterWeights
}
