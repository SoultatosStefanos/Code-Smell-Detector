var Util = require("../Utility.js");

module.exports = {
    callback: function(ST, args){
        var report = [];
        for(const structure_id in ST.structures){
            const structure = ST.structures[structure_id];
            for(const method_id in structure.methods){
                const method = structure.methods[method_id];
                var argc = method.args.length;
                if(argc > args.max_argc){
                    Util.add_incident(report, method.src_info, `Method: "${method.constructor.name}" has ${argc} arguments.`);
                }
            }
        }
        return report;
    }
}