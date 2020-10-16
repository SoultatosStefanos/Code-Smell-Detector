import { slider, checkbox, selector, number, applyButton } from "./components.js"

export default Vue.component('group', {
    props: [
        'groups',
        'group',
        'confighandler'
    ],
    components: { slider, checkbox, selector, number, applyButton },
    template:
        `<div>
        <!-- div class="groupName" :id="group.id"> {{group.description}} </div -->

        <button class="groupName collapsible" :id="group.id" @click="Collapsible"> {{group.description}} </button>
        
        <div class="groupData content">
            <div v-for="data in group.data">
                <div class="groupElem"
                    :class="{disabledElem: evalCondition(data.conditions.disabled)}"
                    v-show="(data.conditions === undefined || data.conditions.displayed === undefined) ? true : evalCondition(data.conditions.displayed)">
                
                    <div v-if="data.type == 'slider'">
                    <slider :data="data" :disabled="evalCondition(data.conditions.disabled)" :onChange="onChange"/>
                    </div>

                    <div v-else-if="data.type == 'checkbox'">
                    <checkbox :data="data" :disabled="evalCondition(data.conditions.disabled)" :onChange="onChange"/>
                    </div>

                    <div v-else-if="data.type == 'select'">
                        <selector :data="data" :disabled="evalCondition(data.conditions.disabled)" :onChange="onChangeSelect"/>
                    </div>

                    <div v-else-if="data.type == 'number'">
                        <number :data="data" :disabled="evalCondition(data.conditions.disabled)" :onChange="onChangeSelect"/>
                    </div>

                    <div v-else-if="data.type == 'applyButton'">
                        <applyButton :data="data" :group="group" :disabled="evalCondition(data.conditions.disabled)" :onClick="onClick"/>
                    </div>

                    <div v-else-if="data.type == 'span'">
                    <span>{{data.description}}</span>
                    </div>

                    <div v-else-if="data.type == 'subgroup'">
                        <group :groups="groups" :group="data" :confighandler="confighandler" class="subgroup" /> 
                    </div>
                </div>
            </div>
        </div>
    </div>`,
    methods: {
        evalCondition(condition) {
            let $currGroup = this.group.data;
            return eval(' ' + condition);
        },
        onChange(event, data) {
            this.confighandler.fire(event, data);
        },
        onChangeSelect(event, data) {
            this.confighandler.fire(event + data);
        },
        onClick(event, groupData, applyOnGroupData) {
            let data = {};

            function dataCreation(element, data, groupData) {
                if (typeof element === 'object' && element !== null) {
                    let subData = {};
                    Object.keys(element).forEach(key => {
                        element[key].forEach(el => {
                            dataCreation(el, subData, groupData[key].data);
                            data[key] = subData;
                        })
                    });
                } else {
                    data[element] = groupData[element].value;
                }
            }
            applyOnGroupData.forEach(element => {
                dataCreation(element, data, groupData);
            });
            console.log(data);
            this.confighandler.fire(event, data)
        },
        Collapsible(e) {
            const button = e.target;
            button.classList.toggle("active");
            var content = button.nextElementSibling;
            if (content.style.maxHeight) {
                content.style.maxHeight = null;
            } else {
                content.style.maxHeight = 4*content.scrollHeight + "px";
            }

        }
    }
});