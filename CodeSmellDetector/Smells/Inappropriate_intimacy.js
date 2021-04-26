const Util = require("../Utility.js");
const assert = require('assert');

module.exports = {
    callback: async function(ST, args){
        let msg, src, smell_level, report = [];

        for(const dependency of ST.dependencies){
            if(dependency.types.MemberExpr){
                smell_level = Util.get_smell_lvl(args.max_member_expr.min, args.max_member_expr.max, dependency.types.MemberExpr);
                if(smell_level > 0){
                    let from = ST.structures[dependency.from];
                    let to = ST.structures[dependency.to];
                    
                    msg = `Structure: "${dependency.from}" has ${dependency.types.MemberExpr} ` +
                    `member expressions regarding structure "${dependency.to}" at ` +
                    `${to.src_info.file}:${to.src_info.line}:${to.src_info.col}.`;
                    src = Util.get_src_obj(from.src_info.file, from.src_info.line, from.src_info.col);
                    report.push(Util.get_incident_obj(src, msg, smell_level));
                }
            }
        }
        
        return report;
    }
}