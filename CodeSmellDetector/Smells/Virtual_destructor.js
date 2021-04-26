const Util = require("../Utility.js");
const assert = require('assert');

module.exports = {
    callback: async function(ST, args){
        let msg, src, smell_level, report = [];
        // for(const structure_id in ST.structures){
        //     const structure = ST.structures[structure_id];
            
        // }
        let base_classes_ids = get_all_base_structures(ST);
        for(const base_class_id of base_classes_ids){
            let method_id = get_non_virtual_destructor(base_class_id, ST);
            if(method_id !== null){
                let base_class = ST.structures[base_class_id];
                let method = base_class.methods[method_id];
                msg = `Destructor of base class: "${base_class_id}" is not virtual.`;
                src = Util.get_src_obj(method.src_info.file, method.src_info.line, method.src_info.col, base_class_id, method_id);
                smell_level = Util.get_smell_lvl(1, 1, 1); // get max smell level.
                report.push(Util.get_incident_obj(src, msg, smell_level));
            }
                
        }
        console.log(base_classes);
        return report;
    }
}



function get_non_virtual_destructor(structure_id, ST){
    let structure = ST.structures[structure_id];
    assert(structure !== undefined);
    
    if(structure.methods === null) return false;

    let has_non_virtual_destructor = false;
    let destructor_id = "~" + structure_id;

    for(const method in structure.methods){
        let destructor_id_idx = method.indexOf(destructor_id)
        if(destructor_id_idx !== -1){
            if(structure.methods[method].virtual === false){
                return method;
            }
        }
    }
    return null;
}

/**
 * returns a list of the names of all base classes. 
 */
function get_all_base_structures(ST){
    base_classes = new Object();
    for(const dependency of ST.dependencies){
        if(dependency.types["Inherit"] !== undefined){
            base_classes[dependency.to] = true;
        }
    }
    return Object.keys(base_classes);
}