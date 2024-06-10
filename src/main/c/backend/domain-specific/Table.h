#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
// Idea 
//getValue(char * identifier, type?) DEV COPIA 
//boolean exists( id)
//insert( id, value)

enum ValueType {
    STATES,
    ALPHABET,
    TRANSITIONS,    
    AUTOMATA_STATES,
    AUTOMATA_ALPHABET,
    AUTOMATA_TRANSITIONS
};


typedef struct {
	union {
        TransitionSet * transitionSet;
        SymbolSet * symbolSet;
        StateSet * stateSet;
    } 

    ValueType type;
} EntryResult;


// Si quiero Automata.states = getValue( "Automata", STATES_DEFINITION) NOOO xq puede q = nom pra un conj de estados suelto 
/**
 * @return EntryResult contiene copia del valor buscado
 */
EntryResult getValue(char * identifier, ValueType type);

void insert(char * identifier,  ValueType type);

//todo: cpySet() 
