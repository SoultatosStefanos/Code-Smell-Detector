const Util = require("../Utility.js");

module.exports = {
    callback: async function(ST, args){
        let msg, src, smell_level, report = [];
        for(const structure_id in ST.structures){
            const structure = ST.structures[structure_id];
            let overloads = new Object(); // pair < method name - count >
            for(const method_id in structure.methods){
                let method_name = Util.get_clean_identifier(method_id);
                if(Util.is_standard_class_func(method_name, structure_id)) continue;

                if(overloads[method_name] === undefined) overloads[method_name] = 0;
                else overloads[method_name]++;  
            }
            for(const method_name in overloads){
                let overload_count = overloads[method_name];
                smell_level = Util.get_smell_lvl(args.max_overload.range, overload_count);
                if(smell_level > 0){ 
                    msg = `Method: "${method_name}" of structure "${structure_id}" is overloaded ${overload_count} times.`;
                    src = Util.get_src_obj(structure.src_info.file, structure.src_info.line, structure.src_info.col, structure_id);
                    report.push(Util.get_smell_obj(src, msg, smell_level));
                }
            }
        }
        return report;
    }
}