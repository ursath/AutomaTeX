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
	ELEMENT_EXPRESSION
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
	MIXED,
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

struct TransitionExpression {
	union {
		TransitionSet * transitionSet;
		struct {
			TransitionExpression * leftExpression;
			TransitionExpression * rightExpression;
		};
		Transition * transition;
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
		Symbol * symbol;
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
		State * state;
	};
	ExpressionType type;
};


/* ------------------------------------------------- SETS ------------------------------------------------- */

struct SymbolSet {
	SymbolNode * first;
	SymbolNode * tail;
	boolean isFromAutomata;
	char * identifier;
};

struct StateSet {
	StateNode * first;
	StateNode * tail;
	char * identifier;
	boolean isFromAutomata;
	StateType stateType;
};

struct TransitionSet {
	TransitionNode * first;	
	TransitionNode * tail;
	char * identifier;
	boolean isFromAutomata;
};


/* ------------------------------------------------- NODES ------------------------------------------------- */

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

struct Transition {
		StateExpression * fromExpression;
		SymbolExpression * symbolExpression;
		StateExpression * toExpression;
};


/* ------------------------------------------------- PROGRAM ------------------------------------------------- */

struct Program {
	DefinitionSet * definitionSet;
};


/**
 * Node recursive destructors.
 */
//void releaseExpression(Expression * expression);
//void releaseProgram(Program * program);

#endif
