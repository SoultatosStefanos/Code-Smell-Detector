// A general utility object
const assert = require('assert');
const MainCfg = require('./MainConfig.json');

module.exports = {
    st_path: MainCfg.ST_Path,
    st_last_edit: null,
    smell_reports_save: MainCfg.SmellsCache_Path,

    /**
     * @argument relative_path: relative path to Main.js || Main.js behaves as if under "./code-smell-detector-gui/src"
     * @returns full path
    */
    get_full_path: (relative_path) => {
        let full_path = __dirname + "\\" + relative_path;
        full_path = path.normalize(full_path);
        return full_path.replace(/\\/g, "/");
    },

    uniform_path: (path) => {
        return path.replace(/\\/g, "/");
    },

    execute_smell_callback: async (smell_detector, ST) => {
        let report = new Object();
        const startTime = Date.now();
        report.detector = smell_detector.name;
        report.incidents = await smell_detector.callback(ST, smell_detector.args);
        for(let incident of report.incidents){
            incident.detector = smell_detector.name;
        }

        report.time = (Date.now() - startTime) / 1000;
        return report;
    },


    /**
     * Returns id without namespaces and brackets
     */
     get_clean_identifier: (id) => {
        let func_starts = id.indexOf("(");
        if(func_starts !== -1)
            id = id.slice(0, func_starts);
        let namespace_ends = id.lastIndexOf(":");
        return id.substring(namespace_ends+1, id.length);
    },

    /**
     * @returns parameters as an object
     */
     get_src_obj: (file, line, col = 0, struct = null, method = null) => {
        return {
            file: module.exports.uniform_path(file),
            line: line,
            col: col,
            struct: struct,
            method: method
        };
    },

    /**
     *  @returns parameters as an object
     */
    get_smell_obj: (src, msg, lvl) => {
       return {
           msg: msg,
           src: src,
           lvl: lvl
       };
    },

    /**
     * Given the scale and level of a smell, returns the level of a smell on the scale 1-10.
     * Returns 0 if minimal_level is not reached.
     */
    get_smell_lvl: ([minimal_level, maximal_level], actual_level) => {
        assert(maximal_level >= minimal_level, `maximal_level can never be larger than minimal_level.`);
        if(actual_level < minimal_level) return 0;
        if(actual_level >= maximal_level) return 10;

        let range = maximal_level - minimal_level;
        let reduced = actual_level - minimal_level;
        return Math.round((reduced / range) * 1000) / 100;

        // let max = maximal_level - (minimal_level - 1);
        // let reduced = actual_level - (minimal_level - 1);
       
        // return Math.floor((reduced / max) * 10);
    },


    /**
     * @argument method_id: clean id.
     * @returns true if a method is constructor / destructor / etc..
     */
    is_standard_class_func: (method_id, structure_id) =>{
        let clean_structures_id = module.exports.get_clean_identifier(structure_id);
        if(method_id === clean_structures_id) return true;
        if(method_id === "~" + clean_structures_id) return true;
        if(method_id.startsWith("operator")) return true;
        return false;
    },

    /**
     * @returns a list of the names of all base classes. 
     */
    get_all_base_structures: (ST) => {
        base_classes = new Object();
        for(const dependency of ST.dependencies){
            if(dependency.types["Inherit"] !== undefined){
                base_classes[dependency.to] = true;
            }
        }
        return Object.keys(base_classes);
    },


    /**
     * @returns previous smell reports if the st matches the one that was used last time. Otherwise null
     */
    get_smells_from_cache: () => {
        try {
            const previous_smells = require(module.exports.get_full_path(module.exports.smell_reports_save));
            if(previous_smells.computed_for.file === module.exports.get_full_path(module.exports.st_path) && previous_smells.computed_for.last_edit === module.exports.st_last_edit)
                return previous_smells.smells;
            else{
                console.log("Smells cache does not match current symbol table version. Smell computation needed.")
                document.getElementById("cfg_changed_label").innerHTML = "Smells cache does not match current symbol table version. Smell computation needed.";
            }
                

        } catch (error) {
            console.log("Smells cache not found. Smell computation needed.");
            document.getElementById("cfg_changed_label").innerHTML = "Smells cache not found. Smell computation needed.";
        }

        return null;
    },

    save_smell_reports: async (smell_reports) => {
        let json = new Object();
        let computed_for = new Object();
        computed_for.file = module.exports.get_full_path(module.exports.st_path);
        computed_for.last_edit = module.exports.st_last_edit;
        json.computed_for = computed_for;
        json.smells = smell_reports;
    
        json = JSON.stringify(json, null, 4);
        fs.writeFile(get_full_path(module.exports.smell_reports_save), json, "utf8", (error) => {
            if(error) throw error;
        });
    },

    get_file_name_from_path: (path) => {
        return path.replace(/^.*[\\\/]/, '');
    },

    get_countof_files: (ST) => {
        let file_count = 0;
        if(ST.headers)
            file_count += ST.headers.length;
        if(ST.sources)
            file_count += ST.sources.length; 
        return file_count;
    },

    get_countof_structures: (ST) => {
        if(!ST.structures) return 0;
        return Object.keys(ST.structures).length;
    },

    get_countof_methods: (ST) => {
        if(!ST.structures) return 0;
        let countof_methods = 0;
        for(const structure_id in ST.structures){
            let structure = ST.structures[structure_id];
            if(structure.methods)
                countof_methods += Object.keys(structure.methods).length;
        }
        return countof_methods;
    },

    // call function fn with delay = ms
    // delay: (fn, ms) => {
    //     let timer = 0;
    //     return (...args) => {
    //         clearTimeout(timer);
    //         timer = setTimeout(fn.bind(this, ...args), ms || 0);
    //     }
    // },

    // call function fn with delay = ms
    // if called with the same label before the label specific timer runs out, the timer resets
    delay_method: (label,callback,time) => {
        if(typeof window.delayed_methods=="undefined")
            window.delayed_methods={};
        delayed_methods[label]=Date.now();
        let t = delayed_methods[label];
        setTimeout(function(){ 
            if(delayed_methods[label]!=t)
                return;
            else{  
                delayed_methods[label]="";
                callback();
            }
        }, time||500);
      }


}