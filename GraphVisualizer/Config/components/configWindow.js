import group from "./group.js"

export default Vue.component('config-window', {
    props: ['data', 'confighandler'],
    components: { group },
    template: `
    <div>
        <h2 id="configTitle">{{ data.title }}</h2>
        <div id="configData">
            <div class="group" v-for="group in data.groups">
                <group :groups="data.groups" :group="group" :confighandler="confighandler" /> 
            </div>
        </div>
    </div>`
});