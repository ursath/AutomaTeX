DFA AUTOMATA [
	states: {  r, o, j , >s , *w, *q },
	alphabet: { a, b},
	transitions: {
        |q|-a->|r| ,  
        |w|<-a->|r|,      
        |s|-a->|w|
    }
];

transitions T: {
        |q|-a->|r| ,  
        |w|<-b->|r|,      
        |s|-c->|w|
    };

alphabet Q: AUTOMATA.alphabet;
alphabet S: {s,m, a};
//states SS: {*q, w} ;
DFA AUTOMATA2 [
	states: {  r, o, j , >s , w, AUTOMATA.states.final },
	alphabet: {Q, c},
	transitions: T
];
