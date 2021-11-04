"use strict";

/**
     * @argument relative_path: relative path to Main.js || Main.js behaves as if under "./code-smell-detector-gui/src"
     * @returns full path
    */
function get_full_path(relative_path){
    let full_path = __dirname + "\\..\\..\\" + relative_path;
    full_path = path.normalize(full_path);
    return full_path.replace(/\\/g, "/");
}

const fs = require("fs");
const path = require("path");
const Util = require(get_full_path("Utility.js"));
const SmellRenderer = require(get_full_path("Smell_renderer.js"));
const DetectorRenderer = require(get_full_path("Detector_renderer.js"));
const StatsRenderer = require(get_full_path("Stats_renderer"));
const smells_cfg_path = require(get_full_path("MainConfig.json")).DetectorsConfig_Path;

// JavaScript runs an event loop, thus, global objects are necessary
let smells_config, smell_detectors, ST, smell_detectors_reports, smells_list = null, smell_renderer, stat_renderer;


async function initialize(){
    smells_list = await init_backend();
    if(smells_list === null || smells_list.length === 0){
        for(let det of smell_detectors)
            det.recompute_flag = true;
    }
    init_frontend();
}

/**
 * @returns previous smell reports if the st matches the one that was used last time. Otherwise null
 */
async function init_backend(){
    ST = require(Util.get_full_path(Util.st_path)); // loads json ST.
    Util.st_last_edit = fs.statSync(Util.get_full_path(Util.st_path)).mtime; // saves last edit time of st.
    Util.st_last_edit = Util.st_last_edit.toString();
    smells_config = require(get_full_path(smells_cfg_path));
    //smells_config = JSON.parse(JSON.stringify(smells_config)); // clone object, otherwise another require will return the same reference as above...
    smell_detectors = get_smell_detectors(smells_config);

    return Util.get_smells_from_cache();
}

async function init_frontend(){
    let smell_display_div = document.getElementById("smell_table");
    smell_renderer = new SmellRenderer(smell_display_div, smell_detectors, SmellRenderer.EnumPreferences.sort_by.Intensity, SmellRenderer.EnumPreferences.order.decreasing);
    
    // next args are div ids directly from index.html
    stat_renderer = new StatsRenderer();
    

    if(smells_list !== null){
        smell_renderer.render(smells_list);
        stat_renderer.compute_stats(smells_list, ST);
    }

    document.getElementById("b_compute_code_smells").onclick = async () => {
        try_compute_smells();
    };

    document.getElementById("b_code_smell_list").style.border = "3px solid black";
    document.getElementById("smells_display").style.display = "block";
    document.getElementById("smells_config").style.display = "none";
    document.getElementById("smells_stats").style.display = "none";

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


    // document.getElementById("b_stats_overall").style.border = "3px solid black";

    // document.getElementById("b_stats_overall").onclick = () => {
    //     stat_renderer.hide_all();
    //     document.getElementById("stats_overview").style.display = "block";
    //     let stat_tab_buttons = document.getElementsByClassName("b_stats_tab");
    //     for(let i=0; i<stat_tab_buttons.length; i++){
    //         stat_tab_buttons[i].style.border = "initial";
    //     }
    //     document.getElementById("b_stats_overall").style.border = "2px solid black";
    // }

    // document.getElementById("b_stats_structures").onclick = () => {
    //     stat_renderer.hide_all();
    //     document.getElementById("stats_by_class").style.display = "block";
    //     let stat_tab_buttons = document.getElementsByClassName("b_stats_tab");
    //     for(let i=0; i<stat_tab_buttons.length; i++){
    //         stat_tab_buttons[i].style.border = "initial";
    //     }
    //     document.getElementById("b_stats_structures").style.border = "2px solid black";
    // }

    // document.getElementById("b_stats_files").onclick = () => {
    //     stat_renderer.hide_all();
    //     document.getElementById("stats_by_file").style.display = "block";
    //     let stat_tab_buttons = document.getElementsByClassName("b_stats_tab");
    //     for(let i=0; i<stat_tab_buttons.length; i++){
    //         stat_tab_buttons[i].style.border = "initial";
    //     }
    //     document.getElementById("b_stats_files").style.border = "2px solid black";
    // }





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

    
   
    let detector_renderer = new DetectorRenderer(document.getElementById("detector_config_div"), document.getElementById("detector_cfg_nav"), get_full_path(smells_cfg_path), smells_config, smell_detectors);
    detector_renderer.render();
}


