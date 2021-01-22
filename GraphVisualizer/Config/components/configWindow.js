import group from "./group.js"

export default Vue.component('config-window', {
    props: ['data', 'confighandler'],
    components: { group },
    template: `
    <div>
        <h5 id="configTitle">{{ data.title }}</h5>
        <div id="configData">
            <div class="group" v-for="group in data.groups">
                <group :groups="data.groups" :group="group" :confighandler="confighandler" /> 
            </div>
        </div>
    </div>`
});