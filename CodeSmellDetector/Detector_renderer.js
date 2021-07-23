const assert = require('assert');

module.exports = class DetectorRenderer{

    static detector_div_ids;
    static detector_nav_button_ids;
    static currently_selected_det_div;
    static currently_selected_nav_button;


    constructor(detectors_holder, detectors_nav){
        this.detectors_holder = detectors_holder;
        this.detectors_nav = detectors_nav;
    }

    get_checked(flag){
        if(flag)
            return "checked";
        return "";
    }

    static nav_button_clicked(button_num, init = false){
        if(init === true){
            DetectorRenderer.currently_selected_det_div = document.getElementById(DetectorRenderer.detector_div_ids[button_num]);
            DetectorRenderer.currently_selected_nav_button = document.getElementById(DetectorRenderer.detector_nav_button_ids[button_num]);
            DetectorRenderer.currently_selected_det_div.style.display = "block";
            DetectorRenderer.currently_selected_nav_button.style.border = "2px solid black";

        }
        else {
            DetectorRenderer.currently_selected_det_div.style.display = "none";
            DetectorRenderer.currently_selected_nav_button.style.border = "none";
            DetectorRenderer.currently_selected_det_div = document.getElementById(DetectorRenderer.detector_div_ids[button_num]);
            DetectorRenderer.currently_selected_nav_button = document.getElementById(DetectorRenderer.detector_nav_button_ids[button_num]);
            DetectorRenderer.currently_selected_det_div.style.display = "block";
            DetectorRenderer.currently_selected_nav_button.style.border = "2px solid black";           
        }
    }


    render(smells_config){
        let smell_detectors = smells_config.smells;
        if(smell_detectors.length === 0){
            console.log("No detectors to render");
            return;
        }
        DetectorRenderer.detector_div_ids = [];
        DetectorRenderer.detector_nav_button_ids = [];



        let holder_html = "<form>";
        let nav_html = "";
        //this.html_detectors_div.innerHTML = "<form>";
        let detector_id = 0;
        let sliders = [];
        let slider_id = 0;
        let dropdowns = [];
        let dropdown_id = 0;
        
        for(let detector of smell_detectors){
            nav_html += `<button id='det_nav_b_${detector_id}' onclick='DetectorRenderer.nav_button_clicked(${detector_id})'>${detector.name}</button>`;
            // document.getElementById(`det_nav_b_${detector_id}`).onclick = () => {
            //     console.log("si mesire");
            //     this.nav_button_clicked(detector_id);
            // };
            DetectorRenderer.detector_nav_button_ids.push(`det_nav_b_${detector_id}`);


            holder_html += 
            `<div id='detector_${detector_id}' class='detector'>` +
            `<h2>${detector.name}</h2>`;
            DetectorRenderer.detector_div_ids.push(`detector_${detector_id}`);
            
            for(let arg_name of Object.keys(detector.args)){
                let arg = detector.args[arg_name];
                if(arg.option === undefined){
                    console.log("TODO");
                    continue;
                }
                switch(arg.option.type){
                    case "min-max-slider":{
                        let slider = new Object();
                        slider.values_label = "lbl" + (slider_id);
                        slider.div = "slider" + (slider_id++);
                        slider.arg_obj = arg;
                        holder_html += `<span>${arg.formal_name}  <label id="${slider.values_label}"></label></span>`;
                        holder_html += `<div id="${slider.div}" class="dbl-slider"></div>`;
                        sliders.push(slider);
                        break;
                    }
                        
                    case "boolean":{
                        holder_html += `${arg.formal_name}` + `<input type="checkbox" ${this.get_checked(arg.val)}><br>`;
                        break;
                    }
                        
                    case "dict_item":{
                        let dropdown = new Object();
                        dropdown.id = dropdown_id;
                        dropdown.div = "dropdown" + (dropdown_id++);
                        dropdown.arg_obj = arg;
                        dropdown.detector = detector;
                        holder_html += `<span>${arg.formal_name}  <label id="${dropdown.div}"></label></span><br>`;
                        dropdowns.push(dropdown);
                        break;
                    }
                }
            }
            // holder_html += `Hide Results <input type="checkbox" id="${detector_id}_hidden" ${this.get_checked(detector.hidden)}>`;
            holder_html += "</div>"
            detector_id++;
        }
        holder_html += "</form>";
        this.detectors_holder.innerHTML = holder_html;
        this.detectors_nav.innerHTML = nav_html;
        


        for(const slider of sliders){
            $(`#${slider.div}`).slider({
                range: true,
                min: slider.arg_obj.option.range[0],
                max: slider.arg_obj.option.range[1],
                values: [slider.arg_obj.min, slider.arg_obj.max],
                slide: (event, ui) => {
                    $(`#${slider.values_label}`).text(ui.values[0] + " - " + ui.values[1]);
                    slider.arg_obj.min = ui.values[0];
                    slider.arg_obj.max = ui.values[1];
                }
            });
            //$(`#${slider.values_label}`).text("kekw");
            $(`#${slider.values_label}`).text($(`#${slider.div}`).slider("values", 0) + " - " + $(`#${slider.div}`).slider("values", 1));
        }

        for(const dropdown of dropdowns){
            let select = document.createElement("select");
            select.name = "select" + dropdown.id;
            select.id = "select" + dropdown.id;
            
            for(const val of Object.keys(dropdown.detector.args[dropdown.arg_obj.option.dict])){
                let option = document.createElement("option");
                option.value = val;
                option.text = val;
                select.appendChild(option);
            }
        
            document.getElementById(dropdown.div).appendChild(select);
        }


        DetectorRenderer.nav_button_clicked(0, true);
//         $(`#0`).slider({
//             range: true,
//             min: 0,
//             max: 500,
//             values: [ 75, 300 ],
//             slide: function( event, ui ) {
//                 $( "#amount" ).val( "$" + ui.values[ 0 ] + " - $" + ui.values[ 1 ] );
//             }
//         });
//         $( "#amount" ).val( "$" + $( `#0` ).slider( "values", 0 ) +
// " - $" + $( `#0` ).slider( "values", 1 ) );

        
        //this.html_detectors_div.innerHTML = hmtl_str;
    }
};