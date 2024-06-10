#ifndef AUTOMATEX_HEADER
#define AUTOMATEX_HEADER

/**
 * We reuse the types from the AST for convenience, but you should separate
 * the layers of the backend and frontend using another group of
 * domain-specific models or DTOs (Data Transfer Objects).
 */
#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../shared/Logger.h"
#include "../../shared/Type.h"
#include <limits.h>
#include "Table.h"

/** Initialize module's internal state. */
void initializeAutomatexModule();

/** Shutdown module's internal state. */
void shutdownAutomatexModule();

typedef struct {
	boolean succeed;
	union {
        DefinitionSet * definitionSet;
        Automata * automata;
        TransitionSet * transitionSet;
        SymbolSet * symbolSet;
        StateSet * stateSet;
        Transition * transition;
        Symbol * symbol;
        State * state;
    } 
    boolean isDefinitionSet;
    DefinitionType type;
} ComputationResult;

ComputationResult computeDefinitionSet(DefinitionSet * definitionSet);
ComputationResult computeDefinition(Definition * definition);
ComputationResult computeAutomata(Automata * automata);
ComputationResult computeTransitionExpression(TransitionExpression * expression);
ComputationResult computeStateExpression(StateExpression * expression);
ComputationResult computeSymbolExpression(SymbolExpression * expression);
ComputationResult computeTransitionSet(TransitionSet * set);
ComputationResult computeStateSet(StateSet * set);
ComputationResult computeSymbolSet(SymbolSet * set);

ComputationResult computeTransition(Transition * transition);
ComputationResult computeState(State * state);

#endif