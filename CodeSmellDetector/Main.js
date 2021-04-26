const fs = require("fs");
const path = require("path");
const Util = require("./Utility");

// dir: relative path to dir where smells.js are found
function get_smells(smells_config){
    let smells = [];
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
    console.log();
    return smells;
}

async function run_smells(smells, ST){
    let promises = [];
    for(const smell of smells){
        console.log(`Running smell detector: "${smell.name}"`);
        let promise = Util.execute_smell_callback(smell.callback, ST, smell.args);
        promises.push(promise);
    }
    console.log();
    let reports = await Promise.all(promises);
    let i = 0;
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
        console.log(`Incidents count: ${smell.report.incidents.length}\n`);
        let counter = 1;
        
        for (const incident of smell.report.incidents){
            console.log(`Incident ${counter++}`);
            console.log(`Location: ${incident.src.file}:${incident.src.line}:${incident.src.col}`);
            console.log(`Message: ${incident.msg}`);
            console.log(`Smell level: ${incident.lvl}\n`);
        }
    }
}

function print_stats(smells){
    for(const smell of smells){
        console.log(`Smell: ${smell.name}`);
        console.log(`Incidents count: ${smell.report.incidents.length}`);
        console.log(`Execution time: ${smell.report.time}\n`);
    }
}


async function main(){
    const st_path = "../ST.json";
    const smells_cfg_path = "./SmellsConfig.json";
    let ST = require(st_path); // loads json ST.
    let smells_config = require(smells_cfg_path);

    let smells = get_smells(smells_config);
    await run_smells(smells, ST);
    print_reports(smells);
    print_stats(smells);


    try {
        const st_stats = fs.statSync(st_path);
        console.log(`ST last modified: ${st_stats.mtime}`);
    } catch (error) {
        console.log(error);
    }


}

main();