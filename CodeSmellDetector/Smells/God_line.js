const Util = require("../Utility.js");
const fs = require("fs");
const readline = require("readline");

module.exports = {
    callback: async function(ST, args){
        let incidents, report = [];
        for(const header of ST.headers){
            incidents = await iterate_file(header, args.max_line_len);
            for(const incident of incidents){
                report.push(incident);
            }
        }
        for(const source of ST.sources){
            incidents = await iterate_file(source, args.max_line_len);
            for(const incident of incidents){
                report.push(incident);
            }
        }
        return report;
    }
}

/* Returns the length of a string (multi-line comments removed) and if comment is left open */
function un_commented_strlen(str, comment_open){
    if(str.len === 0){
        return {len:0, comment_open:comment_open};
    }


    let idx = 0;
    let idx2;
    let len = 0;
    if(comment_open){
        idx = str.indexOf("*/");
        if(idx === -1) return {len:0, comment_open:true};
        str = str.slice(idx+2);
        idx=0;
        comment_open = false;
    }
    
    idx = str.indexOf("/*");
    if(idx === -1) return {len:str.length, comment_open: false};
    
    let ret = un_commented_strlen(str.slice(idx + 2), true);
    ret.len += idx;
    return ret;

}

async function iterate_file(file_path, max_line_len){
    let msg, src, smell_level, file_report = [];
    const file_stream = fs.createReadStream(file_path);

    const rl = readline.createInterface({
        input: file_stream,
        crlfDelay: Infinity
    });
    let line_counter = 0;
    let in_comment = false;
    let info;
    for await(const line of rl){
        line_counter++;
        let line_comm_idx = line.indexOf("//");
        if(line_comm_idx !== -1){
            info = un_commented_strlen(line.slice(0, line_comm_idx));
        }
        else{
            info = un_commented_strlen(line);
        }
        in_comment = info.comment_open;
        smell_level = Util.get_smell_lvl(max_line_len.range, info.len);
        if(smell_level > 0){
            msg = `Line ${line_counter} of ${file_path} has an un-commented length of ${info.len} characters`;
            src = Util.get_src_obj(file_path, line_counter);
            file_report.push(Util.get_smell_obj(src, msg, smell_level));
        }
    }
    return file_report;
}


// var str = "d/*ssdaasdasd/*";
// console.log(un_commented_strlen(str));