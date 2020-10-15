export const slider = Vue.component('slider', {
    props: { 
        data: Object, 
        disabled: Boolean, 
        onChange: Function
    },
    template: 
    `<div>
        {{data.description}}:
        <input type="range" v-model="data.value" :min="data.min" :max="data.max" :id="data.id" :step="data.step" class="slider"
         :disabled="disabled" @change="onChange(data.onChange.event, data.value)">
            {{data.value}}
    </div>`
})


export const checkbox = Vue.component('checkbox', {
    props: {
        data: Object, 
        disabled: Boolean, 
        onChange: Function
    }, 
    template:
    `<div>
    {{data.description}}: 
    <input type="checkbox" v-model="data.value" :id="data.id" :checked="data.value"
         :disabled="disabled" @change="onChange(data.onChange.event, data.value)">
    </div>`, 
})

export const selector = Vue.component('selector', {
    props: {
        data: Object, 
        disabled: Boolean, 
        onChange: Function
    }, 
    template:
    `<div>
        {{data.description}}:
        <select id="data.id" v-model="data.value"
        :disabled="disabled" @change="onChange(data.onChange.event, data.value)">
            <option v-for="option in data.options" :value="option.value">{{option.description}}</option>
        </select>
    </div>`
})

export const number = Vue.component('number', {
    props: {
        data: Object, 
        disabled: Boolean, 
        onChange: Function
    }, 
    template:
    `<div>
        {{data.description}}:
        <input type="number" v-model="data.value" :id="data.id" :min="data.min" :max="data.max" :step="data.step" style="width: 3vw" 
            :disabled="disabled" @change="onChange(data.onChange.event, data.value)"">
    </div>`
})

export const applyButton = Vue.component('applyButton', {
    props: {
        data: Object, 
        disabled: Boolean, 
        onClick: Function, 
        group: Object
    }, 
    template:
    `<div>
        <button class="applyButton" :disabled="disabled" @click="onClick(data.onClick.event, group.data, data.onGroupData)">
            {{data.description}}
        </button>
    </div>`
})
