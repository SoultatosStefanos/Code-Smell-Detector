const fs = require("fs");
const path = require("path");
var ST = require("../ST.json"); // loads json ST.


// dir: relative path to dir where smells.js are found
function get_smells(dir){ 
    const files = fs.readdirSync(path.join(__dirname, dir));
    var smells = [];
    for(const file of files){
        smells.push(require(path.join(__dirname, dir, file)));
    }
    return smells;
}

function run_smells(smells, ST){
    for(const smell of smells){
        console.log(`Running smell detector: ${smell.name}`);
        smell.callback(ST);
    }
}


smells = get_smells("Smells");
run_smells(smells, ST);


//console.log(ST);