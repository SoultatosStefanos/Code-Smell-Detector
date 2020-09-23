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
        this.observers[event](...data);
    }
}

export const obs = new Subject(); 
