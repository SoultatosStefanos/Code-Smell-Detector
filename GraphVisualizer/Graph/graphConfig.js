import config from "./Update/configValues.js"
import obs from "../Observer/observer.js"
import { diagram, contextMenu } from "./Appearance/graphAppearance.js"

// Install the handler for the "eventName" configuration
config.install = function (eventName, action) {
    obs.install(eventName, action);
}

// Uninstall the handler for the "eventName" configuration
config.unstall = function (eventName) {
    obs.unstall(eventName);
}

// Store a configuration Value
config.storeValue = function (name, value) {
    config[name] = value;
}

// Set a recoverer that we will call when it needed
config.setRecoverer = function (recoverer) {
    config.recover[recoverer.name] = recoverer;
}

// The callback gets as first arg the node, and as second arg the contextMenu button(if it is needed)
let nodesContextMenu = {};
nodesContextMenu.add = function (description, action) {
    contextMenu.nodeContextMenu.add(makeButton(description, (e, obj) => { action(obj.part.adornedPart, obj); }));
}

var graph = {
    go_diagram: diagram,
    nodesContextMenu, 
    getNode: function (key) {
        diagram.findNodeForKey(key);
    }
};

export default {
    config,
    graph
};

