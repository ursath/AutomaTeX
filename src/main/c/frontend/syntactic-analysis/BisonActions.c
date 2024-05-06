#include "BisonActions.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

void initializeBisonActionsModule() {
	_logger = createLogger("BisonActions");
}

void shutdownBisonActionsModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
}

/** IMPORTED FUNCTIONS */

extern unsigned int flexCurrentContext(void);

/* PRIVATE FUNCTIONS */

static void _logSyntacticAnalyzerAction(const char * functionName);

/**
 * Logs a syntactic-analyzer action in DEBUGGING level.
 */
static void _logSyntacticAnalyzerAction(const char * functionName) {
	logDebugging(_logger, "%s", functionName);
}


/* ------------------------------------------------- DEFINITION ------------------------------------------------- */


Definition * StateSetDefinitionSemanticAction(char * identifier, StateSet * set) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Definition * definition = calloc(1, sizeof(Definition));
	set->identifier = identifier;
	definition->stateSet = set;
	return definition;
}

Definition * SingularStateSetDefinitionSemanticAction(char * identifier, State* state) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	StateSet * set = SingularStateSetSemanticAction(state);
	return StateSetDefinitionSemanticAction(identifier, set);
}

Definition * SymbolSetDefinitionSemanticAction(char * identifier, SymbolSet * set) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Definition * definition = calloc(1, sizeof(Definition));
	set->identifier = identifier;
	definition->symbolSet = set;
	return definition;
}

Definition * SingularSymbolSetDefinitionSemanticAction(char * identifier, Symbol * symbol) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SymbolSet * set = SingularSymbolSetSemanticAction(symbol);
	return SymbolSetDefinitionSemanticAction(identifier, set);
}

Definition * TransitionSetDefinitionSemanticAction(char * identifier, TransitionSet * set) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Definition * definition = calloc(1, sizeof(Definition));
	set->identifier = identifier;
	definition->transitionSet = set;
	return definition;
}

Definition * SingularTransitionSetDefinitionSemanticAction(char * identifier, Transition * transition){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Definition * definition = calloc(1, sizeof(Definition));
	TransitionSet * set	= SingularTransitionSetSemanticAction(transition);
	return TransitionSetDefinitionSemanticAction(identifier, set);
}

Definition * AutomataDefinitionSemanticAction(AutomataType type, char * identifier, Automata * automata ) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Definition * definition = calloc(1, sizeof(Definition));
	automata->identifier = identifier;
	automata->automataType = type;
	definition->automata = automata;
	return definition;
}


/* ------------------------------------------------- AUTOMATA ------------------------------------------------- */


Automata * AutomataSemanticAction( StateSet * states, SymbolSet * alphabet, TransitionSet * transitions) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Automata * automata = calloc(1, sizeof(Automata));
	automata->states = states;
	automata->alphabet = alphabet;
	automata->transitions = transitions;
	return automata;
}

AutomataType AutomataTypeAction(AutomataType type) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return type;
}


/* ------------------------------------------------- EXPRESSIONS ------------------------------------------------- */


TransitionExpression * ArithmeticTransitionExpressionSemanticAction(TransitionExpression * leftExpression, TransitionExpression * rightExpression, ExpressionType type) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	TransitionExpression * expression = calloc(1, sizeof(TransitionExpression));
	expression->leftExpression = leftExpression;
	expression->rightExpression = rightExpression;
	expression->type = type;
	return expression;
}

StateExpression * ArithmeticStateExpressionSemanticAction(StateExpression * leftExpression, StateExpression * rightExpression, ExpressionType type) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	StateExpression * expression = calloc(1, sizeof(StateExpression));
	expression->leftExpression = leftExpression;
	expression->rightExpression = rightExpression;
	expression->type = type;
	return expression;
}

