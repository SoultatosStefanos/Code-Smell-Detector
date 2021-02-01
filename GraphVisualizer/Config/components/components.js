import UIManager from '../UIManager.js';

export const slider = Vue.component('slider', {
    props: { 
        data: Object, 
        disabled: Boolean, 
        onChange: Function
    },
    template: 
    `<div>
        <label :for="data.id">{{data.description}}:</label>
        <div class="row">
            <div class="col-9">
                <input type="range" v-model="data.value" :min="data.min" :max="data.max" :id="data.id" :step="data.step" class="custom-range"
                    :disabled="disabled" @change="onChange(data.onChange.event, data.value)">
            </div>
            <div class="col-3">
                {{data.value}}
            </div>
        </div>
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
    <input type="checkbox" v-model="data.value" :id="data.id" :checked="data.value"
            :disabled="disabled" @change="onChange(data.onChange.event, data.value)">
    <label class="form-check-label" :for="data.id"> {{data.description}} </label>
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
        <label :for="data.id">{{data.description}}:</label>
        <select id="data.id" v-model="data.value"
        :disabled="disabled" @change="onChange(data.onChange.event, data.value)" class="form-control form-control-sm">
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
        <input type="number" v-model="data.value" :id="data.id" :min="data.min" :max="data.max" :step="data.step" style="width: 4rem;" 
            :disabled="disabled" @change="onChange(data.onChange.event, data.value)" >
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
        <button class="btn btn-primary" :disabled="disabled" @click="applyChanges(data.onClick.event, group.data, data.onGroupData)" style="width: 5rem;">
            {{data.description}}
        </button>
    </div>`, 
    methods: {
        applyChanges: UIManager.applyChanges,
    }
})
