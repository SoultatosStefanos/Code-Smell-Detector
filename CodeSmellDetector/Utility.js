const assert = require('assert');

module.exports = {
    st_path: "D:/Thesis/ST.json",
    st_last_edit: null,
    smell_reports_save: "D:/Thesis/CodeSmellDetector/SmellReports.json",

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
            file: file,
            line: line,
            col: col,
            struct: struct,
            method: method
        };
    },

    /**
     *  @returns parameters as an object
     */
    get_incident_obj: (src, msg, lvl) => {
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
    get_smell_lvl: (minimal_level, maximal_level, actual_level) => {
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
    is_standard_class_func: (method_id, class_id) =>{
        if(method_id === class_id) return true;
        if(method_id === "~" + class_id) return true;
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
        const previous_smells = require(module.exports.smell_reports_save);
        if(previous_smells.computed_for.file === module.exports.st_path && previous_smells.computed_for.last_edit === module.exports.st_last_edit)
            return previous_smells.smells;
        return null;
    },

    save_smell_reports: async (smell_reports) => {
        let json = new Object();
        let computed_for = new Object();
        computed_for.file = module.exports.st_path;
        computed_for.last_edit = module.exports.st_last_edit;
        json.computed_for = computed_for;
        json.smells = smell_reports;
    
        json = JSON.stringify(json, null, 4);
        fs.writeFile(module.exports.smell_reports_save, json, "utf8", (error) => {
            if(error) throw error;
        });
    },

    get_file_name_from_path: (path) => {
        return path.replace(/^.*[\\\/]/, '');
    },

    get_countof_files: (ST) => {
        return ST.headers.length + ST.sources.length;
    },

    get_countof_structures: (ST) => {
        if(ST.structures === null) return 0;
        return Object.keys(ST.structures).length;
    },

    get_countof_methods: (ST) => {
        if(ST.structures === null) return 0;
        let countof_methods = 0;
        for(const structure_id in ST.structures){
            let structure = ST.structures[structure_id];
            if(structure.methods !== null)
                countof_methods += Object.keys(structure.methods).length;
        }
        return countof_methods;
    }
}
