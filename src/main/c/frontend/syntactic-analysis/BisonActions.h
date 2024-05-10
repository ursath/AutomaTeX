#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../shared/CompilerState.h"
#include "../../shared/Logger.h"
#include "../../shared/Type.h"
#include "AbstractSyntaxTree.h"
#include "SyntacticAnalyzer.h"
#include <stdlib.h>

#define LAMBDA_STRING "@"

/** Initialize module's internal state. */
void initializeBisonActionsModule();

/** Shutdown module's internal state. */
void shutdownBisonActionsModule();

/**
 * Bison semantic actions.
 */


/* Definitions */
Definition * AutomataDefinitionSemanticAction(AutomataType type, char * identifier, Automata * automata);
Definition * SingularTransitionSetDefinitionSemanticAction(char * identifier, Transition * transition);
Definition * TransitionSetDefinitionSemanticAction(char * identifier, TransitionSet * set);
Definition * SingularSymbolSetDefinitionSemanticAction(char * identifier, Symbol* symbol);
Definition * SymbolSetDefinitionSemanticAction(char * identifier, SymbolSet * set);
Definition * SingularStateSetDefinitionSemanticAction(char * identifier, State* state);
Definition * StateSetDefinitionSemanticAction(char * identifier, StateSet * set);
DefinitionSet * DefinitionSetSemanticAction(Definition * definition1, DefinitionSet * set2); 
DefinitionSet * SingularDefinitionSetSemanticAction(Definition * definition);

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
StateExpression * StateTypeSetSemanticAction(char * identifier, StateType type);

TransitionExpression * SingularTransitionExpressionSemanticAction(Transition * transitionSet);
StateExpression * SingularStateExpressionSemanticAction(State * stateSet);
SymbolExpression * SingularSymbolExpressionSemanticAction(Symbol * symbolSet);

TransitionExpression * EmptySetTransitionExpressionSemanticAction();
StateExpression * EmptySetStateExpressionSemanticAction();
SymbolExpression * EmptySetSymbolExpressionSemanticAction();

SymbolExpression * IdentifierSymbolExpressionSemanticAction(char * identifier);
StateExpression * IdentifierStateExpressionSemanticAction(char * identifier);
TransitionExpression * IdentifierTransitionExpressionSemanticAction(char * identifier);

TransitionExpression * BothSideTransitionSemanticAction(StateExpression *leftSet, StateExpression *rightSet, SymbolExpression *alphabet); 

/*TransitionSet * TransitionSetSemanticAction(TransitionSet * set);*/
TransitionNode * TransitionSetSemanticAction(TransitionNode * transitionNode, TransitionSet * transitionSet);
TransitionSet * SingularNodeTransitionSetSemanticAction(TransitionNode * transitionNode);

SymbolSet * SymbolExpressionsSemanticAction(SymbolNode * symbolSet1, SymbolSet * symbolSet2);
SymbolSet * SingularNodeSymbolSetSemanticAction(SymbolNode * symbolNode);

StateSet * StateSetSemanticAction(StateNode * stateNode, StateSet * stateSet);
StateSet * SingularNodeStateSetSemanticAction(StateNode * stateNode);

StateNode * ExpressionStateNodeSemanticAction(StateExpression stateExpression);
SymbolNode * ExpressionSymbolNodeSemanticAction(SymbolExpression symbolExpression);
TransitionNode * ExpressionTransitionNodeSemanticAction(TransitionExpression transitionExpression);

State * StateSemanticAction(boolean isInitial, boolean isFinal, Symbol * symbol);
StateType stateTypeSemanticAction(StateType type);

Transition * LeftTransitionSemanticAction(StateExpression * leftSet, StateExpression * rightSet, SymbolExpression * alphabet);
Transition * RightTransitionSemanticAction(StateExpression *leftSet, StateExpression *rightSet, SymbolExpression *alphabet);

Symbol * LambdaSemanticAction();
Symbol * SymbolSemanticAction(char * value);

// TODO: PASAR SET DE DEFINITIONS 
Program * ExpressionProgramSemanticAction(CompilerState * compilerState, Definition * definitionSet);
//Program * ExpressionProgramSemanticAction(CompilerState * compilerState, DefinitionSet * definitionSet);
#endif
