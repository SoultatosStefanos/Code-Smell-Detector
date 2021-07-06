const assert = require('assert');

module.exports = class DetectorRenderer{
    constructor(html_detectors_div){
        this.html_detectors_div = html_detectors_div;
    }

    get_checked(flag){
        if(flag)
            return "checked";
        return "";
    }


    render(smells_config){
        let smell_detectors = smells_config.smells;
        let html = "<form><ul>";
        //this.html_detectors_div.innerHTML = "<form>";
        let detector_id = 0;
        let sliders = [];
        let slider_id = 0;
        let dropdowns = [];
        let dropdown_id = 0;
        
        for(let detector of smell_detectors){
            html += 
            "<li><div class='detector'>" +
            `<h3>${detector.name}</h3>`;
            
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
                        html += `<span>${arg.formal_name}  <label id="${slider.values_label}"></label></span>`;
                        html += `<div id="${slider.div}" class="dbl-slider"></div>`;
                        sliders.push(slider);
                        break;
                    }
                        
                    case "boolean":{
                        html += `${arg.formal_name}` + `<input type="checkbox" ${this.get_checked(arg.val)}><br>`;
                        break;
                    }
                        
                    case "dict_item":{
                        let dropdown = new Object();
                        dropdown.id = dropdown_id;
                        dropdown.div = "dropdown" + (dropdown_id++);
                        dropdown.arg_obj = arg;
                        dropdown.detector = detector;
                        html += `<span>${arg.formal_name}  <label id="${dropdown.div}"></label></span><br>`;
                        dropdowns.push(dropdown);
                        break;
                    }
                }
            }
            html += `Hide Results <input type="checkbox" id="${detector_id}_hidden" ${this.get_checked(detector.hidden)}></div><br><br></li>`;
            detector_id++;
        }
        html += "</ul></form>";
        this.html_detectors_div.innerHTML = html;


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
            $(`#${slider.values_label}`).text("kekw");
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