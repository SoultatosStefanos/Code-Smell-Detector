const Util = require("../Utility.js");
const fs = require("fs");
const readline = require("readline");

module.exports = {
    callback: async function(ST, args){
        let lines, msg, src, smell_level, report = [];
        for(const header of ST.headers){
            lines = await get_file_line_count(header);
            smell_level = Util.get_smell_lvl(args.max_header_lines.range, lines);
            if(smell_level > 0){
                msg = `Header file: "${header}" has ${lines} of code.`;
                src = Util.get_src_obj(header, 1, 1);
                report.push(Util.get_smell_obj(src, msg, smell_level));
            }
        }
        for(const source of ST.sources){
            lines = await get_file_line_count(source);
            smell_level = Util.get_smell_lvl(args.max_src_lines.range, lines);
            if(smell_level > 0){
                msg = `Source file: "${source}" has ${lines} of code.`;
                src = Util.get_src_obj(source, 1, 1);
                report.push(Util.get_smell_obj(src, msg, smell_level));
            }
        }
        return report;
    }
}


async function get_file_line_count(file_path){
    const file_stream = fs.createReadStream(file_path);

    const rl = readline.createInterface({
        input: file_stream,
        crlfDelay: Infinity
    });
    let line_counter = 0;

    for await(const line of rl){
        line_counter++;
    }

    return line_counter;
}
