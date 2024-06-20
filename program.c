states O: {hola};
alphabet B : {buenas};
NFA AUTOMATA [
	states: {  r, s, j , >s , *w, *q },
    alphabet:  { a,@, B, {c, d}},
	transitions: {
        |q|-a->|r| ,  
        |s|-buenas->|{w}|
    }
];

