import grouping from "./grouping.js"
import update from "./updateGraph.js"
import configApplicator from "../configApplicator.js"

let recover = {
    'groupEdges': (configValues, value) => {
        configValues.groupEdgesFlag = value;
        if (!configValues.groupEdgesFlag)
        configApplicator.callWithRecover(update.viewOnly(configValues.viewOnlyDepType));
        update.filterWeights(configValues.weightFilterValue);
    },
    'groupingBy': (configValues) => {
        console.log("groupingBy");
        configApplicator.callWithRecover(grouping.groupEdges, configValues.groupEdgesFlag);
    },
    'groupingByNone': (configValues) => {
        const old_groupEdges_flag = configValues.groupEdgesFlag;
        configApplicator.callWithRecover(grouping.groupEdges,false);
        configValues.groupEdgesFlag = old_groupEdges_flag;
    },
    'louvainMultiLevels': (configValues, multi) => {
        let value = multi ? 'multiLevels' : 'twoLevels';
        configValues.louvainMultiLevels = value;
    },
    'infomapMultiLevels': (configValues, multi) => {
        let value = multi ? '-d --silent' : '-d --two-level --silent';
        configValues.infomapMultiLevels = value;
    },
    'llpGamma': (configValues, gamma) => {
        configValues.llpGamma = gamma;
    }
}

export default recover;