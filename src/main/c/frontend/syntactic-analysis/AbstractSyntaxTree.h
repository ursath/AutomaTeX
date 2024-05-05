#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../shared/Logger.h"
#include <stdlib.h>

/** Initialize module's internal state. */
void initializeAbstractSyntaxTreeModule();

/** Shutdown module's internal state. */
void shutdownAbstractSyntaxTreeModule();

/**
 * This typedefs allows self-referencing types.
 */

typedef enum ExpressionType ExpressionType;
typedef enum FactorType FactorType;

typedef struct Constant Constant;
typedef struct Expression Expression;
typedef struct Factor Factor;
typedef struct Program Program;
typedef struct Symbol Symbol;
typedef struct Set Set;
typedef struct State State;
typedef struct Automata Automata;
typedef struct Transition Transition;
typedef struct SymbolSet 
typedef struct StateSet 
typedef struct TransitionSet 

/**
 * Node types for the Abstract Syntax Tree (AST).
 */

enum ExpressionType {
	ADDITION,
	DIVISION,
	FACTOR,
	MULTIPLICATION,
	SUBTRACTION
};

enum FactorType {
	CONSTANT,
	EXPRESSION
};

struct Constant {
	int value;
};

struct Factor {
	union {
		Constant * constant;
		Expression * expression;
	};
	FactorType type;
};

struct Expression {
	union {
		Set * factor;
		struct {
			Expression * leftExpression;
			Expression * rightExpression;
		};
	};
	ExpressionType type;
};

struct Program {
	Expression * expression;
};

struct Symbol {
	char * name;
};

struct State{
	char * name;
	char * type;
};

enum ExpressionType {
	UNION,
	INTERSECTION,
	DIFFERENCE,
	SYMBOL
};

struct Expression {
	union {
		Set * set; 
		struct {
			Expression * rightOperation;
			Expression * leftOperation;
		}
	}
	ExpressionType type;
};

struct Definition
{
	union {
		Automata * automata;
		Set * set; 
	}
};



/*
enum SetType {
    TRANSITION,
    ALPHABET,
    STATE
};

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

struct SymbolSet {
	Symbol * alphabet;
	char * name;
};

struct StateSet {
	 State * states;
	 char * name;
	/* 
	 struct {
		State * finals;
		State * initial;
		State * regulars;
	}*/
};

struct TransitionSet {
	Transition * transitions;
	char * name;
};

//consultar
struct Transition{
		StateSet * fromSet;
		SymbolSet * symbolSet;
		StateSet * toSet;
}

enum AutomataType {
	DFA,
	NFA,
	LNFA
};


struct Automata {
	StateSet * states;
	StateSet * finals;
	StateSet * initials;
	SymbolSet * alphabet;
	TransitionSet * transitions; 
	AutomataType automataType;
};


/**
 * Node recursive destructors.
 */
void releaseConstant(Constant * constant);
void releaseExpression(Expression * expression);
void releaseFactor(Factor * factor);
void releaseProgram(Program * program);

#endif
