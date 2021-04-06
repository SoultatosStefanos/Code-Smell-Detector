module.exports = {
    callback: function(ST, args){
        var report = [];
        for(const structure of ST.structures){
            for(const method of structure.methods){
                var literals = method.literals;
                if(literals > args.max_literals){
                    var incident = {
                        src: {
                            file: method.src_info.file,
                            line: method.src_info.line,
                            col: method.src_info.column
                        },
                        msg: `Method: "${method.id}" has ${literals} literals.`
                    }
                    report.push(incident);
                }
            }
        }
        return report;
    }
}