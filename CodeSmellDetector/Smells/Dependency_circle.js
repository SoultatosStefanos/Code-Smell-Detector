var Util = require("../Utility.js");

module.exports = {
    callback: async function(ST, args){
        var report = [];
        var msg;
        var src;
        var smell_level;
        var list = get_adjacency_list(ST.dependencies);
        var nodes = get_nodes(ST.dependencies);
        var g = new Graph(nodes, list);
        var circuits = g.findCircuits();
        for(const circuit of circuits){
            smell_level = Util.get_smell_lvl(args.max_circle_len.min, args.max_circle_len.max, circuit.length);
            if(smell_level > 0){
                var structure = ST.structures[circuit[0]];
                msg = `Structure: "${circuit[0]}" is part of this dependency cycle: `;
                for(var i=0; i<circuit.length - 1; i++){
                    msg += `${circuit[i]} -> `;
                }
                msg += `${circuit[circuit.length - 1]}\n`;
                src = Util.get_src_obj(structure.src_info.file, structure.src_info.line, structure.src_info.col, circuit[0]);
                report.push(Util.get_incident_obj(src, msg, smell_level));
            }
            

        }

        return report;
    }
}

function get_adjacency_list(dependencies){
    var list = [];
    for(const dep of dependencies){
        if(list[dep["from"]] === undefined) 
            list[dep["from"]] = [];
        list[dep["from"]].push(dep["to"]);
    }
    return list;
}

function get_nodes(dependencies){
    var nodes = new Set();
    for(const dep of dependencies){
        nodes.add(dep["from"]);
    }
    return Array.from(nodes);
}



function Graph(nodes, a_list) {
    this.arrows = new Map();
    this.nodes = nodes;
    for(const [from, to] of Object.entries(a_list)){
        this.arrows[from] = to;
    }
  }
  
  // Tarjan's strongly connected components algorithm
  Graph.prototype.stronglyConnectedComponents = function tarjan() {
    var index = 0;
    var stack = [];
    var result = [];
    var meta = new Map();
  
    var graph = this;
  
    var connect = function connect(node) {
      var entry = {
        onStack: true,
        index: index,
        lowLink: index
      };
  
      meta.set(node, entry);
      stack.push(node);
      index += 1;
  
      graph.arrows.get(node).forEach(function(adj) {
        if (!meta.has(adj)) {
          // adjacent node has not yet been visited, do it
          connect(adj);
          entry.lowLink = Math.min(entry.lowLink, meta.get(adj).lowLink);
        } else if (meta.get(adj).onStack) {
          entry.lowLink = Math.min(entry.lowLink, meta.get(adj).index);
        }
      });
  
      // check if node is a root node, pop the stack and generated an SCC
      if (entry.lowLink === entry.index) {
        var scc = [];
        var adj = null;
  
        do {
          adj = stack.pop();
          meta.get(adj).onStack = false;
          scc.push(adj);
        } while (node !== adj);
  
        result.push(scc);
      }
    };
  
    graph.nodes.forEach(function(node) {
      if (!meta.has(node)) {
        connect(node);
      }
    });
  
    return result;
  };
  
  // Based on Donald B. Johnson 1975 paper
  // Finding all the elementary circuits of a directed graph
  Graph.prototype.findCircuits = function findCircuits() {
    var startNode;
    var stack = [];
    var circuits = [];
    var blocked = new Map();
  
    // book keeping to prevent Tarjan's algorithm fruitless searches
    var b = new Map();
  
    var graph = this;
  
    function addCircuit(start, stack) {
      var orders = [start.order].concat(
        stack.map(function(n) {
          return n.order;
        })
      );
  
      // prevent duplicated cycles
      // TODO: figure out why this is needed, this is most likely related to
      // startNode being the "least" vertex in Vk
      if (Math.min.apply(null, orders) !== start.order) {
        circuits.push([].concat(stack).concat(start));
      }
    }
  
    function unblock(u) {
      blocked.set(u, false);
  
      if (b.has(u)) {
        b.get(u).forEach(function(w) {
          b.get(u).delete(w);
          if (blocked.get(w)) {
            unblock(w);
          }
        });
      }
    }
  
    function circuit(node) {
      var found = false;
  
      stack.push(node);
      blocked.set(node, true);
        var node_list = graph.arrows[node];
        if(node_list){
            for(var _node of node_list){
                if (_node === startNode) {
                    found = true;
                    addCircuit(startNode, stack);
                } else if (!blocked.get(_node)) {
                    if (circuit(_node)) {
                    found = true;
                    }
                }
            }
        }
        
    //   graph.arrows.get(node).forEach(function(w) {
    //     if (w === startNode) {
    //       found = true;
    //       addCircuit(startNode, stack);
    //     } else if (!blocked.get(w)) {
    //       if (circuit(w)) {
    //         found = true;
    //       }
    //     }
    //   });
  
      if (found) {
        unblock(node);
      } else {
        node_list = graph.arrows[node];
        if(node_list){
            for(var _node of node_list){
                var entry = b.get(_node);
                if (!entry) {
                    entry = new Set();
                    b.set(_node, entry);
                }
              entry.add(node);
            }
        }
        
        // graph.arrows.get(node).forEach(function(w) {
        //   var entry = b.get(w);
  
        //   if (!entry) {
        //     entry = new Set();
        //     b.set(w, entry);
        //   }
  
        //   entry.add(node);
        // });
      }
  
      stack.pop();
      return found;
    }
  
    graph.nodes.forEach(function(node) {
      startNode = node;
      //console.log(graph.arrows[node]);
      //console.log(graph.arrows);
      var node_list = graph.arrows[node];
      for(const _node of node_list){
          circuit(_node);
      }
      //graph.arrows[node].forEach(circuit);
    });
  
    return circuits;
  };
