import { obs } from "../Observer/observer.js"
import config_window from "./components/configWindow.js"

(async () => {
    const json = await fetch("/Config/config.json").then(response => response.json());

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