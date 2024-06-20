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
states Q: AUTOMATA.states;

NFA AUTOMATA2 [
	states:  {  r, o, j , >s , *w, *q },
	alphabet: M ,
	transitions: {
        |q|-a->|r| ,  
        |w|<-b->|r|,      
        |s|-c->|{w,q}|
    }
];