import obs from "../Observer/observer.js"

function ConfigApplicator() {
    let values = {};
    let recover = {}; 

    function callWithRecover(action, data){
        action(data);
        if(recover[action.name]){
            recover[action.name](this.values, data);
        }
    } 
    // Install/Uninstall the handler for the "eventName" configuration
    function install(eventName, action, recoverAction) {
        recover[action.name] = recoverAction;
        obs.install(eventName, (data) => {
            action(data);
            if (recoverAction)
                recoverAction(this.values, data);
        });
    }

    function uninstall(eventName) {
        obs.uninstall(eventName);
    }

    // Store a configuration Value
    function storeValue(name, value) {
        this.values[name] = value;
    }

    function getValue(name) {
        return this.values[name];
    }

    return {
        values,
        // Install/Uninstall the handler for the "eventName" configuration
        install,
        uninstall,
        storeValue, 
        getValue, 
        callWithRecover
    }
}

let configApplicator = new ConfigApplicator();

// set my configs
configApplicator.values = {
    weightFilterValue: 0,
    showWeightsFlag: true,
    groupEdgesFlag: false,
    viewOnlyDepType: 'None',
    louvainMultiLevels: 'twoLevels',
    infomapMultiLevels: '-d --two-level --silent',
    llpGamma: 0,
};
export default configApplicator;