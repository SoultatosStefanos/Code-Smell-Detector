const assert = require('assert');

module.exports = class StatsRenderer{
    constructor(overview_div, by_class_div, by_file_div){
        this.overview_div = overview_div;
        this.by_class_div = by_class_div;
        this.by_file_div = by_file_div;
    }

    hide_all(){
        this.overview_div.style.display = "none";
        this.by_class_div.style.display = "none";
        this.by_file_div.style.display = "none";
    }

    clear_all(){
        document.getElementById("stats_overview_sc").innerHTML = "";
        document.getElementById("stats_overview_ti").innerHTML = "";
        document.getElementById("stats_overview_ai").innerHTML = "";
        document.getElementById("stats_overview_fws").innerHTML = "";
        document.getElementById("stats_overview_sws").innerHTML = "";
        this.by_class_div.innerHTML = "";
        this.by_file_div.innerHTML = "";
    }

    async compute_stats(smells){
        this.clear_all();
        this.hide_all();
    
        let total_intensity = 0;
        let avg_intensity;
        let structures_with_smells = new Object();
        let files_with_smells = new Object();


        for(const smell of smells){
            total_intensity += smell.lvl;
            if(smell.src.struct !== null){
                if(structures_with_smells[smell.src.struct] === undefined)
                    structures_with_smells[smell.src.struct] = [smell.lvl];
                else
                    structures_with_smells[smell.src.struct].push(smell.lvl);
            }
            if(files_with_smells[smell.src.file] === undefined)
                files_with_smells[smell.src.file] = [smell.lvl];
            else
                files_with_smells[smell.src.file].push(smell.lvl);

        }
        avg_intensity = total_intensity / smells.length;

        avg_intensity = Math.round(avg_intensity*100) / 100;
        total_intensity = Math.round(total_intensity*100) / 100;


        document.getElementById("stats_overview_sc").innerHTML = smells.length;
        document.getElementById("stats_overview_ti").innerHTML = total_intensity;
        document.getElementById("stats_overview_ai").innerHTML = avg_intensity;
        document.getElementById("stats_overview_fws").innerHTML = Object.keys(files_with_smells).length;
        document.getElementById("stats_overview_sws").innerHTML = Object.keys(structures_with_smells).length;

                
        Promise.all([this.compute_by_structure(smells, structures_with_smells), this.compute_by_file(smells, files_with_smells)]);

    }



    async compute_by_structure(smells, structures_with_smells){
        // Create the data table.
        if(google_charts_loaded){
            let data = new google.visualization.DataTable();


            // data.addColumn('string', 'structure', { role: 'annotation' });
            // data.addColumn('number', '0-1', { role: 'annotation' });
            // data.addColumn('number', '1-2');
            // data.addColumn('number', '2-3');
            // data.addColumn('number', '3-4');
            // data.addColumn('number', '4-5');
            // data.addColumn('number', '5-6');
            // data.addColumn('number', '6-7');
            // data.addColumn('number', '7-8');
            // data.addColumn('number', '8-9');
            // data.addColumn('number', '9-10');

            
            data.addColumn({type: 'string', label: 'structure', role: 'domain'});
            data.addColumn({type: 'number', label: 'reeeeeeee'});
            data.addColumn({type: 'number', label: '1-2', id: 'xD'});
            data.addColumn({type: 'number', label: '2-3'});
            data.addColumn({type: 'number', label: '3-4'});
            data.addColumn({type: 'number', label: '4-5'});
            data.addColumn({type: 'number', label: '5-6'});
            data.addColumn({type: 'number', label: '6-7'});
            data.addColumn({type: 'number', label: '7-8'});
            data.addColumn({type: 'number', label: '8-9'});
            data.addColumn({type: 'number', label: '9-10'});

            

            data.addRows([
              ['class0', 0, 4, 2, 4, 4, 3, 0, 0, 1, 2],
              ['class1', 1, 1, 2, 5, 3, 0, 2, 0, 3, 1],
              ['class2', 0, 3, 2, 2, 4, 3, 0, 5, 0, 1],
              ['class3', 0, 4, 3, 2, 1, 3, 0, 2, 0, 4],
              ['class4', 0, 5, 1, 1, 1, 2, 4, 0, 3, 2]
            ]);


            //rgba(255, ${green}, 0, 0.6)
            let col_rgb_colors = ['#fff200', '#ffd800', 
                '#ffbf00', '#ffa500', '#ff8c00', '#ff7200',
                '#ff5900', '#ff3f00', '#ff2600', '#ff0c00' 
            ];

            // Set chart options
            let options = {
                title: 'Smell Intensity Distribution by Structure',
                width: 1200,
                //height: 400,
                legend: { position: 'top', maxLines: 2 },
                bar: { groupWidth: '75%' },
                isStacked: true,
                colors: col_rgb_colors
            };

            // Instantiate and draw our chart, passing in some options.
            let chart_div = document.getElementById('stats_by_class');
            let chart = new google.visualization.BarChart(chart_div);         
            
            chart.draw(data, options);
        }
        else {
            this.by_class_div.innerHTML = "Loading Chart";
            var self = this;
            setTimeout(function() { self.compute_by_structure(smells, structures_with_smells); }, 1000);
        }
    }

    async compute_by_file(smells, files_with_smells){
        // Create the data table.
        if(google_charts_loaded){
            var data = new google.visualization.DataTable();
            data.addColumn('string', 'Topping');
            data.addColumn('number', 'Slices');
            data.addRows([
            ['Mushrooms', 3],
            ['Onions', 1],
            ['Olives', 1],
            ['Zucchini', 1],
            ['Pepperoni', 2]
            ]);

            // Set chart options
            var options = {'title':'How Much Pizza I Ate Last Night',
                        'width':400,
                        'height':300};

            // Instantiate and draw our chart, passing in some options.
            var chart = new google.visualization.PieChart(document.getElementById('stats_by_file'));
            chart.draw(data, options);
        }
        else {
            this.by_file_div.innerHTML = "Loading Chart";
            var self = this;
            setTimeout(function() { self.compute_by_file(smells, files_with_smells); }, 1000);
        }
    }

    




};