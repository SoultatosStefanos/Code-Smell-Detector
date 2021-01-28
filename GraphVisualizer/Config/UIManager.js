let UIManager = {}; 

// ----------------------------------------------------------------------------------

UIManager.evalCondition = function(condition) {
    let $currGroup = this.group.data;
    return eval(' ' + condition);
}

// ----------------------------------------------------------------------------------

UIManager.onChange = function(event, data) {
    this.confighandler.fire(event, data);
}


UIManager.onChangeSelect = function(event, data) {
    this.confighandler.fire(event + data);
}


UIManager.onClick = function(event, groupData, applyOnGroupData) {
    let data = {};

    function dataCreation(element, data, groupData) {
        if (typeof element === 'object' && element !== null) {
            let subData = {};
            Object.keys(element).forEach(key => {
                element[key].forEach(el => {
                    dataCreation(el, subData, groupData[key].data);
                    data[key] = subData;
                })
            });
        } else {
            data[element] = groupData[element].value;
        }
    }
    applyOnGroupData.forEach(element => {
        dataCreation(element, data, groupData);
    });
    this.confighandler.fire(event, data)
}

// ----------------------------------------------------------------------------------

UIManager.collapseCard = function(e) {
    const button = e.target;
    button.classList.toggle("active");
    var content = button.nextElementSibling;
    if (content.style.maxHeight) {
        content.style.maxHeight = null;
    } else {
        content.style.maxHeight = 4*content.scrollHeight + "px";
    }
}

// ----------------------------------------------------------------------------------

export default UIManager;