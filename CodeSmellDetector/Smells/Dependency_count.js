const Util = require("../Utility.js");

module.exports = {
    callback: async function(ST, args){
        let msg, src, smell_level, report = [];

        let dependencies_map = new Object();
        for(const dependency of ST.dependencies){
            if(dependencies_map[dependency.from] === undefined){
                dependencies_map[dependency.from] = [dependency.to];
            }
            else {
                dependencies_map[dependency.from].push(dependency.to);
            }
        }


        for(const dep_str in dependencies_map){
            const dep = dependencies_map[dep_str];
            smell_level = Util.get_smell_lvl(args.max_direct_dependencies.range, dep.length);
            if(smell_level > 0){
                let dep_names = "";
                for(let i=0; i<dep.length - 1; i++)
                    dep_names += dep[i] + ", ";
                dep_names += dep[dep.length - 1] + "\n";

                const structure = ST.structures[dep_str];
                msg = `Structure: "${dep_str}" directly depends upon ${dep.length} other classes. These are: ${dep_names}`;
                src = Util.get_src_obj(structure.src_info.file, structure.src_info.line, structure.src_info.col, dep_str);
                report.push(Util.get_smell_obj(src, msg, smell_level));
            }
        }

        return report;




        for(const structure_id in ST.structures){
            const structure = ST.structures[structure_id];
            for(const method_id in structure.methods){
                const method = structure.methods[method_id];
                let branches = method.branches;
                let loops = method.loops;
                smell_level = Util.get_smell_lvl(args.max_branches.min, args.max_branches.max, branches);
                if(smell_level > 0){
                    msg = `Method: "${method_id}" has ${branches} branching statements.`;
                    src = Util.get_src_obj(method.src_info.file, method.src_info.line, method.src_info.col, structure_id, method_id);
                    report.push(Util.get_smell_obj(src, msg, smell_level));
                }
                smell_level = Util.get_smell_lvl(args.max_loops.min, args.max_loops.max, loops);
                if(smell_level > 0){
                    msg = `Method: "${method.constructor.name}" has ${loops} loop statements.`;
                    src = Util.get_src_obj(method.src_info.file, method.src_info.line, method.src_info.col, structure_id, method_id);
                    report.push(Util.get_smell_obj(src, msg, smell_level));
                }
            }
        }
        return report;
    }
}