const assert = require('assert');

module.exports = class SmellRenderer{
    static EnumPreferences = {
        sort_by: {
            Intensity : 0,
            Type: 1,
            File: 2,
            Class: 3
        },
        order: {
            increasing: 1,
            decreasing: -1
        }
    };

    constructor(html_smell_table, sort_by, order){
        this.html_smell_table = html_smell_table;
        this.sort_by = sort_by;
        this.order = order;
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
            default:
                assert(false, "Unhandled smell sort option.");
                
        }
    }

    render(smells){
        smells.sort(this.get_smell_sort_func());



        //this.hmtl_smell_table.innerHTML = "";
        let html_str = "<tr><th>Detector</th>" +
        "<th>Intensity</th><th>Location</th><th>Structure</th><th>Message</th></tr>";
        for(const smell of smells){
            let green = 255 - 25.5 * smell.lvl;
            html_str += `<tr style='background-color:rgba(255, ${green}, 0, 0.6)'><td>${smell.detector}</td>` +
            `<td>${smell.lvl}</td><td><div class='l'>${smell.src.file}:${smell.src.line}:${smell.src.col}</div></td>` +
            `<td>${smell.src.struct}</td><td><div class='l'>${smell.msg}</div></td></tr>`;
        }
        this.html_smell_table.innerHTML = html_str;
    }
};