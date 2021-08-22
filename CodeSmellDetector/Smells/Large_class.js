const Util = require("../Utility.js");

module.exports = {
    callback: async function(ST, args){
        let field_count, method_count, msg, src, smell_level, report = [], field_smell_lvl, method_smell_lvl; 
        for(const structure_id in ST.structures){
            const structure = ST.structures[structure_id];

            field_count = get_structure_field_count(structure, args.public_only.val);
            method_count = get_structure_method_count(structure, args.public_only.val);
            field_smell_lvl = Util.get_smell_lvl(args.max_fields.range, field_count);
            method_smell_lvl = Util.get_smell_lvl(args.max_methods.range, method_count);

            if(field_smell_lvl === 0 && method_smell_lvl === 0) // neither fields or methods smell
                continue;
            else if(field_smell_lvl > 0){
                if(method_smell_lvl === 0){ // only fields smell
                    if(args.public_only.val === true)
                        msg = `Structure: "${structure_id}" has ${field_count} public fields.`;
                    else
                        msg = `Structure: "${structure_id}" has ${field_count} fields.`;
                    smell_level = field_smell_lvl;
                }
                else if(field_smell_lvl > method_smell_lvl){ // both smell but fields > methods
                    if(args.public_only.val === true)
                        msg = `Structure: "${structure_id}" has ${field_count} public fields and ${method_count} public methods.`;
                    else
                        msg = `Structure: "${structure_id}" has ${field_count} fields and ${method_count} methods.`;
                    smell_level = field_smell_lvl;
                }
                else { // both smell but methods > fields
                    if(args.public_only.val === true)
                        msg = `Structure: "${structure_id}" has ${method_count} public methods and ${field_count} public fields.`;
                    else
                        msg = `Structure: "${structure_id}" has ${method_count} methods and ${field_count} fields.`; 
                    smell_level = method_smell_lvl;
                }
            }
            else if(method_smell_lvl > 0){ // only methods smell
                if(args.public_only.val === true)
                    msg = `Structure: "${structure_id}" has ${method_count} public methods.`;
                else
                    msg = `Structure: "${structure_id}" has ${method_count} methods.`;
                smell_level = method_smell_lvl;
            }

            src = Util.get_src_obj(structure.src_info.file, structure.src_info.line, structure.src_info.col, structure_id);
            report.push(Util.get_smell_obj(src, msg, smell_level));
        }
        return report;
    }
}

function get_structure_method_count(structure, public_only){
    if(!structure.methods) return 0;
    if(!public_only)
        return Object.keys(structure.methods).length;
    let methods = structure.methods;
    let count = 0;
    for(const method_id in methods){
        const method = methods[method_id];
        if(method.access === "public"){
            count++;
        }
    }
    return count;
}

function get_structure_field_count(structure, public_only){
    if(!structure.fields) return 0;
    if(public_only === false)
        return Object.keys(structure.fields).length;
    let fields = structure.fields;
    let count = 0;
    for(const field_id in fields){
        const field = fields[field_id];
        if(field.access === "public"){
            count++;
        }
    }
    return count;
}