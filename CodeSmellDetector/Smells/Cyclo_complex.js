var Util = require("../Utility.js");

module.exports = {
    callback: function(ST, args){
        var report = [];
        for(const structure_id in ST.structures){
            const structure = ST.structures[structure_id];
            for(const method_id in structure.methods){
                const method = structure.methods[method_id];
                var branches = method.branches;
                var loops = method.loops;
                if(branches > args.max_branches){
                    Util.add_incident(report, method.src_info, `Method: "${method_id}" has ${branches} branching statements.`);
                }
                if(loops > args.max_loops){
                    Util.add_incident(report, method.src_info, `Method: "${method.constructor.name}" has ${loops} loop statements.`);
                }
            }
        }
        return report;
    }
}