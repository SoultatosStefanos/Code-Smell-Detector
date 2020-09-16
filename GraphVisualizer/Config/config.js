(async () => {
    const json = await fetch("/Config/config.json").then(response => response.json());

    var config = new Vue({
        el: '#config',
        data: {
            title: json.title,
            groups: json.groups
        },
        methods: {
            conditionCheck(obj) {
                if (eval(obj.condition)) {
                    console.log("condition: true");
                    // den to briskei giati den exei oristei akoma 
                    // eval("document.getElementById(\""+ obj.bindTo + "\")."+ obj.result + " = " +  obj.resultValue); 
                } else {
                    console.log("condition: false");
                }
            }
        }
    });
})();