SymbolExpression * ArithmeticSymbolExpressionSemanticAction(SymbolExpression * leftExpression, SymbolExpression * rightExpression, ExpressionType type) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SymbolExpression * expression = calloc(1, sizeof(SymbolExpression));
	expression->leftExpression = leftExpression;
	expression->rightExpression = rightExpression;
	expression->type = type;
	return expression;
}

TransitionExpression * ArithmeticTransitionSetSemanticAction(TransitionSet * transitionSet) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	TransitionExpression * expression = calloc(1, sizeof(TransitionExpression));
	expression->transitionSet = transitionSet;
	expression->type = SET;
	return expression;
}

StateExpression * ArithmeticStateSetSemanticAction(StateSet * stateSet) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	StateExpression * expression = calloc(1, sizeof(StateExpression));
	expression->stateSet = stateSet;
	expression->type = SET;
	return expression;
}

SymbolExpression * ArithmeticSymbolSetSemanticAction(SymbolSet * symbolSet) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SymbolExpression * expression = calloc(1, sizeof(SymbolExpression));
	expression->symbolSet = symbolSet;
	expression->type = SET;
	return expression;
}

TransitionExpression * ArithmeticSingularSymbolSetSemanticAction(Transition * transition) {
_logSyntacticAnalyzerAction(__FUNCTION__);
	TransitionSet * transitionSet = SingularStateSetSemanticAction(transition);
	TransitionExpression * expression = calloc(1, sizeof(TransitionExpression));
	expression->transitionSet = transitionSet;
	expression->type = SET;
	return expression;
}

SymbolExpression * ArithmeticSingularSymbolSetSemanticAction(Symbol * symbol) {
_logSyntacticAnalyzerAction(__FUNCTION__);
	SymbolSet * symbolSet = SingularSymbolSetSemanticAction(symbol);
	SymbolExpression * expression = calloc(1, sizeof(SymbolExpression));
	expression->symbolSet = symbolSet;
	expression->type = SET;
	return expression;
}

StateExpression * ArithmeticSingularStateSetSemanticAction(State * state) {
_logSyntacticAnalyzerAction(__FUNCTION__);
	StateSet * stateSet = SingularStateSetSemanticAction(state);
	StateExpression * expression = calloc(1, sizeof(StateExpression));
	expression->stateSet = stateSet;
	expression->type = SET;
	return expression;
}
/* ------------------------------------------------- SETS ------------------------------------------------- */

/*
StateSet * StateSetSemanticAction(StateSet * stateSet){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	StateSet * state = calloc(1, sizeof(StateSet));
	state = stateSet;
	return state;
}
*/
StateSet * SingularStateSetSemanticAction(State * state){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	StateSet * stateSet = calloc(1, sizeof(StateSet));
	StateNode * node = calloc(1, sizeof(StateNode));
	node->state = *state;
	stateSet->states = node;
	stateSet->tail = node;
	return stateSet;
}

StateSet * StateSetSemanticAction(StateSet * stateSet1, StateSet * stateSet2){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	StateSet * stateSet = calloc(1, sizeof(StateSet));
	stateSet->states = stateSet1->states;
	stateSet1->tail->next = stateSet2->states;
	stateSet->tail = stateSet2->tail;
	return stateSet;
}

StateSet *  EmptyStateSetSemanticAction(){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	StateSet * stateSet = calloc(1, sizeof(StateSet));
	return stateSet;
}

SymbolSet * SingularSymbolSetSemanticAction(Symbol * symbol) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SymbolSet * symbolSet = calloc(1, sizeof(SymbolSet));
	SymbolNode * node = calloc(1, sizeof(SymbolNode));
	node->symbol = *symbol;
	symbolSet->alphabet = node;
	symbolSet->tail = node;
	return symbolSet;
}

SymbolSet * SymbolSetSemanticAction(SymbolSet * symbolSet1, SymbolSet * symbolSet2) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SymbolSet * symbolSet = calloc(1, sizeof(SymbolSet));
	symbolSet->alphabet = symbolSet1->alphabet;
	symbolSet1->tail->next = symbolSet2->alphabet;
	symbolSet->tail = symbolSet2->tail;
	return symbolSet;
}

