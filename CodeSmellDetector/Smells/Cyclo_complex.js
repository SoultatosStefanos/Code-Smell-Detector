const Util = require("../Utility.js");

module.exports = {
    callback: async function(ST, args){
        let msg, src, smell_level, report = [];
        for(const structure_id in ST.structures){
            const structure = ST.structures[structure_id];
            for(const method_id in structure.methods){
                const method = structure.methods[method_id];
                let branches = method.branches;
                let loops = method.loops;
                smell_level = Util.get_smell_lvl(args.max_branches.min, args.max_branches.max, branches);
                if(smell_level > 0){
                    msg = `Method: "${method_id}" has ${branches} branching statements.`;
                    src = Util.get_src_obj(method.src_info.file, method.src_info.line, method.src_info.col, structure_id, method_id);
                    report.push(Util.get_incident_obj(src, msg, smell_level));
                }
                smell_level = Util.get_smell_lvl(args.max_loops.min, args.max_loops.max, loops);
                if(smell_level > 0){
                    msg = `Method: "${method.constructor.name}" has ${loops} loop statements.`;
                    src = Util.get_src_obj(method.src_info.file, method.src_info.line, method.src_info.col, structure_id, method_id);
                    report.push(Util.get_incident_obj(src, msg, smell_level));
                }
            }
        }
        return report;
    }
}