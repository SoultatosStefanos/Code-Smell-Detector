const Util = require("../Utility.js");
const assert = require('assert');

module.exports = {
    callback: async function(ST, args){
        let msg, src, smell_level, report = [];
        // for(const structure_id in ST.structures){
        //     const structure = ST.structures[structure_id];
            
        // }
        let base_classes_ids = Util.get_all_base_structures(ST);

        for(const structure_id in ST.structures){
            // if structure is not base
            if(base_classes_ids.indexOf(structure_id) > -1) continue;
            

            const structure = ST.structures[structure_id];
            let protected_members = 0;
            for(const method_id in structure.methods){
                const method = structure.methods[method_id];
                if(method.access === "protected") 
                    protected_members++;
            }
            for(const field_id in structure.fields){
                const field = structure.fields[field_id];
                if(field.access === "protected")
                    protected_members++;
            }


            smell_level = Util.get_smell_lvl(args.max_unneeded_protected.min, args.max_unneeded_protected.max, protected_members);
            if(smell_level > 0){
                msg = `Class: "${structure_id}" is not inherited from and has ${protected_members} protected variables or methods.`;
                src = Util.get_src_obj(structure.src_info.file, structure.src_info.line, structure.src_info.column, structure_id);
                report.push(Util.get_incident_obj(src, msg, smell_level));
            }
        }
        return report;
    }
}
