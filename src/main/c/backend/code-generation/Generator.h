#ifndef GENERATOR_HEADER
#define GENERATOR_HEADER

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../shared/CompilerState.h"
#include "../../shared/Logger.h"
#include "../../shared/String.h"
#include <stdarg.h>
#include <stdio.h>

typedef struct MatrixNode {
    State * state;
    MatrixNode * next;
} MatrixNode;

typedef struct TransitionMatrixCell {
    MatrixNode * first;
    MatrixNode * tail;
} TransitionMatrixCell;

typedef struct SymbolMatrixNode {
    Symbol * symbol;
    SymbolMatrixNode * next;
} SymbolMatrixNode;

typedef struct AutomataMatrixCell {
    SymbolMatrixNode * first;
    SymbolMatrixNode * tail;
} AutomataMatrixCell;


/** Initialize module's internal state. */
void initializeGeneratorModule();

/** Shutdown module's internal state. */
void shutdownGeneratorModule();

/**
 * Generates the final output using the current compiler state.
 */
void generate(CompilerState * compilerState);

#endif
