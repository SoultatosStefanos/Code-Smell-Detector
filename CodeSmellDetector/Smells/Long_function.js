var Util = require("../Utility.js");

module.exports = {
    callback: async function(ST, args){
        var report = [];
        for(const structure_id in ST.structures){
            const structure = ST.structures[structure_id];
            for(const method_id in structure.methods){
                const method = structure.methods[method_id];
                var statements = method.statements;
                if(statements > args.max_statements){
                    Util.add_incident(report, method.src_info, `Method: "${method_id}" has ${statements} statements.`);
                }
            }
        }
        return report;
    }
}