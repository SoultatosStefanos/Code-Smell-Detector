(async () => {
    const json = await fetch("/Config/config.json").then(response => response.json());
    
    var config = new Vue({
        el: '#config',
        data: {
            title: json.title,
            groups: json.groups
        },
        methods: {
            evalCondition(condition) {
                return eval(condition);
            }
        }
    });
})();

