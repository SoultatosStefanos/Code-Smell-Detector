const assert = require('assert');

module.exports = {
    /**
     * Returns id without namespaces and brackets
     */
     get_clean_identifier: (id) => {
        var namespace_ends = id.lastIndexOf(":");
        var func_starts = id.indexOf("(");
        if(func_starts == -1){
            func_starts = id.length;
        }
        return id.substring(namespace_ends+1, func_starts);
    },

    /**
     * Adds new incident to a report.
     */
    add_incident: (report, src_info, msg) => {
        report.push({
            src: src_info,
            msg: msg
        });
    },

    /**
     * Given the scale and level of a smell, returns the level of a smell on the scale 1-10.
     * Returns 0 if minimal_level is not reached.
     */
    normalize_smell_lvl: (minimal_level, maximal_level, actual_level) => {
        assert(maximal_level >= minimal_level, `maximal_level can never be larger than minimal_level.`);
        if(actual_level < minimal_level) return 0;
        if(actual_level >= maximal_level) return 10;

        var max = maximal_level - (minimal_level - 1);
        var reduced = actual_level - (minimal_level - 1);
       
        return Math.floor((reduced / max) * 10);
    }
}