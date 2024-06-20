#ifndef UTILS_HEADER
#define UTILS_HEADER


#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"

/*----------------------------------------- SET CONTAINS ----------------------------------------------------*/ 
boolean containsState(StateNode *first, State * state );
boolean containsSymbol(SymbolNode *first, Symbol * symbol );

/*----------------------------------------- COPY SET ----------------------------------------------------*/ 
SymbolSet * cpySymbolSet(SymbolSet * symbolSet);
StateSet * cpyStateSet(StateSet * StateSet);
TransitionSet * cpyTransitionSet(TransitionSet * StateSet);


/*-------------------------------------- FREE NODE SETS --------------------*/
void freeTransitionSet(TransitionSet * set);
void freeStateSet(StateSet * set);
void freeSymbolSet(SymbolSet * set);

/*----------------------------------------- SET EQUALS ----------------------------------------------------*/ 
boolean transitionSetEquals(TransitionSet * set1, TransitionSet * set2);
boolean stateSetEquals(StateSet * set1, StateSet * set2);
boolean symbolSetEquals(SymbolSet *set1, SymbolSet *set2);
boolean transitionEquals(Transition * trans1, Transition * trans2);
boolean stateEquals(State * state1, State * state2);
boolean stateSymbolEquals(State * state1, State * state2);      // no me interesa si el state es final o initial
boolean symbolEquals(Symbol * symbol1, Symbol * symbol2);

#endif