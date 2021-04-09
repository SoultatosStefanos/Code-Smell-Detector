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
    }

}
