#ifndef TABLE_HEADER
#define TABLE_HEADER

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"


typedef enum {
    STATES,
    ALPHABET,
    TRANSITIONS,    
    AUTOMATA
} ValueType;

typedef union {
    TransitionSet * transitionSet;
    SymbolSet * symbolSet;
    StateSet * stateSet;
    Automata * automata;
} Value;  

typedef struct{
    Value value;
    boolean found;
} EntryResult;

void initializeTable(void);

/**
 * @return Value que contiene ptr a lo buscado
 * es responsabilidad del cliente copiar el contenido del ptr
 */
EntryResult getValue(char * identifier, ValueType type);

//boolean exists(char * identifier, ValueType type );

boolean insert(char * identifier,  ValueType type, Value value );

#endif      
