const fs = require("fs");
const path = require("path");
var ST = require("../ST.json"); // loads json ST.
var smells_config = require("./SmellsConfig.json");

// dir: relative path to dir where smells.js are found
function get_smells(smells_config){
    var smells = [];
    for(const smell of smells_config.smells){
        try {
            var smell_path = path.join(smells_config.folder, smell.file);
            var new_smell = require(smell_path);
        } catch (error) {
            console.log(`Could not load smell "${smell.name}" from "${smell_path}". Skipped.`);
            continue;
        }
        new_smell.name = smell.name;
        new_smell.args = smell.args;
        smells.push(new_smell);
        console.log(`Loaded smell "${smell.name}" from "${smell_path}".`);
    }
    return smells;
}

async function run_smells(smells, ST){
    var promises = [];
    for(const smell of smells){
        console.log(`\nRunning smell detector: "${smell.name}"`);
        promises.push(smell.callback(ST, smell.args));
    }
    var reports = await Promise.all(promises);
    var i = 0;
    for(const smell of smells){
        smell.report = reports[i++];
    }
}

function print_reports(smells){
    for(const smell of smells){
        //smell.report = await smell.report;
        //console.log(smell.report);
        console.log("------------------------------");
        console.log(`Smell: ${smell.name}`);
        console.log(`Incidents count: ${smell.report.length}\n`);
        var counter = 1;
        
        for (const incident of smell.report){
            console.log(`Incident ${counter++}`);
            console.log(`Location: ${incident.src.file}:${incident.src.line}:${incident.src.col}`);
            console.log(`Message: ${incident.msg}`);
            console.log(`Smell level: ${incident.lvl}\n`);
        }
    }
}




async function main(){
    var smells = get_smells(smells_config);
    await run_smells(smells, ST);
    print_reports(smells);
}

main();