states Q: {*r, o};

NFA Automata [
    states: { w, >q, Q.final },    // Q.final = {*r}  
    alphabet: {a, b},
    transitions: {
        |q|-a->|r| ,  
        |o|<-b->|r|      
        }
];

alphabet A: Automata.alphabet;       // A = {a, b}
states S: Automata.states;           
states I: Automata.states.initial;   