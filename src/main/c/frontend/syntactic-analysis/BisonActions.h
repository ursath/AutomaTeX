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
Definition * TransitionSetDefinitionSemanticAction(char * identifier, TransitionSet * set);
Definition * SymbolSetDefinitionSemanticAction(char * identifier, SymbolSet * set);
Definition * StateSetDefinitionSemanticAction(char * identifier, StateSet * set);

Automata * AutomataSemanticAction(StateSet * states, SymbolSet * alphabet, TransitionSet * transitions);
AutomataType AutomataTypeAction(AutomataType type);

TransitionExpression * ArithmeticTransitionExpressionSemanticAction(TransitionExpression * leftExpression, TransitionExpression * rightExpression, ExpressionType type);
TransitionExpression * ArithmeticTransitionSetSemanticAction(TransitionSet * transitionSet);
SymbolExpression * ArithmeticSymbolExpressionSemanticAction(SymbolExpression * leftExpression, SymbolExpression * rightExpression, ExpressionType type);
SymbolExpression * ArithmeticSymbolSetSemanticAction(SymbolSet * symbolSet);
StateExpression * ArithmeticStateExpressionSemanticAction(StateExpression * leftExpression, StateExpression * rightExpression, ExpressionType type);
StateExpression * ArithmeticStateSetSemanticAction(StateSet * stateSet);


TransitionSet * TransitionSetSemanticAction(TransitionSet * set);
TransitionSet * TransitionSetSemanticAction(TransitionSet * transitionSet1, TransitionSet * transitionSet2);
TransitionSet * SingularTransitionSetSemanticAction(Transition * transition);

SymbolSet * SingularSymbolSetSemanticAction(Symbol * symbol); 
SymbolSet * SymbolSetSemanticAction(SymbolSet * symbolSet1, SymbolSet * symbolSet2);
SymbolSet * SingularSymbolSetSemanticAction(Symbol * symbol);

StateSet * StateSetSemanticAction(StateSet * stateSet);
StateSet * StateSetSemanticAction(StateSet * stateSet1, StateSet * stateSet2);
StateSet * SingularStateSetSemanticAction(State * state);

State * StateSemanticAction(boolean isInitial, boolean isFinal, Symbol symbol);

Transition * LeftTransitionSemanticAction(StateSet * leftSet, StateSet * rightSet, SymbolSet * alphabet);
Transition * RightTransitionSemanticAction(StateSet *leftSet, StateSet *rightSet, SymbolSet *alphabet);
Transition * BothSideTransitionSemanticAction(StateSet *leftSet, StateSet *rightSet, SymbolSet *alphabet); 

Symbol * LambdaSemanticAction();
Symbol * SymbolSemanticAction(const char * value);

Program * ExpressionProgramSemanticAction(CompilerState * compilerState, Expression * expression);
#endif
