#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../shared/CompilerState.h"
#include "../../shared/Logger.h"
#include "../../shared/Type.h"
#include "AbstractSyntaxTree.h"
#include "SyntacticAnalyzer.h"
#include <stdlib.h>

#define LAMBDA "@"

/** Initialize module's internal state. */
void initializeBisonActionsModule();

/** Shutdown module's internal state. */
void shutdownBisonActionsModule();

/**
 * Bison semantic actions.
 */


/* Definitions */
Definition * AutomataDefinitionSemanticAction(AutomataType type, char * identifier, Automata * automata );
Definition * SingularTransitionSetDefinitionSemanticAction(char * identifier, Transition * transition);
Definition * TransitionSetDefinitionSemanticAction(char * identifier, TransitionSet * set);
Definition * SingularSymbolSetDefinitionSemanticAction(char * identifier, Symbol* symbol);
Definition * SymbolSetDefinitionSemanticAction(char * identifier, SymbolSet * set);
Definition * SingularStateSetDefinitionSemanticAction(char * identifier, State* state);
Definition * StateSetDefinitionSemanticAction(char * identifier, StateSet * set);

Automata * AutomataSemanticAction(StateSet * states, SymbolSet * alphabet, TransitionSet * transitions);
AutomataType AutomataTypeAction(AutomataType type);

TransitionExpression * ArithmeticTransitionExpressionSemanticAction(TransitionExpression * leftExpression, TransitionExpression * rightExpression, ExpressionType type);
TransitionExpression * ArithmeticTransitionSetSemanticAction(TransitionSet * transitionSet);
TransitionExpression * ArithmeticSingularSymbolSetSemanticAction(Transition * transition);
SymbolExpression * ArithmeticSymbolExpressionSemanticAction(SymbolExpression * leftExpression, SymbolExpression * rightExpression, ExpressionType type);
SymbolExpression * ArithmeticSymbolSetSemanticAction(SymbolSet * symbolSet);
SymbolExpression * ArithmeticSingularSymbolSetSemanticAction(Symbol * symbol);
StateExpression * ArithmeticStateExpressionSemanticAction(StateExpression * leftExpression, StateExpression * rightExpression, ExpressionType type);
StateExpression * ArithmeticStateSetSemanticAction(StateSet * stateSet);
StateExpression * ArithmeticSingularStateSetSemanticAction(State * state);

/*TransitionSet * TransitionSetSemanticAction(TransitionSet * set);*/
TransitionSet * TransitionSetSemanticAction(TransitionSet * transitionSet1, TransitionSet * transitionSet2);
TransitionSet * SingularTransitionSetSemanticAction(Transition * transition);
TransitionSet * EmptyTransitionSetSemanticAction();

/*SymbolSet * SymbolSetSemanticAction(SymbolSet * symbolSet); */
SymbolSet * SymbolSetSemanticAction(SymbolSet * symbolSet1, SymbolSet * symbolSet2);
SymbolSet * SingularSymbolSetSemanticAction(Symbol * symbol);
SymbolSet * EmptySymbolSetSemanticAction();

/*StateSet * StateSetSemanticAction(StateSet * stateSet);*/
StateSet * StateSetSemanticAction(StateSet * stateSet1, StateSet * stateSet2);
StateSet * SingularStateSetSemanticAction(State * state);
StateSet * EmptyStateSetSemanticAction();

State * StateSemanticAction(boolean isInitial, boolean isFinal, Symbol * symbol);

Transition * LeftTransitionSemanticAction(StateExpression * leftSet, StateExpression * rightSet, SymbolExpression * alphabet);
Transition * RightTransitionSemanticAction(StateExpression *leftSet, StateExpression *rightSet, SymbolExpression *alphabet);
Transition * BothSideTransitionSemanticAction(StateExpression *leftSet, StateExpression *rightSet, SymbolExpression *alphabet); 

Symbol * LambdaSemanticAction();
Symbol * SymbolSemanticAction(const char * value);

// TODO: PASAR SET DE DEFINITIONS 
Program * ExpressionProgramSemanticAction(CompilerState * compilerState, Definition * definition);
#endif
