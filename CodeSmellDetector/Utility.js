const assert = require('assert');

module.exports = {

    execute_smell_callback: async (smell_callback, ST, args) => {
        let report = new Object();
        const startTime = Date.now();
        report.incidents = await smell_callback(ST, args);
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

        let max = maximal_level - (minimal_level - 1);
        let reduced = actual_level - (minimal_level - 1);
       
        return Math.floor((reduced / max) * 10);
    },


    /**
     * 
     * @returns true if a method is constructor / destructor / etc..
     */
    is_standard_class_func: (method_id, class_id) =>{
        if(method_id === class_id) return true;
        if(method_id === "~" + class_id) return true;
        return false;
    }


}

console.log(module.exports.get_clean_identifier("UnitTest3::LoadSpriteList(std::vector<std::string> &, const class AnimationFilm *, const std::string &, const std::string &)"));