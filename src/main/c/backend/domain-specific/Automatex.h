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

#include "utils.h"
#include "Table.h"


/* LOGGER STR */
#define AUTOMATA_NOT_CREATED "The automata cannot be created because"

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
    };
    boolean isDefinitionSet;
    boolean isSingleElement;
    DefinitionType type;
} ComputationResult;

ComputationResult computeDefinitionSet(DefinitionSet * definitionSet);
ComputationResult computeDefinition(Definition * definition);
ComputationResult computeAutomata(Automata * automata);
ComputationResult computeTransitionExpression(TransitionExpression * expression, boolean isSingleElement );
ComputationResult computeStateExpression(StateExpression * expression, boolean isSingleElement);
ComputationResult computeSymbolExpression(SymbolExpression * expression, boolean isSingleElement);
ComputationResult computeTransitionSet(TransitionSet * set, boolean isDefinition);
ComputationResult computeStateSet(StateSet * set, boolean isDefinition);
ComputationResult computeSymbolSet(SymbolSet * set, boolean isDefinition);

ComputationResult computeTransition(Transition * transition, boolean isSingleElement);
ComputationResult computeState(State * state, boolean isSingleElement);
ComputationResult computeSymbol(Symbol* symbol, boolean isSingleElement);


#endif