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

typedef enum DefinitionType DefinitionType;
typedef enum ExpressionType ExpressionType;
typedef enum AutomataType AutomataType;
typedef enum SetType SetType;
typedef enum NodeType NodeType;
typedef enum StateType StateType;

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
typedef struct DefinitionSet DefinitionSet;
typedef struct DefinitionNode DefinitionNode;

/**
 * Node types for the Abstract Syntax Tree (AST).
 */


/* ------------------------------------------------- ENUMS ------------------------------------------------- */
enum DefinitionType {
	AUTOMATA_DEFINITION,
	TRANSITION_DEFINITION,
	ALPHABET_DEFINITION,
	STATE_DEFINITION
};

enum SetType {
	TRANSITIONSET,
	ALPHABETSET,
	STATESET
};

enum ExpressionType {
	UNION_EXPRESSION,
	INTERSECTION_EXPRESSION,
	DIFFERENCE_EXPRESSION,
	SET_EXPRESSION,
	VOID_EXPRESSION
};

enum NodeType {
	EXPRESSION,
	ELEMENT
};

enum AutomataType {
	DFA_AUTOMATA,
	NFA_AUTOMATA,
	LNFA_AUTOMATA
};

enum StateType {
	FINAL,
	INITIAL,
	REGULAR,
	MIXED
};

/* ----------------------------------------------- DEFINITION SET ----------------------------------------------- */

struct DefinitionSet {
	DefinitionNode * first;
	DefinitionNode * tail;
};

struct DefinitionNode {
	Definition * definition;
	DefinitionNode * next;
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
	DefinitionType type; 
};


/* ------------------------------------------------- AUTOMATA ------------------------------------------------- */

struct Automata {
	char * identifier;
	StateExpression* states;
	StateExpression* finals;
	StateExpression* initials;
	SymbolExpression* alphabet;
	TransitionExpression* transitions; 
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

struct SymbolSet {
	SymbolNode * first;
	SymbolNode * tail;
	char * identifier;
};

struct StateSet {
	StateNode * first;
	StateNode * tail;
	char * identifier;
	StateType stateType;
	
	/* 
	 struct {
		State * finals;
		State * initial;
		State * regulars;
	}*/
};

struct TransitionSet {
	TransitionNode * first;	
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
	union {
		Symbol * symbol;
		SymbolExpression * symbolExpression;
		SymbolSet * symbolSubset;
	};
	NodeType type;
	SymbolNode * next;
};

struct StateNode {
	union {
		State * state;
		StateExpression * stateExpression;
		StateSet * stateSubset;
	};
	NodeType type;
	StateNode * next;
};

struct TransitionNode {
	union {
		Transition * transition;
		TransitionExpression * transitionExpression;
		TransitionSet * transitionSubset;
	};
	NodeType type;
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
		StateExpression * fromExpression;
		SymbolExpression * symbolExpression;
		StateExpression * toExpression;
};


/* ------------------------------------------------- PROGRAM ------------------------------------------------- */

struct Program {
	//DefinitionSet * definitionSet;
	Definition * definition;
};


/**
 * Node recursive destructors.
 */
//void releaseExpression(Expression * expression);
//void releaseProgram(Program * program);

#endif
