states O: {hola};
alphabet B : {buenas};
DFA AUTOMATA [
	states: {  r, O, j , >s , *w, *q },
    alphabet:  { a, B, {c, d}},
	transitions: {
        |q|-a->|r| ,  
        |s|-buenas->|{w}|
    }
];
