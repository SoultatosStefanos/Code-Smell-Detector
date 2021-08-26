const assert = require('assert');
const { exec } = require('child_process');
const Util = require("./Utility.js");
const text_editor_call = require("./MainConfig.json").TextEditorStartUpCommand;

module.exports = class SmellRenderer{
    static EnumPreferences = {
        sort_by: {
            Intensity : 0,
            Type: 1,
            File: 2,
            Class: 3,
            Note: 4
        },
        order: {
            increasing: 1,
            decreasing: -1
        }
    };

    static smells_ref;
    static modal;

    // // calls fn with delay = ms
    // static delay(fn, ms) {
    //     let timer = 0;
    //     return (...args) => {
    //         clearTimeout(timer);
    //         timer = setTimeout(fn.bind(this, ...args), ms || 0);
    //     }
    // }

    constructor(html_smell_table, smell_detectors, sort_by, order){
        this.html_smell_table = html_smell_table;
        this.smell_detectors = smell_detectors;
        this.shown_detectors = [];
        this.sort_by = sort_by;
        this.order = order;
        this.text_filter = "";
        
        this.init_show_only();

        this.smells_per_page = 10;
        this.smell_page = 1;
        this.total_smell_pages = 1;
        document.getElementById("b_10_smells_per_page").onclick = () => {
            this.smells_per_page = 10;
            this.render(SmellRenderer.smells_ref);
        };
        document.getElementById("b_20_smells_per_page").onclick = () => {
            this.smells_per_page = 20;
            this.render(SmellRenderer.smells_ref);
        };
        document.getElementById("b_50_smells_per_page").onclick = () => {
            this.smells_per_page = 50;
            this.render(SmellRenderer.smells_ref);
        };
        document.getElementById("b_100_smells_per_page").onclick = () => {
            this.smells_per_page = 100;
            this.render(SmellRenderer.smells_ref);
        };
        document.getElementById("b_200_smells_per_page").onclick = () => {
            this.smells_per_page = 200;
            this.render(SmellRenderer.smells_ref);
        };

        document.getElementById("b_prev_page").onclick = () => {
            if(this.smell_page === 1)
                return;
            this.smell_page--;
            this.render(SmellRenderer.smells_ref, false);
        }
        document.getElementById("b_next_page").onclick = () => {
            if(this.smell_page >= this.total_smell_pages)
                return;
            this.smell_page++;
            this.render(SmellRenderer.smells_ref, false);
        }
        

        // $("#seach_txtfield").keyup(Util.delay(() => {
        //     let text_field_contents = document.getElementById('seach_txtfield').value;
        //     this.text_filter = text_field_contents;
        //     this.render(SmellRenderer.smells_ref);
        //     // if(text_field_contents !== "")
        //     //     this.render(SmellRenderer.smells_ref, text_field_contents);
        //     // else
        //     //     this.render(SmellRenderer.smells_ref);
        // }, 500));

        $("#seach_txtfield").keyup(() => {
            Util.delay_method("smell_txt_search", () => {
                let text_field_contents = document.getElementById('seach_txtfield').value;
                this.text_filter = text_field_contents;
                this.render(SmellRenderer.smells_ref);
                // if(text_field_contents !== "")
                //     this.render(SmellRenderer.smells_ref, text_field_contents);
                // else
                //     this.render(SmellRenderer.smells_ref);
            }, 500);
        });

        SmellRenderer.modal = (function(){
            var method = {}, $overlay, $modal, $content, $close, $save, $delete, smell_ref, smell_num;
            // HTML HERE
            $overlay = $('<div id="overlay"></div>');
            $modal = $('<div id="modal"></div>');
            $content = $('<div id="content"></div>');
            $close = $('<a id="close" href="#">close</a>');
            $save = $('<a id="save" href="#">save</a>');
            $delete = $('<a id="delete" href="#">delete</a>');


            $(document).ready(function(){
                $('body').append($overlay, $modal);
            });

            $modal.hide();
            $overlay.hide();
            $modal.append($content, $close, $save, $delete);


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
                $content.empty().append(
                    "<form><textarea id='note_textfield'>" +
                    settings.content +
                    "</textarea></form>"
                );

                smell_ref = settings.smell;
                smell_num = settings.smell_num;

                $modal.css({
                    width: settings.width || 'auto', 
                    height: settings.height || 'auto'
                })

                method.center();

                $(window).bind('resize.modal', method.center);

                $modal.show();
                $overlay.show();
            };


            method.close = function(){
                $modal.hide();
                $overlay.hide();
                $content.empty();
                $(window).unbind('resize.modal');
            };

            method.save = function(){
                
                smell_ref.note = document.getElementById('note_textfield').value;
                $modal.hide();
                $overlay.hide();
                $content.empty();
                $(window).unbind('resize.modal');
                let note_holder = document.getElementById(`note_holder${smell_num}`);
                if(smell_ref.note === ""){
                    smell_ref.note = undefined;
                    note_holder.outerHTML = `<i id='note_holder${smell_num}' class='icon_add_note' onclick='SmellRenderer.edit_note(${smell_num})'></i>`;
                }
                else {
                    note_holder.outerHTML = `<i id='note_holder${smell_num}' class='icon_edit_note' onclick='SmellRenderer.edit_note(${smell_num})'></i>`;
                }
                Util.save_smell_reports(SmellRenderer.smells_ref);
            }

            method.delete = function(){
                smell_ref.note = undefined;
                Util.save_smell_reports(SmellRenderer.smells_ref);
                $modal.hide();
                $overlay.hide();
                $content.empty();
                $(window).unbind('resize.modal');
                let note_holder = document.getElementById(`note_holder${smell_num}`);
                note_holder.outerHTML = `<i id='note_holder${smell_num}' class='icon_add_note' onclick='SmellRenderer.edit_note(${smell_num})'></i>`;
            }

            $close.click(function(e){
                e.preventDefault();
                method.close();
            });

            $save.click(function(e){
                e.preventDefault();
                method.save();
            });

            $delete.click(function(e){
                e.preventDefault();
                method.delete();
            });


            return method;
        }());
    }

    shown_detectors_edit(re_render_smells = true){
        this.shown_detectors = [];
        let detectors_checkboxes = document.getElementsByClassName("shown_detectors");
        for(let i=0; i<detectors_checkboxes.length; i++){
            if(detectors_checkboxes[i].checked === true){
                let parent_div = detectors_checkboxes[i].parentElement;
                this.shown_detectors.push(parent_div.textContent);
            }
        }
        if(re_render_smells)
            this.render(SmellRenderer.smells_ref);
        //console.log(this.shown_detectors);
    }

    init_show_only(){
        let innerHTML_text = "";
        for(const detector of this.smell_detectors){
            if(detector.hidden === true)
                innerHTML_text += `<div class="dropdown_b"><input type="checkbox" class="shown_detectors"/>${detector.name}</div>`;
            else
                innerHTML_text += `<div class="dropdown_b"><input type="checkbox" class="shown_detectors" checked/>${detector.name}</div>`;
        }
        document.getElementById("show_only_dropdown_content").innerHTML = innerHTML_text;
        let detectors_checkboxes = document.getElementsByClassName("shown_detectors");
        for(let i=0; i<detectors_checkboxes.length; i++){
            detectors_checkboxes[i].onclick = () => {
                this.shown_detectors_edit();
            };
        }
        this.shown_detectors_edit(false);
    }

    set_sort_by_pref(sort_by){
        this.sort_by = sort_by;
    }

    set_order_pref(order){
        this.order = order;
    }

    get_smell_sort_func(){
        switch(this.sort_by){
            case SmellRenderer.EnumPreferences.sort_by.Intensity: {
                return (smell1, smell2) => {
                    return this.order * (smell1.lvl - smell2.lvl);
                };
            }
            case SmellRenderer.EnumPreferences.sort_by.Type: {
                return (smell1, smell2) => {
                    return this.order * (smell1.detector.localeCompare(smell2.detector));
                };
            }
            case SmellRenderer.EnumPreferences.sort_by.File: {
                return (smell1, smell2) => {
                    return this.order * (smell1.src.file.localeCompare(smell2.src.file));
                };
            }
            case SmellRenderer.EnumPreferences.sort_by.Class: {
                return (smell1, smell2) => {
                    if(smell1.src.struct === null)
                        return this.order;
                    if(smell2.src.struct === null)
                        return -this.order;
                    return this.order * (smell1.src.struct.localeCompare(smell2.src.struct));
                };
            }
            case SmellRenderer.EnumPreferences.sort_by.Note: {
                return (smell1, smell2) => {
                    if(smell1.note !== undefined){
                        if(smell2.note === undefined) 
                            return this.order;
                        return -this.order;
                    }
                    else {
                        if(smell2.note === undefined) 
                            return this.order;
                        return -this.order;
                    }
                }
            }
            default:
                assert(false, "Unhandled smell sort option.");
                
        }
    }

    get_table_header(){
        let icon;
        if(this.order === SmellRenderer.EnumPreferences.order.decreasing){
            icon = "<i class='arrow arr_down'></i>"
        }
        else{
            icon = "<i class='arrow arr_up'></i>"
        }

        switch(this.sort_by){
            case SmellRenderer.EnumPreferences.sort_by.Intensity: {
                return `<tr><th>Detector</th><th>Intensity ${icon}</th><th>Location</th><th>Structure</th><th>Message</th><th>Note</th></tr>`;
            }
            case SmellRenderer.EnumPreferences.sort_by.Type: {
                return `<tr><th>Detector ${icon}</th><th>Intensity</th><th>Location</th><th>Structure</th><th>Message</th><th>Note</th></tr>`;
            }
            case SmellRenderer.EnumPreferences.sort_by.File: {
                return `<tr><th>Detector</th><th>Intensity</th><th>Location ${icon}</th><th>Structure</th><th>Message</th><th>Note</th></tr>`;
            }
            case SmellRenderer.EnumPreferences.sort_by.Class: {
                return `<tr><th>Detector</th><th>Intensity</th><th>Location</th><th>Structure ${icon}</th><th>Message</th><th>Note</th></tr>`;
            }
            case SmellRenderer.EnumPreferences.sort_by.Note: {
                return `<tr><th>Detector</th><th>Intensity</th><th>Location</th><th>Structure</th><th>Message</th><th>Note ${icon}</th></tr>`;
            }
            default:
                assert(false, "Unhandled smell sort option.");
                
        }
    }

    static edit_note(smell_num){
        let smell = SmellRenderer.smells_ref[smell_num];
        if(smell.note === undefined){
            SmellRenderer.modal.open({smell: smell, smell_num: smell_num, content: ""});
        }
        else {
            SmellRenderer.modal.open({smell: smell, smell_num: smell_num, content: smell.note});
        }

    }


    static open_in_text_editor(){
        let sibling_divs = this.parentElement.childNodes;
        Array.from(sibling_divs).forEach(function(element) {
            if(element.classList.contains("path_txt")){
                let path_to_open = element.innerHTML;
                exec(text_editor_call + " " + path_to_open, (err, stdout, stderr) => {
                    if (err) {
                        alert("Could not open in text editor");
                        return;
                    }
                });
                return;
            }
        });
        // console.log(this.parentElement.childNodes);
        // return;
        
    }

    set_text_editor_on_path_click(){
        let path_elements = document.getElementsByClassName("icon_link");

        Array.from(path_elements).forEach(function(element) {
            element.addEventListener('click', SmellRenderer.open_in_text_editor);
        });
    }

    static smell_contains_query(smell, query){
        assert(query !== "");

        query = query.replace(/[.*+?^${}()|[\]\\]/g, '\\$&'); // escape any regex characters - only search pure text
        let regex = new RegExp(query, "i");
        if(regex.test(smell.detector))
            return true;
        if(regex.test(smell.lvl))
            return true;
        if(regex.test(smell.src.file))
            return true;
        if(regex.test(smell.src.structure))
            return true;
        if(regex.test(smell.src.method))
            return true;
        if(regex.test(smell.msg))
            return true;
        if(regex.test(smell.note))
            return true;
        return false;
    }

    async render(smells, reset_page = true){
        if(reset_page)
            this.smell_page = 1;
        
        smells.sort(this.get_smell_sort_func());

        let html_str = this.get_table_header();

        //this.hmtl_smell_table.innerHTML = "";
        // let html_str = "<tr><th>Detector</th>" +
        // "<th>Intensity</th><th>Location</th><th>Structure</th><th>Message</th></tr>";
        let smell_num = -1;
        let candidate_shown_smells = 0; // this is the sum of smells that could be shown. However out of all the candidates some will be shown only, those that belong to the current page.
        let first_shown_smell = (this.smell_page - 1) * this.smells_per_page;
        let last_shown_smell = (this.smell_page * this.smells_per_page) - 1;
        for(const smell of smells){
            smell_num++;
            if(!this.shown_detectors.includes(smell.detector)) continue;
            if(this.text_filter !== ""){
                if(!SmellRenderer.smell_contains_query(smell, this.text_filter)) continue;
            }

            if((candidate_shown_smells < first_shown_smell) || (candidate_shown_smells > last_shown_smell)){
                candidate_shown_smells++;
                continue;
            }
            candidate_shown_smells++;


            let green = 255 - 25.5 * smell.lvl;
            html_str += `<tr style='background-color:rgba(255, ${green}, 0, 0.6)'><td>${smell.detector}</td>` +
            `<td>${smell.lvl}</td><td><div class='l'><i class='icon_link'></i><div class='path_txt'>${smell.src.file}:${smell.src.line}:${smell.src.col}</div></div></td>` +
            `<td>${smell.src.struct != null ? smell.src.struct : ""}</td><td><div class='l'>${smell.msg}</div></td><td>`;
            if(smell.note === undefined){
                html_str += `<i id='note_holder${smell_num}' class='icon_add_note' onclick='SmellRenderer.edit_note(${smell_num})'></i></td></tr>`;
            }
            else {
                html_str += `<i id='note_holder${smell_num}' class='icon_edit_note' onclick='SmellRenderer.edit_note(${smell_num})'></i></td></tr>`;
            }
        }
        this.total_smell_pages = Math.trunc((candidate_shown_smells - 1) / this.smells_per_page) + 1;


        this.html_smell_table.innerHTML = html_str;
        SmellRenderer.smells_ref = smells;
        document.getElementById("page_info").innerHTML = ` Page ${this.smell_page}/${this.total_smell_pages} `;
        this.set_text_editor_on_path_click();
    }
};
