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

Automata * AutomataSemanticAction(StateExpression* states, SymbolExpression* alphabet, TransitionExpression* transitions);
AutomataType AutomataTypeAction(AutomataType type);

TransitionExpression * ArithmeticTransitionExpressionSemanticAction(TransitionExpression * leftExpression, TransitionExpression * rightExpression, ExpressionType type);
TransitionExpression * ArithmeticSetTransitionExpressionSemanticAction(TransitionSet * transitionSet);
TransitionExpression * ArithmeticSingularTransitionExpressionSemanticAction(Transition * transition);

SymbolExpression * ArithmeticSymbolExpressionSemanticAction(SymbolExpression * leftExpression, SymbolExpression * rightExpression, ExpressionType type);
SymbolExpression * ArithmeticSetSymbolExpressionSemanticAction(SymbolSet * symbolSet);
SymbolExpression * ArithmeticSingularSymbolExpressionSemanticAction(Symbol * symbol);

StateExpression * ArithmeticStateExpressionSemanticAction(StateExpression * leftExpression, StateExpression * rightExpression, ExpressionType type);
StateExpression * ArithmeticSetStateExpressionSemanticAction(StateSet * stateSet);
StateExpression * ArithmeticSingularStateExpressionSemanticAction(State * state);

TransitionSet * SingularExpressionTransitionSetSemanticAction(TransitionExpression* transitionExpression);
StateSet * SingularExpressionStateSetSemanticAction(StateExpression * stateExpression);
SymbolSet * SingularExpressionSymbolSetSemanticAction(SymbolExpression * symbolSet);

TransitionExpression * ArithmeticSetTransitionExpressionSemanticAction(TransitionSet * transitionSet);
SymbolExpression * ArithmeticSetSymbolExpressionSemanticAction(SymbolSet * symbolSet);
StateExpression * ArithmeticSetStateExpressionSemanticAction(StateSet * stateSet);

TransitionExpression * ArithmeticSingularTransitionExpressionSemanticAction(Transition * transitionSet);
StateExpression * ArithmeticSingularStateExpressionSemanticAction(State * stateSet);
SymbolExpression * ArithmeticSingularSymbolExpressionSemanticAction(Symbol * symbolSet);

TransitionExpression * ArithmeticEmptySetTransitionExpressionSemanticAction();
StateExpression * ArithmeticEmptySetStateExpressionSemanticAction();
SymbolExpression * ArithmeticEmptySetSymbolExpressionSemanticAction();

/*TransitionSet * TransitionSetSemanticAction(TransitionSet * set);*/
TransitionSet * TransitionExpressionsSemanticAction(TransitionExpression* transitionSet1, TransitionExpression* transitionSet2);
TransitionSet * SingularTransitionSetSemanticAction(Transition * transition);

/*SymbolSet * SymbolSetSemanticAction(SymbolSet * symbolSet); */
SymbolSet * SymbolExpressionsSemanticAction(SymbolExpression* symbolSet1, SymbolExpression* symbolSet2);
SymbolSet * SingularSymbolSetSemanticAction(Symbol * symbol);

/*StateSet * StateSetSemanticAction(StateSet * stateSet);*/
StateSet * StateExpressionsSemanticAction(StateExpression* stateSet1, StateExpression* stateSet2);
StateSet * SingularStateSetSemanticAction(State * state);

State * StateSemanticAction(boolean isInitial, boolean isFinal, Symbol * symbol);

Transition * LeftTransitionSemanticAction(StateExpression * leftSet, StateExpression * rightSet, SymbolExpression * alphabet);
Transition * RightTransitionSemanticAction(StateExpression *leftSet, StateExpression *rightSet, SymbolExpression *alphabet);
Transition * BothSideTransitionSemanticAction(StateExpression *leftSet, StateExpression *rightSet, SymbolExpression *alphabet); 

Symbol * LambdaSemanticAction();
Symbol * SymbolSemanticAction(const char * value);

// TODO: PASAR SET DE DEFINITIONS 
Program * ExpressionProgramSemanticAction(CompilerState * compilerState, Definition * definition);
#endif
