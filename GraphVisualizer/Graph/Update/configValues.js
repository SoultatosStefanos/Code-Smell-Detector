import update from "./updateGraph.js"
import grouping from "./grouping.js"

var config = {
    weightFilterValue: 0,
    showWeightsFlag: true,
    groupEdgesFlag: false,
    viewOnlyDepType: 'None',
    louvainMultiLevels: 'twoLevels', 
    infomapMultiLevels: '-d --two-level --silent',
    recover: {
        'groupEdges': (value) => {
            config.groupEdgesFlag = value;
            if (!config.groupEdgesFlag)
                update.viewOnly(config.viewOnlyDepType)();
            update.filterWeights(config.weightFilterValue);
        },
        'groupingBy': () => {
            grouping.groupEdges(config.groupEdgesFlag);
        },
        'groupingByNone': () => {
            const old_groupEdges_flag = config.groupEdgesFlag;
            grouping.groupEdges(false);
            config.groupEdgesFlag = old_groupEdges_flag;
        },
        'weightFilter': (value) => {
            config.weightFilterValue = value;
            if (!config.groupEdgesFlag)
                update.viewOnly(config.viewOnlyDepType)();
        },
        'viewOnly': (depKind) => {
            config.viewOnlyDepType = depKind;
            update.filterWeights();
        },
        'showWeights': (value) => {
            config.showWeightsFlag = value;
        }, 
        'louvainMultiLevels': (value) => {
            config.louvainMultiLevels = value;
        },
        'infomapMultiLevels': (value) => {
            config.infomapMultiLevels = value;
        }
    }
};

export default config;