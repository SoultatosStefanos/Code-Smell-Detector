const Util = require("../Utility.js");

module.exports = {
    callback: async function(ST, args){
        let msg, src, smell_level, report = [];
        for(const structure_id in ST.structures){
            const structure = ST.structures[structure_id];
            for(const method_id in structure.methods){
                const method = structure.methods[method_id];
                if(method.args === null)
                    continue;
                let argc = Object.keys(method.args).length;
                smell_level = Util.get_smell_lvl(args.max_argc.range, argc);
                if(smell_level > 0){
                    msg = `Method: "${method_id}" has ${argc} arguments.`;
                    src = Util.get_src_obj(method.src_info.file, method.src_info.line, method.src_info.col, structure_id, method_id);
                    report.push(Util.get_smell_obj(src, msg, smell_level));
                }
            }
        }
        return report;
    }
}