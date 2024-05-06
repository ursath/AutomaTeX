#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../shared/Logger.h"
#include <stdlib.h>
#include "../../shared/Type.h"

/** Initialize module's internal state. */
void initializeAbstractSyntaxTreeModule();

/** Shutdown module's internal state. */
void shutdownAbstractSyntaxTreeModule();

/**
 * This typedefs allows self-referencing types.
 */

typedef enum ExpressionType ExpressionType;
typedef enum AutomataType AutomataType;
typedef enum SetType SetType;

typedef struct Expression Expression;
typedef struct Program Program;
typedef struct Symbol Symbol;
typedef struct Set Set;
typedef struct State State;
typedef struct Automata Automata;
typedef struct Transition Transition;
typedef struct SymbolSet SymbolSet;
typedef struct StateSet StateSet;
typedef struct TransitionSet TransitionSet;
typedef struct Definition Definition;
typedef struct SymbolNode SymbolNode;
typedef struct StateNode StateNode;
typedef struct TransitionNode TransitionNode;
typedef struct TransitionExpression TransitionExpression;
typedef struct SymbolExpression SymbolExpression;
typedef struct StateExpression StateExpression;

/**
 * Node types for the Abstract Syntax Tree (AST).
 */

/* ------------------------------------------------- ENUMS ------------------------------------------------- */

enum SetType {
	TRANSITION,
	ALPHABET,
	STATE
};

enum ExpressionType {
	UNION,
	INTERSECTION,
	DIFFERENCE,
	SET
};

enum AutomataType {
	DFA,
	NFA,
	LNFA
};


/* ------------------------------------------------- DEFINITION ------------------------------------------------- */

struct Definition
{
	union {
		Automata * automata;
		StateSet * stateSet;
		SymbolSet * symbolSet;
		TransitionSet * transitionSet;
	};
};


/* ------------------------------------------------- AUTOMATA ------------------------------------------------- */

struct Automata {
	char * identifier;
	StateSet * states;
	StateSet * finals;
	StateSet * initials;
	SymbolSet * alphabet;
	TransitionSet * transitions; 
	AutomataType automataType;
};


/* ------------------------------------------------- EXPRESSIONS ------------------------------------------------- */

struct Expression {
	union {
		Set * set; 
		struct {
			Expression * rightOperation;
			Expression * leftOperation;
		};
	};
	ExpressionType type;
};

struct TransitionExpression {
	union {
		TransitionSet * transitionSet;
		struct {
			TransitionExpression * leftExpression;
			TransitionExpression * rightExpression;
		};
	};
	ExpressionType type;
};

struct SymbolExpression {
	union {
		SymbolSet * symbolSet;
		struct {
			SymbolExpression * leftExpression;
			SymbolExpression * rightExpression;
		};
	};
	ExpressionType type;
};

struct StateExpression {
	union {
		StateSet * stateSet;
		struct {
			StateExpression * leftExpression;
			StateExpression * rightExpression;
		};
	};
	ExpressionType type;
};


/* ------------------------------------------------- SETS ------------------------------------------------- */

struct Set {
	union {
		TransitionSet * transitionSet;
		SymbolSet * symbolSet;
		StateSet * stateSet;
	};
	SetType type;
};

struct SymbolSet {
	SymbolNode * alphabet;				// ! TODO: no conviene poner first? así queda + claro qué es
	SymbolNode * tail;
	char * identifier;
};

struct StateSet {
	 StateNode * states;				// ! TODO: no conviene poner first? así queda + claro qué es
	 StateNode * tail;
	 char * identifier;
	/* 
	 struct {
		State * finals;
		State * initial;
		State * regulars;
	}*/
};

struct TransitionSet {
	TransitionNode * transitions;		// ! TODO: no conviene poner first? así queda + claro qué es
	TransitionNode * tail;
	char * identifier;
};

/*
struct Set {
    union {
        // state 
        State * states;
	
        // alphabet 
        Symbol * alphabet;

        // transition 
        Transition * transitions;
    }
    SetType type; 
};
*/

/* ------------------------------------------------- NODES ------------------------------------------------- */
//los nodos podrían tener un union que sirva para poder incluir subconjuntos (sino siempre se ignoran y se obtiene un conjunto con elementos sueltos)
//además con esto se evita que el primer caso de cada action de los sets sea una función void
struct SymbolNode {
	Symbol symbol;
	SymbolNode * next;
};

struct StateNode {
	State state;
	StateNode * next;
};

struct TransitionNode {
	Transition transition;
	TransitionNode * next;
};


/* ------------------------------------------------- ELEMENTS ------------------------------------------------- */

struct Symbol {
	char * value;
};	

struct State {
	Symbol symbol;
	boolean isFinal;
	boolean isInitial;
};

//consultar
struct Transition {
		StateSet * fromSet;
		SymbolSet * symbolSet;
		StateSet * toSet;
};


/* ------------------------------------------------- PROGRAM ------------------------------------------------- */

struct Program {
	Definition * definition;
};


/**
 * Node recursive destructors.
 */
void releaseExpression(Expression * expression);
void releaseProgram(Program * program);

#endif
