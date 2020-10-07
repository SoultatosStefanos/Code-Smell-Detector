import {obs} from "../Observer/observer.js"

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
            }, 
            onChange(event, data) {
                obs.fire(event, data);
            }, 
            onChangeSelect(event, data) {
                obs.fire(event+data);
            }, 
            onClick(event, groupData, applyOnGroupData){
                let data = {}; 
                applyOnGroupData.forEach(element => {
                    data[element] = groupData[element].value; 
                }); 
               
                obs.fire(event, data)
            }            
        }, 
    });
})();

/*
            "friends": {
                    "id": "friends",
                    "description": "Friends",
                    "type": "select",
                    "value": "none",
                    "options": {
                        "none": {
                            "id": "none",
                            "description": "---",
                            "value": "none"
                        },
                        "ignored": {
                            "id": "ignored",
                            "description": "Totally Ignored",
                            "value": "ignored"
                        },
                        "weighted": {
                            "id": "weighted",
                            "description": "Apply Weight",
                            "value": "weighted"
                        },
                        "weight1": {
                            "id": "weight1",
                            "description": "compute it as 1 Dependency",
                            "value": "weight1"
                        }
                    },
                    "conditions": {},
                    "onChange": {}
                },
                "nestedClasses": {
                    "id": "nestedClasses",
                    "description": "Nested Classes",
                    "type": "select",
                    "value": "none",
                    "options": {
                        "none": {
                            "id": "none",
                            "description": "---",
                            "value": "none"
                        },
                        "ignored": {
                            "id": "ignored",
                            "description": "Totally Ignored",
                            "value": "ignored"
                        },
                        "weighted": {
                            "id": "weighted",
                            "description": "Apply Weight",
                            "value": "weighted"
                        },
                        "weight1": {
                            "id": "weight1",
                            "description": "compute it as 1 Dependency",
                            "value": "weight1"
                        }
                    },
                    "conditions": {},
                    "onChange": {}
                },
                "classFields": {
                    "id": "classFields",
                    "description": "Classes Fields",
                    "type": "select",
                    "value": "none",
                    "options": {
                        "none": {
                            "id": "none",
                            "description": "---",
                            "value": "none"
                        },
                        "ignored": {
                            "id": "ignored",
                            "description": "Totally Ignored",
                            "value": "ignored"
                        },
                        "weighted": {
                            "id": "weighted",
                            "description": "Apply Weight",
                            "value": "weighted"
                        },
                        "weight1": {
                            "id": "weight1",
                            "description": "compute it as 1 Dependency",
                            "value": "weight1"
                        }
                    },
                    "conditions": {},
                    "onChange": {}
                },
                "classTemplateParent": {
                    "id": "classTemplateParent",
                    "description": "Classes' Template Parent",
                    "type": "select",
                    "value": "none",
                    "options": {
                        "none": {
                            "id": "none",
                            "description": "---",
                            "value": "none"
                        },
                        "ignored": {
                            "id": "ignored",
                            "description": "Totally Ignored",
                            "value": "ignored"
                        },
                        "weighted": {
                            "id": "weighted",
                            "description": "Apply Weight",
                            "value": "weighted"
                        },
                        "weight1": {
                            "id": "weight1",
                            "description": "compute it as 1 Dependency",
                            "value": "weight1"
                        },
                        "conditions": {},
                        "onChange": {}
                    }
                },
                "classTemplateArg": {},
                "methodReturn": {},
                "methodArg": {},
                "methodDefinition": {},
                "methodTemplateArg": {},
                "memberExpr": {}
*/