module.exports = {
    callback: function(ST, args){
        var report = [];
        for(const structure_id in ST.structures){
            const structure = ST.structures[structure_id];
            for(const method_id in structure.methods){
                const method = structure.methods[method_id];
                var argc = method.args.length;
                if(argc > args.max_argc){
                    var incident = {
                        src: {
                            file: method.src_info.file,
                            line: method.src_info.line,
                            col: method.src_info.column
                        },
                        msg: `Method: "${method.constructor.name}" has ${argc} arguments.`
                    }
                    report.push(incident);
                }
            }
        }
        return report;
    }
}