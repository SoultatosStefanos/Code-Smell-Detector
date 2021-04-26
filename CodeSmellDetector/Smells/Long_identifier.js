const Util = require("../Utility.js");

module.exports = {
    callback: async function(ST, args){
        let id_len, msg, src, smell_level, report = [];
        for(const structure_id in ST.structures){
            const structure = ST.structures[structure_id];
            id_len = Util.get_clean_identifier(structure_id).length;
            smell_level = Util.get_smell_lvl(args.max_id_len.min, args.max_id_len.max, id_len);
            if(smell_level > 0){
                msg = `Structure: "${structure_id}" has an id of ${id_len} characters.`;
                src = Util.get_src_obj(structure.src_info.file, structure.src_info.line, structure.src_info.col, structure_id);
                report.push(Util.get_incident_obj(src, msg, smell_level));
            }
            for(const method_id in structure.methods){
                const method = structure.methods[method_id];
                id_len = Util.get_clean_identifier(method_id).length;
                smell_level = Util.get_smell_lvl(args.max_id_len.min, args.max_id_len.max, id_len);
                if(smell_level > 0){
                    msg = `Method: "${method_id}" has an id of ${id_len} characters.`;
                    src = Util.get_src_obj(method.src_info.file, method.src_info.line, method.src_info.col, structure_id, method_id);
                    report.push(Util.get_incident_obj(src, msg, smell_level));
                }
                for(const arg_id in method.args){
                    smell_level = Util.get_smell_lvl(args.max_id_len.min, args.max_id_len.max, arg_id.length);
                    if(smell_level > 0){
                        msg = `Argument: "${arg_id}" of "${method_id}" has an id of ${arg_id.length} characters.`;
                        src = Util.get_src_obj(method.src_info.file, method.src_info.line, method.src_info.col, structure_id, method_id);
                        report.push(Util.get_incident_obj(src, msg, smell_level));
                    }
                }
                for(const def_id in method.definitions){
                    smell_level = Util.get_smell_lvl(args.max_id_len.min, args.max_id_len.max, def_id.length);
                    if(smell_level > 0){
                        msg = `Definition: "${def_id}" in "${method_id}" has an id of ${def_id.length} characters.`;
                        src = Util.get_src_obj(method.src_info.file, method.src_info.line, method.src_info.col, structure_id, method_id);
                        report.push(Util.get_incident_obj(src, msg, smell_level));
                    }
                }
            }
            for(const field_id in structure.fields){
                id_len = Util.get_clean_identifier(field_id).length;
                smell_level = Util.get_smell_lvl(args.max_id_len.min, args.max_id_len.max, id_len);
                if(smell_level > 0){
                    msg = `Field: "${field_id}" of "${structure_id}" has an id of ${id_len} characters.`;
                    src = Util.get_src_obj(structure.src_info.file, structure.src_info.line, structure.src_info.col, structure_id);
                    report.push(Util.get_incident_obj(src, msg, smell_level));
                }
            }
        }
        return report;
    }
}