// statement below creates a dial_box used for recomputation confirmation.
// standard confirm() (or alert()) implementation bugs electron.
let dialog_box = (function(){
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

/**
     * @returns true only if detector with the name 'detector_name' exists and has its recompute_flag true.
*/
function has_recompute_flag(detector_name){
    for(let det of smell_detectors){
        if(det.name === detector_name && det.recompute_flag === true)
            return true;
    }
    return false;
}

/**
     * @returns note count in smells_list originating from one of the detectors with recompute_flag === true.
*/
function get_relevant_note_count(){
    if(smells_list === null)
        return 0;
        
    let note_count = 0;
    for(const smell of smells_list){
        if(smell.note !== undefined && has_recompute_flag(smell.detector))
            note_count++;
    }
    return note_count;
}

/**
    Confirms user re-computation request if notes are about to be deleted.
*/
function try_compute_smells(){
    let note_count = get_relevant_note_count();
    if(note_count === 0)
        compute_smells();
    else if(note_count === 1)
        dialog_box.open({content: "Are you sure you want to re-compute smells and delete 1 note?"});
    else
        dialog_box.open({content: `Are you sure you want to re-compute smells and delete ${note_count} notes?`});
}

function remove_smells_from_modified_detectors(smells_list){
    let i = 0;
    while (i < smells_list.length) {
        if (has_recompute_flag(smells_list[i].detector))
            smells_list.splice(i, 1);
        else
            i++;
    }
}

async function compute_smells(){
    smell_detectors_reports = await run_smell_detectors(smell_detectors, ST);
    if(smells_list)
        remove_smells_from_modified_detectors(smells_list);
    else
        smells_list = [];
    for(let report of smell_detectors_reports){
        for(let incident of report.incidents){
            smells_list.push(incident);
        }
    }
    document.getElementById("cfg_changed_label").innerHTML = "";
    for(let det of smell_detectors)
        det.recompute_flag = false;
    smell_renderer.render(smells_list);
    stat_renderer.compute_stats(smells_list, ST);
    await Util.save_smell_reports(smells_list);
    save_smell_config();
}



// function get_smell_detectors_names(_smell_detectors){
//     let smell_detectors_names = [];
//     for(let smell_detector of _smell_detectors){
//         smell_detectors_names.push(smell_detector.name);
//     }
//     return smell_detectors_names;
// }


// dir: relative path to dir where smells.js are found
function get_smell_detectors(smells_config){
    let _smell_detectors = [];
    for(let smell_detector of smells_config.detectors){
        try {
            var detector_path = get_full_path(smell_detector.file);
            var new_smell_detector = require(detector_path);
        } catch (error) {
            console.log(`Could not load smell detector "${smell_detector.name}" from "${detector_path}". Skipped.`);
            continue;
        }
        new_smell_detector.name = smell_detector.name;
        new_smell_detector.args = smell_detector.args;
        new_smell_detector.recompute_flag = false;
        _smell_detectors.push(new_smell_detector);
        console.log(`Loaded smell "${smell_detector.name}" from "${detector_path}".`);
    }
    return _smell_detectors;
}


/**
    * Runs asynchronously smell_detectors and wait for all to complete
    * @returns a list of smell_reports (objects)
    *   smell_reports (object) = {
    *       detector: [detector name],
    *       time: [time taken to run detector in seconds].
    *       incidents: [the list of smells produced by the detector]
    *   }
*/
async function run_smell_detectors(smell_detectors, ST){
    let smell_reports = [];
    for(const smell_detector of smell_detectors){
        if(!smell_detector.recompute_flag)
            continue;
        console.log(`Running smell detector: "${smell_detector.name}"`);
        let promise = Util.execute_smell_callback(smell_detector, ST);
        smell_reports.push(promise);
    }
    return await Promise.all(smell_reports);
}



// function print_reports(smell_reports){
//     for(const smell_report of smell_reports){
//         console.log("------------------------------");
//         console.log(`Smell Detector: ${smell_report.detector}`);
//         console.log(`Incidents count: ${smell_report.incidents.length}\n`);
//         let counter = 1;
        
//         for (const incident of smell_report.incidents){
//             console.log(`Incident ${counter++}`);
//             console.log(`Location: ${incident.src.file}:${incident.src.line}:${incident.src.col}`);
//             console.log(`Message: ${incident.msg}`);
//             console.log(`Smell level: ${incident.lvl}\n`);
//         }
//     }
// }

// function print_stats(smells){
//     for(const smell of smells){
//         console.log(`Smell: ${smell.name}`);
//         console.log(`Incidents count: ${smell.report.incidents.length}`);
//         console.log(`Execution time: ${smell.report.time}\n`);
//     }
// }

function save_smell_config(){ 
    let json = JSON.stringify(smells_config, null, 4);
    fs.writeFile(get_full_path(smells_cfg_path), json, "utf8", (error) => {
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