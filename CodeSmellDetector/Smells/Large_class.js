var Util = require("../Utility.js");

module.exports = {
    callback: async function(ST, args){
        var report = [];
        var msg;
        var src;
        var smell_level;
        var field_count, method_count;
        for(const structure_id in ST.structures){
            const structure = ST.structures[structure_id];
            field_count = get_structure_field_count(structure, args.public_only);
            smell_level = Util.get_smell_lvl(args.max_fields.min, args.max_fields.max, field_count);
            if(smell_level > 0){
                if(args.public_only)
                    msg = `Structure: "${structure_id}" has ${field_count} public fields.`;
                else
                    msg = `Structure: "${structure_id}" has ${field_count} fields.`;
                src = Util.get_src_obj(structure.src_info.file, structure.src_info.line, structure.src_info.col, structure_id);
                report.push(Util.get_incident_obj(src, msg, smell_level));
            }

            method_count = get_structure_method_count(structure, args.public_only);
            smell_level = Util.get_smell_lvl(args.max_methods.min, args.max_methods.max, method_count);
            if(smell_level > 0){
                if(args.public_only)
                    msg = `Structure: "${structure_id}" has ${method_count} public methods.`;
                else
                    msg = `Structure: "${structure_id}" has ${method_count} methods.`;
                src = Util.get_src_obj(structure.src_info.file, structure.src_info.line, structure.src_info.col, structure_id);
                report.push(Util.get_incident_obj(src, msg, smell_level));
            }
        }
        return report;
    }
}

function get_structure_method_count(structure, public_only){
    if(!structure.methods) return 0;
    if(!public_only){
        return Object.keys(structure.methods).length;
    }
    var methods = Object.keys(structure.methods);
    var count = 0;
    for(const method of methods){
        if(method.access === "public"){
            count++;
        }
    }
    return count;
}

function get_structure_field_count(structure, public_only){
    if(!structure.fields) return 0;
    if(public_only === false){
        return Object.keys(structure.fields).length;
    }
    var fields = Object.keys(structure.fields);
    var count = 0;
    for(const field of fields){
        if(field.access === "public"){
            count++;
        }
    }
    return count;
}