#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../shared/CompilerState.h"
#include "../../shared/Logger.h"
#include "../../shared/Type.h"
#include "AbstractSyntaxTree.h"
#include "SyntacticAnalyzer.h"
#include <stdlib.h>



/** Initialize module's internal state. */
void initializeBisonActionsModule();

/** Shutdown module's internal state. */
void shutdownBisonActionsModule();

/**
 * Bison semantic actions.
 */


/* Definitions */
Definition * AutomataDefinitionSemanticAction(AutomataType type, char * identifier, Automata * automata);
Definition * TransitionSetDefinitionSemanticAction(char * identifier, TransitionSet * set);
Definition * SymbolSetDefinitionSemanticAction(char * identifier, SymbolSet * set);
Definition * StateSetDefinitionSemanticAction(char * identifier, StateSet * set);
DefinitionSet * DefinitionSetSemanticAction(Definition * definition1, DefinitionSet * set2); 
DefinitionSet * SingularDefinitionSetSemanticAction(Definition * definition);

Definition * TransitionExpressionDefinitionSemanticAction(char * identifier,TransitionExpression * transitionExpression);
Definition * SymbolExpressionDefinitionSemanticAction(char * identifier,SymbolExpression * symbolExpression);
Definition * StateExpressionDefinitionSemanticAction(char * identifier,StateExpression * stateExpression);

Automata * AutomataSemanticAction(StateExpression* states, SymbolExpression* alphabet, TransitionExpression* transitions);

TransitionExpression * TransitionExpressionSemanticAction(TransitionExpression * leftExpression, TransitionExpression * rightExpression, ExpressionType type);
TransitionExpression * SetTransitionExpressionSemanticAction(TransitionSet * transitionSet);
TransitionExpression * SingularTransitionExpressionSemanticAction(Transition * transition);

SymbolExpression * SymbolExpressionSemanticAction(SymbolExpression * leftExpression, SymbolExpression * rightExpression, ExpressionType type);
SymbolExpression * SetSymbolExpressionSemanticAction(SymbolSet * symbolSet);
SymbolExpression * SingularSymbolExpressionSemanticAction(Symbol * symbol);

StateExpression * StateExpressionSemanticAction(StateExpression * leftExpression, StateExpression * rightExpression, ExpressionType type);
StateExpression * SetStateExpressionSemanticAction(StateSet * stateSet);
StateExpression * SingularStateExpressionSemanticAction(State * state);

TransitionExpression * SetTransitionExpressionSemanticAction(TransitionSet * transitionSet);
SymbolExpression * SetSymbolExpressionSemanticAction(SymbolSet * symbolSet);
StateExpression * SetStateExpressionSemanticAction(StateSet * stateSet);

SymbolSet* IdentifierSymbolSetSemanticAction(char * identifier, boolean isFromAutomata);
StateSet* IdentifierStateSetSemanticAction(char * identifier, boolean isFromAutomata);
TransitionSet* IdentifierTransitionSetSemanticAction(char * identifier, boolean isFromAutomata);

TransitionNode * SingularExpressionTransitionNodeSemanticAction(TransitionExpression * transitionExpression);
StateNode * SingularExpressionStateNodeSemanticAction(StateExpression * stateExpression);
SymbolNode * SingularExpressionSymbolNodeSemanticAction(SymbolExpression * symbolExpression);

TransitionNode * ExpressionsTransitionNodeSemanticAction(TransitionExpression * leftExpression, TransitionNode * nextNode);
StateNode * ExpressionsStateNodeSemanticAction(StateExpression * leftExpression, StateNode * nextNode);
SymbolNode * ExpressionsSymbolNodeSemanticAction(SymbolExpression * leftExpression, SymbolNode * nextNode);

TransitionSet * NodeTransitionSetSemanticAction(TransitionNode * transitionNode);
StateSet * NodeStateSetSemanticAction(StateNode * stateNode);	
SymbolSet * NodeSymbolSetSemanticAction(SymbolNode * symbolNode);

StateSet * StateTypeSetSemanticAction(char * identifier, StateType type, boolean isFromAutomata);

TransitionSet * EmptyTransitionSetSemanticAction();
StateSet * EmptyStateSetSemanticAction();
SymbolSet * EmptySymbolSetSemanticAction();

TransitionSet * BothSideTransitionSemanticAction(StateExpression *leftSet, StateExpression *rightSet, SymbolExpression *alphabet); 

State * StateSemanticAction(boolean isInitial, boolean isFinal, Symbol * symbol);

Transition * LeftTransitionSemanticAction(StateExpression * leftSet, StateExpression * rightSet, SymbolExpression * alphabet);
Transition * RightTransitionSemanticAction(StateExpression *leftSet, StateExpression *rightSet, SymbolExpression *alphabet);

Symbol * LambdaSemanticAction();
Symbol * SymbolSemanticAction(char * value);

Program * ExpressionProgramSemanticAction(CompilerState * compilerState, DefinitionSet * definitionSet);

#endif
