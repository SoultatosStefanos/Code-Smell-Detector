const assert = require('assert');

module.exports = {

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
    }
}
