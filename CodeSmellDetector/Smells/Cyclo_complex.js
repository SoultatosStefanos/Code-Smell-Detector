module.exports = {
    callback: function(ST, args){
        var report = [];
        for(const structure_id in ST.structures){
            const structure = ST.structures[structure_id];
            for(const method_id in structure.methods){
                const method = structure.methods[method_id];
                var branches = method.branches;
                var loops = method.loops;
                if(branches > args.max_branches){
                    var incident = {
                        src: {
                            file: method.src_info.file,
                            line: method.src_info.line,
                            col: method.src_info.column
                        },
                        msg: `Method: "${method_id}" has ${branches} branching statements.`
                    }
                    report.push(incident);
                }
                if(loops > args.max_loops){
                    var incident = {
                        src: {
                            file: method.src_info.file,
                            line: method.src_info.line,
                            col: method.src_info.column
                        },
                        msg: `Method: "${method.constructor.name}" has ${loops} loop statements.`
                    }
                    report.push(incident);
                }
            }
        }
        return report;
    }
}