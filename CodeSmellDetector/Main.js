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

function run_smells(smells, ST){
    for(const smell of smells){
        console.log(`\nRunning smell detector: "${smell.name}"`);
        smell.report = smell.callback(ST, smell.args);
    }
}

function print_reports(smells){
    for(const smell of smells){

        console.log("------------------------------");
        console.log(`Smell: ${smell.name}`);
        console.log(`Incidents count: ${smell.report.length}\n`);
        var counter = 1;
        for(const incident of smell.report){
            console.log(`Incident ${counter++}`);
            console.log(`Location: ${incident.src.file}:${incident.src.line}:${incident.src.col}`);
            console.log(`Message: ${incident.msg}\n`);
        }
    }
}


var smells = get_smells(smells_config);
run_smells(smells, ST);
print_reports(smells);

