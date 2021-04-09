var Util = require("../Utility.js");

module.exports = {
    callback: function(ST, args){
        var report = [];
        var id_len;
        for(const structure_id in ST.structures){
            const structure = ST.structures[structure_id];
            id_len = Util.get_clean_identifier(structure_id).length;
            if(id_len > args.max_id_len){
                Util.add_incident(report, structure.src_info, `Structure: "${structure_id}" has an id of ${id_len} characters.`);
            }
            for(const method_id in structure.methods){
                const method = structure.methods[method_id];
                id_len = Util.get_clean_identifier(method_id).length;
                if(id_len > args.max_id_len){
                    Util.add_incident(report, structure.src_info, `Method: "${method_id}" has an id of ${id_len} characters.`);
                }
                for(const arg_id in method.args){
                    if(arg_id.length > args.max_id_len){
                        Util.add_incident(report, structure.src_info, `Argument: "${arg_id}" of "${method_id}" has an id of ${arg_id.length} characters.`);
                    }
                }
                for(const def_id in method.definitions){
                    if(def_id.length > args.max_id_len){
                        Util.add_incident(report, structure.src_info, `Definition: "${def_id}" of "${method_id}" has an id of ${def_id.length} characters.`);
                    }
                }
            }
            for(const field_id in structure.fields){
                id_len = Util.get_clean_identifier(field_id).length;
                if(id_len > args.max_id_len){
                    Util.add_incident(report, structure.src_info, `Field: "${field_id}" of "${structure_id}" has an id of ${id_len} characters.`);
                }
            }
        }
        return report;
    }
}

