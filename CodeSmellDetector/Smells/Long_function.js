const Util = require("../Utility.js");

module.exports = {
    callback: async function(ST, args){
        let msg, src, smell_level, report = [], statements_smell_lvl, lines_smell_lvl;
        for(const structure_id in ST.structures){
            const structure = ST.structures[structure_id];
            for(const method_id in structure.methods){
                const method = structure.methods[method_id];


                statements_smell_lvl = Util.get_smell_lvl(args.max_statements.range, method.statements);
                lines_smell_lvl = Util.get_smell_lvl(args.max_lines.range, method.lines);

                if(statements_smell_lvl === 0 && lines_smell_lvl === 0) // neither statements or lines smell
                    continue;
                else if(statements_smell_lvl > 0){
                    if(lines_smell_lvl === 0){  // only statements smell
                        msg = `Method: "${method_id}" has ${method.statements} statements.`;
                        smell_level = statements_smell_lvl;
                    }
                    else if(statements_smell_lvl > lines_smell_lvl){ // both smell but statements > lines
                        msg = `Method: "${method_id}" has ${method.statements} statements and ${method.lines} lines.`;
                        smell_level = statements_smell_lvl;
                    }
                    else { // both smell but lines > statements
                        msg = `Method: "${method_id}" has ${method.lines} lines and ${method.statements} statements.`;
                        smell_level = lines_smell_lvl;
                    }
                }
                else if(lines_smell_lvl > 0){ // only lines smell
                    msg = `Method: "${method_id}" has ${method.lines} lines.`;
                    smell_level = lines_smell_lvl;
                }
                src = Util.get_src_obj(method.src_info.file, method.src_info.line, method.src_info.col, structure_id, method_id);
                report.push(Util.get_incident_obj(src, msg, smell_level));
            }
        }
        return report;
    }
}