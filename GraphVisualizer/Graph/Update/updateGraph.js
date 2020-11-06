import { diagram } from "../Appearance/graphAppearance.js"
import { obs } from "../../Observer/observer.js"
import totalWeight from "../utilities/totalWeight.js"

function showAllEdges(value) {
  if (value) {
    diagram.model.commit(function (m) {
      m.linkDataArray.forEach((linkData) => {
        m.set(linkData, 'visibleLink', true);
      });
    });
  }
};

function weightFilter(value) {
  console.log('weightFilter');
  diagram.model.commit(function (m) {
    m.linkDataArray.forEach((linkData) => {
      if (linkData.weight < value) {
        m.set(linkData, 'visibleLink', false);
      }
      else {
        m.set(linkData, 'visibleLink', true);
      }
    });
  }, 'weightFilter');
}

function showWeights(value) {
  diagram.model.commit(function (m) {
    m.linkDataArray.forEach((linkData) => {
      m.set(linkData, 'visibleWeight', value);
    });
  }, 'showWeights');

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
            m.set(linkData, 'color', 'OrangeRed');
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
      else {
        m.linkDataArray.forEach((linkData) => {
          if (linkData.data.dependencies[depKind] !== undefined)
            m.set(linkData, 'visibleLink', true);
          else
            m.set(linkData, 'visibleLink', false);
        });
      }
    }, 'highlight');
  }
}

function dependenciesConfig(data) {
  diagram.model.commit(function (m) {
      m.linkDataArray.forEach((linkData) => {
          m.set(linkData, 'weight', totalWeight(linkData.data.dependencies, data));
      });
  }, 'dependenciesConfig');
}

obs.install('allEdges', showAllEdges);
obs.install('weightFilter', weightFilter);
obs.install('showWeights', showWeights);
obs.install('highlightNone', highlight('None'));
obs.install('highlightClassField', highlight('ClassField'));
obs.install('highlightClassTemplateParent', highlight('ClassTemplateParent'));
obs.install('highlightClassTemplateArg', highlight('ClassTemplateArg'));
obs.install('highlightInherite', highlight('Inherite'));
obs.install('highlightFriend', highlight('Friend'));
obs.install('highlightNestedClass', highlight('NestedClass'));
obs.install('highlightMethodArg', highlight('MethodArg'));
obs.install('highlightMethodDefinition', highlight('MethodDefinition'));
obs.install('highlightMethodReturn', highlight('MethodReturn'));
obs.install('highlightMethodTemplateArg', highlight('MethodTemplateArg'));
obs.install('highlightMemberExpr', highlight('MemberExpr'));
obs.install('viewOnlyNone', viewOnly('None'));
obs.install('viewOnlyClassField', viewOnly('ClassField'));
obs.install('viewOnlyClassTemplateParent', viewOnly('ClassTemplateParent'));
obs.install('viewOnlyClassTemplateArg', viewOnly('ClassTemplateArg'));
obs.install('viewOnlyInherite', viewOnly('Inherite'));
obs.install('viewOnlyFriend', viewOnly('Friend'));
obs.install('viewOnlyNestedClass', viewOnly('NestedClass'));
obs.install('viewOnlyMethodArg', viewOnly('MethodArg'));
obs.install('viewOnlyMethodDefinition', viewOnly('MethodDefinition'));
obs.install('viewOnlyMethodReturn', viewOnly('MethodReturn'));
obs.install('viewOnlyMethodTemplateArg', viewOnly('MethodTemplateArg'));
obs.install('viewOnlyMemberExpr', viewOnly('MemberExpr'));


obs.install('dependenciesConfig', dependenciesConfig); 