SymbolSet * EmptySymbolSetSemanticAction(){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SymbolSet * symbolSet = calloc(1, sizeof(SymbolSet));
	return symbolSet;
}
/*
TransitionSet * TransitionSetSemanticAction(TransitionSet * set) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	TransitionSet * transitionSet = calloc(1, sizeof(TransitionSet));
	transitionSet = set;
	return transitionSet;
}
*/
TransitionSet * SingularTransitionSetSemanticAction(Transition * transition) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	TransitionSet * transitionSet = calloc(1, sizeof(TransitionSet));
	TransitionNode * node = calloc(1, sizeof(TransitionNode));
	node->transition = *transition;
	transitionSet->transitions = node;
	transitionSet->tail = node;
	return transitionSet;
}

TransitionSet * TransitionSetSemanticAction(TransitionSet * transitionSet1, TransitionSet * transitionSet2) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	TransitionSet * transitionSet = calloc(1, sizeof(TransitionSet));
	transitionSet->transitions = transitionSet1->transitions;
	transitionSet1->tail->next = transitionSet2->transitions;
	transitionSet->tail = transitionSet2->tail;
	return transitionSet;
}

TransitionSet * EmptyTransitionSetSemanticAction(){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	TransitionSet * transitionSet = calloc(1, sizeof(TransitionSet));
	return transitionSet;
}

/* ------------------------------------------------- ELEMENTS ------------------------------------------------- */


State * StateSemanticAction(boolean isInitial, boolean isFinal, Symbol * symbol){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	State * state = calloc(1, sizeof(State));
	state->isFinal = isFinal;
	state->isInitial = isInitial;
	state->symbol = *symbol;
	return state;
}

Symbol * LambdaSemanticAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Symbol * symbol = calloc(1, sizeof(Symbol));
	symbol->value= LAMBDA;
	return symbol;
}


Symbol * SymbolSemanticAction(const char * value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Symbol * symbol = calloc(1, sizeof(Symbol));
	symbol->value = value;
	return symbol;
}

// se tiene que mandar un TransitionSet donde ya parseo las transitions en individuales (estado -simbolo-> estado) lo que me sirve para en el 
// backend hacer el manejo de operaciones con conjuntos de transiciones 
Transition * LeftTransitionSemanticAction(StateSet * leftSet, StateSet * rightSet, SymbolSet * alphabet){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Transition * transition = calloc(1, sizeof(Transition));	
	transition->fromSet= rightSet;
	transition->toSet = leftSet;
	transition->symbolSet = alphabet;
	return transition;
}

Transition * RightTransitionSemanticAction(StateSet *leftSet, StateSet *rightSet, SymbolSet *alphabet){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Transition * transition = calloc(1, sizeof(Transition));
	transition->fromSet= leftSet;
	transition->toSet = rightSet;
	transition->symbolSet = alphabet;
	return transition;	
}

Transition * BothSideTransitionSemanticAction(StateSet *leftSet, StateSet *rightSet, SymbolSet *alphabet){
	_logSyntacticAnalyzerAction(__FUNCTION__);	
	Transition * transition = calloc(1, sizeof(Transition));
	transition->fromSet= leftSet;
	transition->toSet = rightSet;
	transition->symbolSet = alphabet;
	return transition;
}

/* ------------------------------------------------- PROGRAM ------------------------------------------------- */

// se tendría que manejar un conjunto de definiciones como resultado de un programa que luego se resuelven en el back
Program * ExpressionProgramSemanticAction(CompilerState * compilerState, Definition * definition) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	program->definition = definition;
	compilerState->abstractSyntaxtTree = program;
	if (0 < flexCurrentContext()) {
		logError(_logger, "The final context is not the default (0): %d", flexCurrentContext());
		compilerState->succeed = false;
	}
	else {
		compilerState->succeed = true;
	}
	return program;
}

