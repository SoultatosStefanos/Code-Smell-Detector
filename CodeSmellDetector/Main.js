const fs = require("fs");
const path = require("path");
const Util = require("D:/Thesis/CodeSmellDetector/Utility");
const SmellRenderer = require("D:/Thesis/CodeSmellDetector/Smell_renderer.js");
const DetectorRenderer = require("D:/Thesis/CodeSmellDetector/Detector_renderer.js");
const StatsRenderer = require("D:/Thesis/CodeSmellDetector/Stats_renderer.js");



//const st_path = "D:/Thesis/ST.json";
//let st_last_edit = null;
const smells_cfg_path = "D:/Thesis/CodeSmellDetector/SmellsConfig.json";


let smells_config, smell_detectors, ST, smell_detectors_reports, smells_list = null, smell_renderer, stat_renderer;




// async function save_smell_reports(smell_reports){
//     let json = new Object();
//     let computed_for = new Object();
//     computed_for.file = st_path;
//     computed_for.last_edit = st_last_edit;
//     json.computed_for = computed_for;
//     json.smells = smell_reports;

//     json = JSON.stringify(json, null, 4);
//     fs.writeFile(smell_reports_save, json, "utf8", (error) => {
//         if(error) throw error;
//     });
// }






async function initialize(){
    smells_list = await init_backend();
    init_frontend();
}

/**
 * @returns previous smell reports if the st matches the one that was used last time. Otherwise null
 */
async function init_backend(){
    ST = require(Util.st_path); // loads json ST.
    Util.st_last_edit = fs.statSync(Util.st_path).mtime; // saves last edit time of st.
    Util.st_last_edit = Util.st_last_edit.toString();
    smells_config = require(smells_cfg_path);
    smell_detectors = get_smell_detectors(smells_config);

    return Util.get_smells_from_cache();
}

async function init_frontend(){
    let smell_display_div = document.getElementById("smell_table");
    smell_renderer = new SmellRenderer(smell_display_div, SmellRenderer.EnumPreferences.sort_by.Intensity, SmellRenderer.EnumPreferences.order.decreasing);
    
    // next args are div ids directly from index.html
    stat_renderer = new StatsRenderer();
    

    if(smells_list !== null){
        smell_renderer.render(smells_list);
        stat_renderer.compute_stats(smells_list);
    }



    document.getElementById("b_compute_code_smells").onclick = async () => {
        try_compute_smells();
    };
    document.getElementById("b_code_smell_list").style.border = "3px solid black";

    document.getElementById("b_code_smell_list").onclick = () => {
        document.getElementById("smells_display").style.display = "block";
        document.getElementById("smells_config").style.display = "none";
        document.getElementById("smells_stats").style.display = "none";
        let main_buttons = document.getElementsByClassName("main_button");
        for(let i=0; i<main_buttons.length; i++){
            main_buttons[i].style.border = "initial";
        }
        document.getElementById("b_code_smell_list").style.border = "3px solid black";
    }

    document.getElementById("b_code_smell_config").onclick = () => {
        document.getElementById("smells_display").style.display = "none";
        document.getElementById("smells_config").style.display = "block";
        document.getElementById("smells_stats").style.display = "none";
        let main_buttons = document.getElementsByClassName("main_button");
        for(let i=0; i<main_buttons.length; i++){
            main_buttons[i].style.border = "initial";
        }
        document.getElementById("b_code_smell_config").style.border = "3px solid black";
    };

    document.getElementById("b_code_smell_stats").onclick = () => {
        document.getElementById("smells_display").style.display = "none";
        document.getElementById("smells_config").style.display = "none";
        document.getElementById("smells_stats").style.display = "block";
        let main_buttons = document.getElementsByClassName("main_button");
        for(let i=0; i<main_buttons.length; i++){
            main_buttons[i].style.border = "initial";
        }
        document.getElementById("b_code_smell_stats").style.border = "3px solid black";
    }


    document.getElementById("b_stats_overall").style.border = "3px solid black";

    document.getElementById("b_stats_overall").onclick = () => {
        stat_renderer.hide_all();
        document.getElementById("stats_overview").style.display = "block";
        let stat_tab_buttons = document.getElementsByClassName("b_stats_tab");
        for(let i=0; i<stat_tab_buttons.length; i++){
            stat_tab_buttons[i].style.border = "initial";
        }
        document.getElementById("b_stats_overall").style.border = "2px solid black";
    }

    document.getElementById("b_stats_structures").onclick = () => {
        stat_renderer.hide_all();
        document.getElementById("stats_by_class").style.display = "block";
        let stat_tab_buttons = document.getElementsByClassName("b_stats_tab");
        for(let i=0; i<stat_tab_buttons.length; i++){
            stat_tab_buttons[i].style.border = "initial";
        }
        document.getElementById("b_stats_structures").style.border = "2px solid black";
    }

    document.getElementById("b_stats_files").onclick = () => {
        stat_renderer.hide_all();
        document.getElementById("stats_by_file").style.display = "block";
        let stat_tab_buttons = document.getElementsByClassName("b_stats_tab");
        for(let i=0; i<stat_tab_buttons.length; i++){
            stat_tab_buttons[i].style.border = "initial";
        }
        document.getElementById("b_stats_files").style.border = "2px solid black";
    }





    document.getElementById("b_sort_by_intensity").onclick = async () => {
        smell_renderer.set_sort_by_pref(SmellRenderer.EnumPreferences.sort_by.Intensity);
        smell_renderer.render(smells_list);
    };

    document.getElementById("b_sort_by_type").onclick = async () => {
        smell_renderer.set_sort_by_pref(SmellRenderer.EnumPreferences.sort_by.Type);
        smell_renderer.render(smells_list);
    };

    document.getElementById("b_sort_by_file").onclick = async () => {
        smell_renderer.set_sort_by_pref(SmellRenderer.EnumPreferences.sort_by.File);
        smell_renderer.render(smells_list);
    };

    document.getElementById("b_sort_by_class").onclick = async () => {
        smell_renderer.set_sort_by_pref(SmellRenderer.EnumPreferences.sort_by.Class);
        smell_renderer.render(smells_list);
    };

    document.getElementById("b_sort_by_note").onclick = async () => {
        smell_renderer.set_sort_by_pref(SmellRenderer.EnumPreferences.sort_by.Note);
        smell_renderer.render(smells_list);
    }

    document.getElementById("b_order_incr").onclick = async () => {
        smell_renderer.set_order_pref(SmellRenderer.EnumPreferences.order.increasing);
        smell_renderer.render(smells_list);
    };

    document.getElementById("b_order_decr").onclick = async () => {
        smell_renderer.set_order_pref(SmellRenderer.EnumPreferences.order.decreasing);
        smell_renderer.render(smells_list);
    };

    
   


    let detector_renderer = new DetectorRenderer(document.getElementById("smells_config"));
    detector_renderer.render(smells_config);
}

