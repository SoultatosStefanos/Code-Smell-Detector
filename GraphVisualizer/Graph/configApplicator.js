import obs from "../Observer/observer.js"

let configApplicator = {};

// ----------------------------------------------------------------------------------

// initialized with my configurations
configApplicator.values = {
    weightFilterValue: 0,
    showWeightsFlag: true,
    groupEdgesFlag: false,
    viewOnlyDepType: 'None',
    louvainMultiLevels: 'twoLevels',
    infomapMultiLevels: '-d --two-level --silent',
    llpGamma: 0,
};

configApplicator.recover = {};

// ----------------------------------------------------------------------------------

configApplicator.callWithRecover = function(action, data) {
    action(data);
    if (configApplicator.recover[action.name]) {
        configApplicator.recover[action.name](this.values, data);
    }
}

// ----------------------------------------------------------------------------------

configApplicator.install = function (eventName, action, recoverAction) {
    configApplicator.recover[action.name] = recoverAction;
    obs.install(eventName, (data) => {
        action(data);
        if (recoverAction)
            recoverAction(this.values, data);
    });
}

configApplicator.uninstall = function (eventName) {
    obs.uninstall(eventName);
}

// ----------------------------------------------------------------------------------

// Store a configuration Value
configApplicator.storeValue = function(name, value) {
    this.values[name] = value;
}

configApplicator.getValue = function(name) {
    return this.values[name];
}

// ----------------------------------------------------------------------------------

export default configApplicator;