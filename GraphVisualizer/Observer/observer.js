function Subject() {
    this.observers = {};
};

Subject.prototype = {
    install: function (event, fn) {
        this.observers[event] = fn;
    },
    uninstall: (event) => {
        delete this.observers[event];
    },
    fire: function(event, ...data) {
        if(this.observers[event] !== undefined)
            this.observers[event](...data);
        else 
        console.log('None event handler is defined for "' + event  + '" event.');
    }
}

export const obs = new Subject(); 
