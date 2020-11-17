import obs from "../Observer/observer.js"
import config_window from "./components/configWindow.js"

(async () => {
    const json = await fetch("/Config/config.json").then(response => response.json());
    const schema = await await fetch("/Config/config_schema.json").then(response => response.json());
    var ajv = new Ajv();
    var valid = ajv.validate(schema, json);
    if (!valid)
        console.log(ajv.errors);
        else
        console.log("schema validation succeed!");
   
    var config = new Vue({
        el: '#config',
        data: {
            data: json,
            confighandler: obs
        },
        components: {
            config_window
        }
    });
})();