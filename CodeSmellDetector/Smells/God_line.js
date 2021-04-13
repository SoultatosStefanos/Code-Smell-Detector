
var Util = require("../Utility.js");
var fs = require("fs");
const readline = require("readline");
const { add_incident } = require("../Utility.js");

module.exports = {
    callback: async function(ST, args){
        var report = [];
        for(const header of ST.headers){
            var incidents = await iterate_file(header, args.max_line_len);
            for(const incident of incidents){
                report.push(incident);
            }
        }
        for(const source of ST.sources){
            var incidents = await iterate_file(source, args.max_line_len);
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


    var idx = 0;
    var idx2;
    var len = 0;
    if(comment_open){
        idx = str.indexOf("*/");
        if(idx === -1) return {len:0, comment_open:true};
        str = str.slice(idx+2);
        idx=0;
        comment_open = false;
    }
    
    idx = str.indexOf("/*");
    if(idx === -1) return {len:str.length, comment_open: false};
    
    var ret = un_commented_strlen(str.slice(idx + 2), true);
    ret.len += idx;
    return ret;

}

async function iterate_file(file_path, max_line_len){
    var file_report = [];
    const file_stream = fs.createReadStream(file_path);

    const rl = readline.createInterface({
        input: file_stream,
        crlfDelay: Infinity
    });
    var line_counter = 0;
    var in_comment = false;
    var info;
    for await(const line of rl){
        line_counter++;
        var line_comm_idx = line.indexOf("//");
        if(line_comm_idx !== -1){
            info = un_commented_strlen(line.slice(0, line_comm_idx));
        }
        else{
            info = un_commented_strlen(line);
        }
        in_comment = info.comment_open;
        if(info.len > max_line_len){
            add_incident(
                file_report,
                {file:file_path, line:line_counter, column:0},
                `Line ${line_counter} of ${file_path} has an un-commented length of ${info.len} characters`);
        }
    }
    return file_report;
}


// var str = "d/*ssdaasdasd/*";
// console.log(un_commented_strlen(str));