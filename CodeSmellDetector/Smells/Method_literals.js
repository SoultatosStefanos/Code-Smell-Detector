var Util = require("../Utility.js");

module.exports = {
    callback: async function(ST, args){
        var report = [];
        for(const structure_id in ST.structures){
            const structure = ST.structures[structure_id];
            for(const method_id in structure.methods){
                const method = structure.methods[method_id];
                var literals = method.literals;
                if(literals > args.max_literals){
                    Util.add_incident(report, method.src_info, `Method: "${method.constructor.name}" has ${literals} literals.`);
                }
            }
        }
        return report;
    }
}