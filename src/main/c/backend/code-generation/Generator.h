#ifndef GENERATOR_HEADER
#define GENERATOR_HEADER

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../shared/CompilerState.h"
#include "../../shared/Logger.h"
#include "../../shared/String.h"
#include <stdarg.h>
#include <stdio.h>

typedef struct TransitionMatrixCell TransitionMatrixCell;
typedef struct AutomataMatrixCell AutomataMatrixCell;
typedef struct SymbolMatrixNode SymbolMatrixNode;
typedef struct MatrixNode MatrixNode;

struct MatrixNode {
    State * state;
    MatrixNode * next;
};

struct TransitionMatrixCell {
    MatrixNode * first;
    MatrixNode * tail;
};

struct SymbolMatrixNode {
    Symbol * symbol;
    SymbolMatrixNode * next;
};

struct AutomataMatrixCell {
    SymbolMatrixNode * first;
    SymbolMatrixNode * tail;
};


/** Initialize module's internal state. */
void initializeGeneratorModule();

/** Shutdown module's internal state. */
void shutdownGeneratorModule();

/**
 * Generates the final output using the current compiler state.
 */
void generate(CompilerState * compilerState);

#endif
