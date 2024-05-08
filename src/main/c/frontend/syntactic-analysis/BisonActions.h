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

TransitionExpression * TransitionExpressionSemanticAction(TransitionExpression * leftExpression, TransitionExpression * rightExpression, ExpressionType type);
TransitionExpression * SetTransitionExpressionSemanticAction(TransitionSet * transitionSet);
TransitionExpression * SingularTransitionExpressionSemanticAction(Transition * transition);

SymbolExpression * SymbolExpressionSemanticAction(SymbolExpression * leftExpression, SymbolExpression * rightExpression, ExpressionType type);
SymbolExpression * SetSymbolExpressionSemanticAction(SymbolSet * symbolSet);
SymbolExpression * SingularSymbolExpressionSemanticAction(Symbol * symbol);

StateExpression * StateExpressionSemanticAction(StateExpression * leftExpression, StateExpression * rightExpression, ExpressionType type);
StateExpression * SetStateExpressionSemanticAction(StateSet * stateSet);
StateExpression * SingularStateExpressionSemanticAction(State * state);

TransitionSet * SingularExpressionTransitionSetSemanticAction(TransitionExpression* transitionExpression);
StateSet * SingularExpressionStateSetSemanticAction(StateExpression * stateExpression);
SymbolSet * SingularExpressionSymbolSetSemanticAction(SymbolExpression * symbolSet);

TransitionExpression * SetTransitionExpressionSemanticAction(TransitionSet * transitionSet);
SymbolExpression * SetSymbolExpressionSemanticAction(SymbolSet * symbolSet);
StateExpression * SetStateExpressionSemanticAction(StateSet * stateSet);
StateExpression * StateTypeSetSemanticAction(char * identifier, Token type);

TransitionExpression * SingularTransitionExpressionSemanticAction(Transition * transitionSet);
StateExpression * SingularStateExpressionSemanticAction(State * stateSet);
SymbolExpression * SingularSymbolExpressionSemanticAction(Symbol * symbolSet);

TransitionExpression * EmptySetTransitionExpressionSemanticAction();
StateExpression * EmptySetStateExpressionSemanticAction();
SymbolExpression * EmptySetSymbolExpressionSemanticAction();

/*TransitionSet * TransitionSetSemanticAction(TransitionSet * set);*/
TransitionSet * TransitionExpressionsSemanticAction(TransitionExpression* transitionSet1, TransitionExpression* transitionSet2);
TransitionSet * SingularTransitionSetSemanticAction(Transition * transition);
TransitionSet * BothSideTransitionSemanticAction(StateExpression *leftSet, StateExpression *rightSet, SymbolExpression *alphabet); 

/*SymbolSet * SymbolSetSemanticAction(SymbolSet * symbolSet); */
SymbolSet * SymbolExpressionsSemanticAction(SymbolExpression* symbolSet1, SymbolExpression* symbolSet2);
SymbolSet * SingularSymbolSetSemanticAction(Symbol * symbol);

/*StateSet * StateSetSemanticAction(StateSet * stateSet);*/
StateSet * StateExpressionsSemanticAction(StateExpression* stateSet1, StateExpression* stateSet2);
StateSet * SingularStateSetSemanticAction(State * state);

State * StateSemanticAction(boolean isInitial, boolean isFinal, Symbol * symbol);
StateType stateTypeSemanticAction(Token * token);

Transition * LeftTransitionSemanticAction(StateExpression * leftSet, StateExpression * rightSet, SymbolExpression * alphabet);
Transition * RightTransitionSemanticAction(StateExpression *leftSet, StateExpression *rightSet, SymbolExpression *alphabet);

Symbol * LambdaSemanticAction();
Symbol * SymbolSemanticAction(const char * value);

// TODO: PASAR SET DE DEFINITIONS 
Program * ExpressionProgramSemanticAction(CompilerState * compilerState, DefinitionSet * definitionSet);
#endif
