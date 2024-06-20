NFA AUTOMATA [
	states: {  r, o, j , >s , *w, *q },
	alphabet: { u, a, b, c },
	transitions: {
        |q|-a->|r| ,  
        |w|<-b->|r|,      
        |s|-c->|{w,q}|
    }
];
alphabet R: { u, a, b, c };
alphabet M: R;
states Q: {  r, o, j , >s , *w, *q };
states Q1: AUTOMATA.states.regular;

NFA AUTOMATA2 [
	states:  Q1,
	alphabet: {M} ,
	transitions: {
        |q|-a->|r| ,  
        |w|<-b->|r|,      
        |s|-c->|{w,q}|
    }
];