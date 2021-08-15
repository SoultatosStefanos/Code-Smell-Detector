const assert = require('assert');
const { Stats } = require('fs');
const Util = require('./Utility');

module.exports = class StatsRenderer{
    constructor(overview_div, by_class_div, by_file_div){
        this.overview_div = document.getElementById("stats_overview");
        this.by_class_div = document.getElementById("stats_by_class");
        this.by_file_div = document.getElementById("stats_by_file");
        this.by_class_div.style.display = "none";
        this.by_file_div.style.display = "none";
    }

    hide_all(){
        this.overview_div.style.display = "none";
        this.by_class_div.style.display = "none";
        this.by_file_div.style.display = "none";
    }

    clear_all(){
        // clear overview tables
        document.getElementById("stats_overview_sc").innerHTML = "-------";
        document.getElementById("stats_overview_ti").innerHTML = "-------";
        document.getElementById("stats_overview_ai").innerHTML = "-------";
        document.getElementById("stats_overview_mi").innerHTML = "-------";
        // document.getElementById("stats_overview_swn").innerHTML = "-------";
        document.getElementById("stats_overview_fws").innerHTML = "-------";
        document.getElementById("stats_overview_pofws").innerHTML = "-------";
        document.getElementById("stats_overview_sws").innerHTML = "-------";
        document.getElementById("stats_overview_posws").innerHTML = "-------";
        document.getElementById("stats_overview_mws").innerHTML = "-------";
        document.getElementById("stats_overview_pomws").innerHTML = "-------";
        document.getElementById("stats_overview_mcs").innerHTML = "-------";
        document.getElementById("stats_overview_fwhsi").innerHTML = "-------";
        document.getElementById("stats_overview_fwms").innerHTML = "-------";
        document.getElementById("stats_overview_swhsi").innerHTML = "-------";
        document.getElementById("stats_overview_swms").innerHTML = "-------";
        document.getElementById("stats_overview_mwhsi").innerHTML = "-------";
        document.getElementById("stats_overview_mwms").innerHTML = "-------";



        // clear by_class and by_div graphs.
        this.by_class_div.innerHTML = "";
        this.by_file_div.innerHTML = "";
    }

    async compute_stats(smells, ST){
        this.clear_all();
    
        let total_intensity = 0;
        let avg_intensity;
        let max_intensity = 0;
        //let smells_with_notes = 0;
        let structures_with_smells = new Object();
        let files_with_smells = new Object();
        let methods_with_smells = new Object();
        let smell_count_per_detector = new Object();


        for(const smell of smells){
            if(smell.lvl > max_intensity)
                max_intensity = smell.lvl;
            total_intensity += smell.lvl;
            // if(smell.note !== undefined)
            //     smells_with_notes++;
            if(smell.src.struct !== null){
                if(structures_with_smells[smell.src.struct] === undefined){
                    //structures_with_smells[smell.src.struct] = [smell.lvl];
                    structures_with_smells[smell.src.struct] = new Object();
                    structures_with_smells[smell.src.struct].smell_lvls = [smell.lvl];
                    //structures_with_smells[smell.src.struct].src = structure src location ...;
                }
                else
                    structures_with_smells[smell.src.struct].smell_lvls.push(smell.lvl);
            }
            if(smell.src.file !== ""){
                if(files_with_smells[smell.src.file] === undefined){
                    // files_with_smells[smell.src.file] = [smell.lvl];
                    files_with_smells[smell.src.file] = new Object();
                    files_with_smells[smell.src.file].smell_lvls = [smell.lvl];
                }
                else
                    files_with_smells[smell.src.file].smell_lvls.push(smell.lvl);
            }
            if(smell.src.method !== null){
                if(methods_with_smells[smell.src.method] === undefined){
                    methods_with_smells[smell.src.method] = new Object();
                    methods_with_smells[smell.src.method].smell_lvls = [smell.lvl];
                }
                else
                    methods_with_smells[smell.src.method].smell_lvls.push(smell.lvl);
            }

            if(smell_count_per_detector[smell.detector] === undefined)
                smell_count_per_detector[smell.detector] = 1;
            else
                smell_count_per_detector[smell.detector]++;

        }

        if(smells.length !== 0){
            avg_intensity = total_intensity / smells.length;
            avg_intensity = Math.round(avg_intensity*100) / 100;
        }
        else
            avg_intensity = "-------";
        
        total_intensity = Math.round(total_intensity*100) / 100;

        let most_smells_of_a_type = 0, detector_with_most_smells;
        for (const [key, value] of Object.entries(smell_count_per_detector)){
            if(value > most_smells_of_a_type){
                most_smells_of_a_type = value;
                detector_with_most_smells = key;
            }
        }

        let countof_files_with_smells = Object.keys(files_with_smells).length;
        let countof_structures_with_smells = Object.keys(structures_with_smells).length;
        let countof_methods_with_smells = Object.keys(methods_with_smells).length;

        let countof_files = Util.get_countof_files(ST);
        let countof_structures = Util.get_countof_structures(ST);
        let countof_methods = Util.get_countof_methods(ST);

        let perc_of_smelly_files = "-------", perc_of_smelly_structures = "-------", perc_of_smelly_methods = "-------";
        if(countof_files !== 0)
            perc_of_smelly_files = `${Math.round(10000 * countof_files_with_smells / countof_files)/100} %`;
        if(countof_structures !== 0)
            perc_of_smelly_structures = `${Math.round(10000 * countof_structures_with_smells / countof_structures)/100} %`;
        if(countof_methods !== 0)
            perc_of_smelly_methods = `${Math.round(10000 * countof_methods_with_smells / countof_methods)/100} %`;


        let f_info = StatsRenderer.set_smells_in_buckets(files_with_smells);
        let s_info = StatsRenderer.set_smells_in_buckets(structures_with_smells);
        let m_info = StatsRenderer.set_smells_in_buckets(methods_with_smells);


        document.getElementById("stats_overview_sc").innerHTML = smells.length;
        document.getElementById("stats_overview_ti").innerHTML = total_intensity;
        document.getElementById("stats_overview_ai").innerHTML = avg_intensity;
        document.getElementById("stats_overview_mi").innerHTML = max_intensity;
        // document.getElementById("stats_overview_swn").innerHTML = smells_with_notes;
        document.getElementById("stats_overview_fws").innerHTML = countof_files_with_smells;
        document.getElementById("stats_overview_pofws").innerHTML = perc_of_smelly_files;
        document.getElementById("stats_overview_sws").innerHTML = countof_structures_with_smells;
        document.getElementById("stats_overview_posws").innerHTML = perc_of_smelly_structures;
        document.getElementById("stats_overview_mws").innerHTML = countof_methods_with_smells;
        document.getElementById("stats_overview_pomws").innerHTML = perc_of_smelly_methods;
        
        
        if(most_smells_of_a_type !== 0)
            document.getElementById("stats_overview_mcs").innerHTML = `${detector_with_most_smells} (${most_smells_of_a_type})`;
        else
            document.getElementById("stats_overview_mcs").innerHTML = "-------";


        if(f_info !== null){
            document.getElementById("stats_overview_fwhsi").innerHTML = `${Util.get_file_name_from_path(f_info.most_intensity.id)} (${Math.round(f_info.most_intensity.total_lvl*100)/100})`;
            document.getElementById("stats_overview_fwms").innerHTML = `${Util.get_file_name_from_path(f_info.most_smells.id)} (${f_info.most_smells.count})`;
        }
        else{
            document.getElementById("stats_overview_fwhsi").innerHTML = "-------";
            document.getElementById("stats_overview_fwms").innerHTML = "-------";
        }
        
        if(s_info !== null){
            document.getElementById("stats_overview_swhsi").innerHTML = `${s_info.most_intensity.id} (${Math.round(s_info.most_intensity.total_lvl*100)/100})`;
            document.getElementById("stats_overview_swms").innerHTML = `${s_info.most_smells.id} (${s_info.most_smells.count})`;
        }
        else{
            document.getElementById("stats_overview_swhsi").innerHTML = "-------";
            document.getElementById("stats_overview_swms").innerHTML = "-------";
        }

        if(m_info !== null){
            document.getElementById("stats_overview_mwhsi").innerHTML = `${m_info.most_intensity.id} (${Math.round(m_info.most_intensity.total_lvl*100)/100})`;
            document.getElementById("stats_overview_mwms").innerHTML = `${m_info.most_smells.id} (${m_info.most_smells.count})`;
        }
        else{
            document.getElementById("stats_overview_mwhsi").innerHTML = "-------";
            document.getElementById("stats_overview_mwms").innerHTML = "-------";
        }
        

        if(s_info !== null){
            this.compute_by_structure(s_info.rows);
        }
        else{
            console.log("TODO");
        }
        
        if(f_info !== null){
            this.compute_by_file(f_info.rows);
        }
        else {
            console.log("TODO");
        }
        
    }



    /**
     * 
     * @param smells_dict = {
     *      id0: {
     *          smell_lvls = [smell_lvl0, smell_lvl1, ...]
     *      },
     *      ...,
     *      ..
     * } 
     * 
     * @returns = {
     *      most_smells: {
     *          id: ...,
     *          count: ...    
     *      },
     *      most_intensity: {
     *          id: ...,
     *          total_lvl: ... 
     *      },
     *      rows: [
     *          [idK, smell_lvlX, smell_lvlZ, .., .., .., .., .., .., ..],
     *          ...,
     *          ..
     *      ]
     * }
     */
    static set_smells_in_buckets(smells_dict){
        if(Object.keys(smells_dict).length === 0)
            return null;


        let rows = [];
        let most_intensity = new Object();
        let most_smells = new Object();
        most_smells.id = "------";
        most_smells.count = 0;
        most_intensity.id = "------";
        most_intensity.total_lvl = 0;


        for (const [key, value] of Object.entries(smells_dict)){
            let smell_lvls = value.smell_lvls;
            let smell_lvl_buckets = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
            let smell_count_of_id = 0;
            let total_intensity_of_id = 0;
            
            for(const smell_lvl of smell_lvls){
                total_intensity_of_id += smell_lvl;
                smell_count_of_id++;
                let rounded = Math.round(smell_lvl);
                if(rounded === 10) //  (put in range 9-10: smell_lvl_buckets[9])
                    rounded = 9;
                    smell_lvl_buckets[rounded]++;
            }
            if(smell_count_of_id > most_smells.count){
                most_smells.count = smell_count_of_id;
                most_smells.id = key;
            }
            if(total_intensity_of_id > most_intensity.total_lvl){
                most_intensity.total_lvl = total_intensity_of_id;
                most_intensity.id = key;
            }
            smell_lvl_buckets[smell_lvl_buckets.length] = total_intensity_of_id; // append temporarily the total intensity (for sorting)
            let row = [key];
            row.push(...smell_lvl_buckets);
            rows.push(row);
        }

         // sort by total intensity
         rows.sort((row1, row2) => {
            if(row1[row1.length - 1] < row2[row2.length - 1])
                return 1;
            return -1;
        });

        // now remove the tmp sum.
        for(let row of rows)
            row.pop();


        return {
            most_smells: most_smells,
            most_intensity: most_intensity,
            rows: rows
        };
    }



    compute_by_structure(rows){
        this.by_class_div.innerHTML = "Loading Chart";

        if(google_charts_loaded){
            // let rows = [];
            // for (const [key, value] of Object.entries(structures_with_smells)){ 
            //     const structure_id = key;
            //     let smell_lvls = value.smell_lvls;
            //     let smell_lvl_ranges = [];
            //     for(let i=0; i<10; i++){
            //         smell_lvl_ranges[i] = 0;
            //     }
            //     let structure_smell_lvl = 0;
            //     for(const smell_lvl of smell_lvls){
            //         structure_smell_lvl += smell_lvl;
            //         let rounded = Math.round(smell_lvl);
            //         if(rounded === 10){ // put in range 9-10
            //             rounded = 9;
            //         }
            //         smell_lvl_ranges[rounded]++;
            //     }
            //     smell_lvl_ranges[smell_lvl_ranges.length] = structure_smell_lvl; // append temporarily the sum of smell_lvls
            //     let row = [structure_id];
            //     row.push(...smell_lvl_ranges);
            //     rows.push(row);
            // }

            // // sort by sum
            // rows.sort((row1, row2) => {
            //     if(row1[row1.length - 1] < row2[row2.length - 1]){
            //         return 1;
            //     }
            //     return -1;
            // });

            // // now remove the tmp sum.
            // for(let row of rows){
            //     row.pop();
            // }




            var data = google.visualization.arrayToDataTable([
                [   
                    {label: 'STR', type: 'string'},
                    {label: '0 - 1', type: 'number'},
                    {label: '1 - 2', type: 'number'},
                    {label: '2 - 3', type: 'number'},
                    {label: '3 - 4', type: 'number'},
                    {label: '4 - 5', type: 'number'},
                    {label: '5 - 6', type: 'number'},
                    {label: '6 - 7', type: 'number'},
                    {label: '7 - 8', type: 'number'},
                    {label: '8 - 9', type: 'number'},
                    {label: '9 - 10', type: 'number'},
                ]
            ]);

            data.addRows(rows);


            //rgba(255, ${green}, 0, 0.6)
            let col_rgb_colors = ['#fff200', '#ffd800', 
                '#ffbf00', '#ffa500', '#ff8c00', '#ff7200',
                '#ff5900', '#ff3f00', '#ff2600', '#ff0c00' 
            ];

            // Set chart options
            let options = {

                title: 'Smell Intensity Distribution by Structure',

                
                legend: { position: 'top', maxLines: 2 },
                //bar: { groupWidth: '75%' },
                isStacked: true,
                chartArea:{left:250,top:100, width: 1200, height: rows.length * 34,},
                width: 1400,
                height: rows.length * 40,
                colors: col_rgb_colors,
                backgroundColor: '#F6FFEF'
            };

            // Instantiate and draw our chart, passing in some options.
            let chart = new google.visualization.BarChart(this.by_class_div);
            
            let enclosing_was_hidden = false;
            let was_hidden = false;
            if(document.getElementById("smells_stats").style.display === "none"){
                enclosing_was_hidden = true;
                document.getElementById("smells_stats").style.display = "block";
            }
            if(this.by_class_div.style.display === "none"){
                was_hidden = true;
                this.by_class_div.style.display = "block";
            }    
            
            chart.draw(data, options);

            if(was_hidden){
                this.by_class_div.style.display = "none";
            }
            if(enclosing_was_hidden){
                document.getElementById("smells_stats").style.display = "none";
            }
            
        }
        else {
            var self = this;
            setTimeout(function() { self.compute_by_structure(rows); }, 900);
        }
    }

    compute_by_file(rows){
        this.by_file_div.innerHTML = "Loading Chart";

        if(google_charts_loaded){
            // let rows = [];
            // for (const [key, value] of Object.entries(files_with_smells)){ 
            //     const file_path = key;
            //     let smell_lvls = value.smell_lvls;
            //     let smell_lvl_ranges = [];
            //     for(let i=0; i<10; i++){
            //         smell_lvl_ranges[i] = 0;
            //     }
            //     let file_smell_lvl = 0;
            //     for(const smell_lvl of smell_lvls){
            //         file_smell_lvl += smell_lvl;
            //         let rounded = Math.round(smell_lvl);
            //         if(rounded === 10){ // put in range 9-10
            //             rounded = 9;
            //         }
            //         smell_lvl_ranges[rounded]++;
            //     }
            //     smell_lvl_ranges[smell_lvl_ranges.length] = file_smell_lvl; // append temporarily the sum of smell_lvls
            //     let row = [Util.get_file_name_from_path(file_path)];
            //     row.push(...smell_lvl_ranges);
            //     rows.push(row);
            // }

            // // sort by sum
            // rows.sort((row1, row2) => {
            //     if(row1[row1.length - 1] < row2[row2.length - 1]){
            //         return 1;
            //     }
            //     return -1;
            // });

            // // now remove the tmp sum.
            // for(let row of rows){
            //     row.pop();
            // }

            for(let row of rows){
                row[0] = Util.get_file_name_from_path(row[0]);
            }




            var data = google.visualization.arrayToDataTable([
                [   
                    {label: 'STR', type: 'string'},
                    {label: '0 - 1', type: 'number'},
                    {label: '1 - 2', type: 'number'},
                    {label: '2 - 3', type: 'number'},
                    {label: '3 - 4', type: 'number'},
                    {label: '4 - 5', type: 'number'},
                    {label: '5 - 6', type: 'number'},
                    {label: '6 - 7', type: 'number'},
                    {label: '7 - 8', type: 'number'},
                    {label: '8 - 9', type: 'number'},
                    {label: '9 - 10', type: 'number'},
                ]
            ]);

            data.addRows(rows);


            //color = rgb(255, ${green}, 0)
            // green0 = 255 - 25.5 * 0.5;
            // green1 = 255 - 25.5 * 1.5;
            //...
            // green9 = 255 - 25.5 * 9.5;
            let col_rgb_colors = ['#fff200', '#ffd800', 
                '#ffbf00', '#ffa500', '#ff8c00', '#ff7200',
                '#ff5900', '#ff3f00', '#ff2600', '#ff0c00' 
            ];

            // Set chart options
            let options = {

                title: 'Smell Intensity Distribution by File',

                
                legend: { position: 'top', maxLines: 2 },
                //bar: { groupWidth: '75%' },
                isStacked: true,
                chartArea:{left:250,top:100, width: 1200, height: rows.length * 34},
                width: 1400,
                height: rows.length * 40,
                colors: col_rgb_colors,
                backgroundColor: '#F6FFEF'
            };

            // Instantiate and draw our chart, passing in some options.
            let chart = new google.visualization.BarChart(this.by_file_div);

            let enclosing_was_hidden = false;
            let was_hidden = false;
            if(document.getElementById("smells_stats").style.display === "none"){
                enclosing_was_hidden = true;
                document.getElementById("smells_stats").style.display = "block";
            }
            if(this.by_file_div.style.display === "none"){
                was_hidden = true;
                this.by_file_div.style.display = "block";
            }    
            
            chart.draw(data, options);

            if(was_hidden){
                this.by_file_div.style.display = "none";
            }
            if(enclosing_was_hidden){
                document.getElementById("smells_stats").style.display = "none";
            }
        }
        else {
            var self = this;
            setTimeout(function() { self.compute_by_file(rows); }, 700);
        }
    }

    




};