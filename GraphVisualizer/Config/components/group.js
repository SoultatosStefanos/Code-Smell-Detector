import { slider, checkbox, selector, number, applyButton } from "./components.js";
import UIManager from '../UIManager.js';

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
                        <number :data="data" :disabled="evalCondition(data.conditions.disabled)" :onChange="onChange"/>
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
        evalCondition: UIManager.evalCondition,
        onChange: UIManager.onChange,
        onChangeSelect: UIManager.onChangeSelect,
        onClick: UIManager.onClick,
        Collapsible: UIManager.collapseCard
    }
});