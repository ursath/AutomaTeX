#ifndef TABLE_HEADER
#define TABLE_HEADER

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"


enum ValueType {
    STATES,
    ALPHABET,
    TRANSITIONS,    
    AUTOMATA
};

typedef union {
    TransitionSet * transitionSet;
    SymbolSet * symbolSet;
    StateSet * stateSet;
    Automata * automata;
} value;  

typedef NULL NO_RESULT;

typedef struct {
	Value value;
    ValueType type;
} Entry;


/**
 * @return Value que contiene ptr a lo buscado
 * es responsabilidad del cliente copiar el contenido del ptr
 */
Value getValue(char * identifier, ValueType type);

boolean exists(char * identifier, ValueType type );

boolean insert(char * identifier,  ValueType type, Value value );
