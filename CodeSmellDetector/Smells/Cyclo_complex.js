module.exports = {
    callback: function(ST, args){
        var report = [];
        for(const structure of ST.structures){
            for(const method of structure.methods){
                var branches = method.branches;
                var loops = method.loops;
                if(branches > args.max_branches){
                    var incident = {
                        src: {
                            file: method.src_info.file,
                            line: method.src_info.line,
                            col: method.src_info.column
                        },
                        msg: `Method: "${method.id}" has ${branches} branching statements.`
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
                        msg: `Method: "${method.id}" has ${loops} loop statements.`
                    }
                    report.push(incident);
                }
            }
        }
        return report;
    }
}