states O: {hola, buenas, *buenas2};
alphabet B : {buenas, buenas2, buen};
DFA AUTOMATA [
	states: {  r, {O}, j , >s , *w, *q },
    alphabet:  { a, B, {c, d}},
	transitions: {
        |q|-a->|r| ,  
        |s|-buenas->|{w}|
    }
];