dialog_box = (function(){
    var method = {}, $overlay, $modal, $content, $yes, $no;
    // HTML HERE
    $overlay = $('<div id="overlay"></div>');
    $modal = $('<div id="modal"></div>');
    $content = $('<div id="content"></div>');
    $yes = $('<button id="yes" href="#">Yes</button>');
    $no= $('<button id="no" href="#">No</button>');


    $(document).ready(function(){
        $('body').append($overlay, $modal);
    });

    $modal.hide();
    $overlay.hide();
    $modal.append($content, $yes, $no);


    method.center = function(){
        var top, left;

        top = Math.max($(window).height() - $modal.outerHeight(), 0) / 2;
        left = Math.max($(window).width() - $modal.outerWidth(), 0) / 2;

        $modal.css({
            top:top + $(window).scrollTop(), 
            left:left + $(window).scrollLeft()
        });
    };


    method.open = function(settings){
        $content.empty().append(`<p>${settings.content}</p>`);

        //smell_ref = settings.smell;
        //smell_num = settings.smell_num;

        $modal.css({
            width: settings.width || 'auto', 
            height: settings.height || 'auto'
        })

        method.center();

        $(window).bind('resize.modal', method.center);

        $modal.show();
        $overlay.show();
    };


    method.yes = function(){
        $modal.hide();
        $overlay.hide();
        $content.empty();
        $(window).unbind('resize.modal');
        compute_smells();
    };

    method.no = function(){
        $modal.hide();
        $overlay.hide();
        $content.empty();
        $(window).unbind('resize.modal');
    }


    $yes.click(function(e){
        e.preventDefault();
        method.yes();
    });

    $no.click(function(e){
        e.preventDefault();
        method.no();
    });

    return method;
}());


function get_note_count(){
    let note_count = 0;
    for(const smell of smells_list){
        if(smell.note !== undefined) note_count++;
    }
    return note_count;
}

function try_compute_smells(){
    let note_count = get_note_count();
    if(note_count === 0)
        compute_smells();
    else if(note_count === 1)
        dialog_box.open({content: "Are you sure you want to re-compute smells deleting 1 note?"});
    else
        dialog_box.open({content: `Are you sure you want to re-compute smells deleting ${note_count} notes?`});
}

async function compute_smells(){
    // if(!approve_smell_recomputation(smells_list)) 
    //         return;
    smell_detectors_reports = await run_smell_detectors(smell_detectors, ST);
    smells_list = [];
    for(let report of smell_detectors_reports){
        for(let incident of report.incidents){
            smells_list.push(incident);
        }
    }
    smell_renderer.render(smells_list);
    stat_renderer.compute_stats(smells_list);
    await Util.save_smell_reports(smells_list);
    save_smell_config();
}






// dir: relative path to dir where smells.js are found
function get_smell_detectors(smells_config){
    let smells = [];
    for(let smell of smells_config.smells){
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

async function run_smell_detectors(smell_detectors, ST){
    let smell_reports = [];
    for(const smell_detector of smell_detectors){
        console.log(`Running smell detector: "${smell_detector.name}"`);
        let promise = Util.execute_smell_callback(smell_detector, ST);
        smell_reports.push(promise);
    }
    return await Promise.all(smell_reports);
}



function print_reports(smell_reports){
    for(const smell_report of smell_reports){
        console.log("------------------------------");
        console.log(`Smell Detector: ${smell_report.detector}`);
        console.log(`Incidents count: ${smell_report.incidents.length}\n`);
        let counter = 1;
        
        for (const incident of smell_report.incidents){
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

function save_smell_config(){ 
    let json = JSON.stringify(smells_config, null, 4);
    fs.writeFile(smells_cfg_path, json, "utf8", (error) => {
        if(error) throw error;
    });
}


// async function main(){
    
//     let ST = require(st_path); // loads json ST.
//     let smells_config = require(smells_cfg_path);

//     let smells = get_smell_detectors(smells_config);
//     await run_smell_detectors(smells, ST);
//     //print_reports(smells);
//     print_stats(smells);





// }

//main(); 
initialize();