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

/* ----------------------------------------------- DEFINITION SET ----------------------------------------------- */

DefinitionSet * DefinitionSetSemanticAction(Definition * definition1, DefinitionSet * set2) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	DefinitionSet * set = calloc(1, sizeof(DefinitionSet));
	DefinitionNode * node = calloc(1, sizeof(DefinitionNode));
	node->definition = definition1;
	node->next = set2->first;
	set->first = node;
	set->tail = set2->tail;
	return set;
}

DefinitionSet * SingularDefinitionSetSemanticAction(Definition * definition) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	DefinitionSet * set = calloc(1, sizeof(DefinitionSet));
	DefinitionNode * node = calloc(1, sizeof(DefinitionNode));
	node->definition = definition;
	set->first = node;
	set->tail = node;
	return set;
}

/* ------------------------------------------------- DEFINITION ------------------------------------------------- */


Definition * StateSetDefinitionSemanticAction(char * identifier, StateSet * set) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Definition * definition = calloc(1, sizeof(Definition));
	set->identifier = identifier;
	definition->stateSet = set;
	definition->type = STATE_DEFINITION;
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
	definition->type = ALPHABET_DEFINITION;
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
	definition->type = TRANSITION_DEFINITION;
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
	definition->type = AUTOMATA_DEFINITION;
	return definition;
}

/*-------------- CONVERSION DE TOKEN A ENUM ----------------------------*/
StateType stateTypeSemanticAction(StateType type){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return type;
}

/* ------------------------------------------------- AUTOMATA ------------------------------------------------- */


Automata * AutomataSemanticAction( StateExpression* states, SymbolExpression* alphabet, TransitionExpression* transitions) {
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

/*---------------------------EXPRESIONES QUE REPRESENTAN UNA OPERACIÓN ENTRE CONJUNTOS-------------*/

TransitionExpression * TransitionExpressionSemanticAction(TransitionExpression * leftExpression, TransitionExpression * rightExpression, ExpressionType type) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	TransitionExpression * expression = calloc(1, sizeof(TransitionExpression));
	expression->leftExpression = leftExpression;
	expression->rightExpression = rightExpression;
	expression->type = type;
	return expression;
}

StateExpression * StateExpressionSemanticAction(StateExpression * leftExpression, StateExpression * rightExpression, ExpressionType type) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	StateExpression * expression = calloc(1, sizeof(StateExpression));
	expression->leftExpression = leftExpression;
	expression->rightExpression = rightExpression;
	expression->type = type;
	return expression;
}

SymbolExpression * SymbolExpressionSemanticAction(SymbolExpression * leftExpression, SymbolExpression * rightExpression, ExpressionType type) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SymbolExpression * expression = calloc(1, sizeof(SymbolExpression));
	expression->leftExpression = leftExpression;
	expression->rightExpression = rightExpression;
	expression->type = type;
	return expression;
}

/*----------------EXPRESIONES QUE REPRESENTAN UN SUBCONJUNTO DE ELEMENTOS ------------------*/
//conjunto de transiciones per se (al menos es un subconjunto)
TransitionExpression * SetTransitionExpressionSemanticAction(TransitionSet * transitionSet){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	TransitionExpression * expression = calloc(1, sizeof(TransitionExpression));
	expression->transitionSet = transitionSet;
	expression->type = SET_EXPRESSION;
	return expression;
}

//conjunto de símbolos per se (al menos es un subconjunto)
SymbolExpression * SetSymbolExpressionSemanticAction(SymbolSet * symbolSet){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SymbolExpression * expression = calloc(1, sizeof(SymbolExpression));
	expression->symbolSet= symbolSet;
	expression->type = SET_EXPRESSION;
	return expression;
}

//conjuto de estados per se (al menos es un subconjunto)
StateExpression * SetStateExpressionSemanticAction(StateSet * stateSet){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	StateExpression * expression = calloc(1, sizeof(StateExpression));
	expression->stateSet = stateSet;
	expression->type = SET_EXPRESSION;
	return expression;
}

/*---------------------------EXPRESIONES QUE REPRESENTAN UN ELEMENTO SUELTO --------------------*/

//transition suelta en la expresión (se maneja como un conjunto de transiciones con solo un elemento)
TransitionExpression * SingularTransitionExpressionSemanticAction(Transition * transition){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	TransitionExpression * expression = calloc(1, sizeof(TransitionExpression));
	TransitionSet * transitionSet = SingularTransitionSetSemanticAction(transition);
	//acá podría agarrar y copiar el set 
	expression->transitionSet = transitionSet;
	expression->type = SET_EXPRESSION;
	return expression;
}

//estado suelto en la expresión (se maneja como un conjunto de estados con solo un elemento)
StateExpression * SingularStateExpressionSemanticAction(State * state){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	StateExpression * expression = calloc(1, sizeof(StateExpression));
	expression->stateSet = SingularStateSetSemanticAction(state);
	expression->type = SET_EXPRESSION;
	return expression;
}

//símbolo suelto en la expresión (se maneja como un conjunto de símbolos con solo un elemento)
SymbolExpression * SingularSymbolExpressionSemanticAction(Symbol * symbol){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SymbolExpression * expression = calloc(1, sizeof(SymbolExpression));
	expression->symbolSet = SingularSymbolSetSemanticAction(symbol);
	expression->type = SET_EXPRESSION;
	return expression;
}

/*----------------- EXPRESIONES QUE REPRESENTAN EL CONJUNTO VACIO -------------------*/
//conjunto vacio que espera ser una expresión de transiciones 
TransitionExpression * EmptySetTransitionExpressionSemanticAction(){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	TransitionExpression * expression = calloc(1, sizeof(TransitionExpression));
	TransitionSet* emptySet = calloc(1, sizeof(TransitionSet));	
	expression->transitionSet = emptySet;
	expression->type = VOID_EXPRESSION;
	return expression;
}

//conjunto vacio que espera ser una expresión de estados
StateExpression * EmptySetStateExpressionSemanticAction(){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	StateExpression * expression = calloc(1, sizeof(StateExpression));
	StateSet * emptySet = calloc(1, sizeof(StateSet));
	expression->stateSet = emptySet;
	expression->type = VOID_EXPRESSION;
	return expression;
}

//conjunto vacio que espera ser una expresión de símbolos
SymbolExpression * EmptySetSymbolExpressionSemanticAction(){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SymbolExpression * expression = calloc(1, sizeof(SymbolExpression));
	SymbolSet * emptySet = calloc(1, sizeof(SymbolSet));
	expression->symbolSet = emptySet;
	expression->type = VOID_EXPRESSION;
	return expression;
}

/* ------------------------------------------------- SETS ------------------------------------------------- */

/*-----------CONJUNTOS CON SOLO UNA EXPRESIÓN ---------------*/
//formo set a partir de una sola transition expression
TransitionSet * SingularExpressionTransitionSetSemanticAction(TransitionExpression * transitionExpression){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (transitionExpression->type == SET_EXPRESSION || transitionExpression->type == VOID_EXPRESSION) {
		//cubre casos state, empty y stateSet
		//no haría falta alocar memoria porque ya lo hice anteriormente 
		return transitionExpression->transitionSet;
	}
	//cubro casos UNION, INTERSECTION y DIFFERENCE
	TransitionSet * transitionSet = calloc(1, sizeof(TransitionSet));
	TransitionNode * node = calloc(1, sizeof(TransitionNode));
	node->transitionExpression = transitionExpression;
	node->type = EXPRESSION;
	transitionSet->first= node;
	transitionSet->tail = node;
	return transitionSet;
}


//formo set a partir de una sola state expression
StateSet * SingularExpressionStateSetSemanticAction(StateExpression * stateExpression){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (stateExpression->type == SET_EXPRESSION || stateExpression->type == VOID_EXPRESSION) {
		//cubre casos state, empty y stateSet
		//no haría falta alocar memoria porque ya lo hice anteriormente 

		/* sino quedaría algo así
		StateSet * stateSet = calloc(1, sizeof(StateSet));
		stateSet->states = stateExpression->stateSet->states;
		stateSet->tail = stateExpression->stateSet->tail;		
		*/

		return stateExpression->stateSet;
	}
	//cubro casos UNION, INTERSECTION y DIFFERENCE
	StateSet * stateSet = calloc(1, sizeof(StateSet));
	StateNode * node = calloc(1, sizeof(StateNode));
	node->stateExpression = stateExpression;	
	node->type = EXPRESSION;
	stateSet->first= node;
	stateSet->tail = node;
	return stateSet;
}

//formo set a partir de una sola symbol expression
SymbolSet * SingularExpressionSymbolSetSemanticAction(SymbolExpression * symbolExpression){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (symbolExpression->type == SET_EXPRESSION || symbolExpression->type == VOID_EXPRESSION) {
		//cubre casos state, empty y stateSet
		//no haría falta alocar memoria porque ya lo hice anteriormente 
		return symbolExpression->symbolSet;
	}
	//cubro casos UNION, INTERSECTION y DIFFERENCE
	SymbolSet * symbolSet = calloc(1, sizeof(SymbolSet));
	SymbolNode * node = calloc(1, sizeof(SymbolNode));
	node->symbolExpression = symbolExpression;
	node->type = EXPRESSION;
	symbolSet->first= node;
	symbolSet->tail = node;
	return symbolSet;
}

/*----------- CONJUNTOS CON SUBCONJUNTOS ----------------*/
//formo set con 2 subconjuntos de tipo transition
TransitionSet * TransitionExpressionsSemanticAction(TransitionExpression* exp1, TransitionExpression* exp2){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	//solo aloco memoria nueva porque necesito el identificador pero los nodos ya están creados
	TransitionSet * transitionSet = calloc(1, sizeof(TransitionSet));
	TransitionNode * node1 = calloc(1, sizeof(TransitionNode));
	TransitionNode * node2 = calloc(1, sizeof(TransitionNode));	
	node1->transitionExpression= exp1;
	node1->type = EXPRESSION;
	node2->transitionExpression= exp2;
	node2->type = EXPRESSION;
	//conecto los nodos con los subconjuntos
	node1->next = node2;
	transitionSet->first = node1;
	transitionSet->tail = node2;	
	return transitionSet;
}

//formo set con 2 subconjuntos de tipo state
StateSet * StateExpressionsSemanticAction(StateExpression* exp1, StateExpression* exp2){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	//solo aloco memoria nueva porque necesito el identificador pero los nodos ya están creados
	StateSet * stateSet = calloc(1, sizeof(StateSet));
	StateNode * node1 = calloc(1, sizeof(StateNode));
	StateNode * node2 = calloc(1, sizeof(StateNode));	
	node1->stateExpression= exp1;
	node1->type = EXPRESSION;
	node2->stateExpression= exp2;
	node2->type = EXPRESSION;
	//conecto los nodos con los subconjuntos
	node1->next = node2;
	stateSet->first = node1;
	stateSet->tail = node2;	
	return stateSet;
}

//formo set con 2 subconjuntos de tipo symbol
SymbolSet * SymbolExpressionsSemanticAction(SymbolExpression* exp1, SymbolExpression* exp2){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	//solo aloco memoria nueva porque necesito el identificador pero los nodos ya están creados
	SymbolSet * symbolSet = calloc(1, sizeof(SymbolSet));
	SymbolNode * node1 = calloc(1, sizeof(SymbolNode));
	SymbolNode * node2 = calloc(1, sizeof(SymbolNode));	
	node1->symbolExpression= exp1;
	node1->type = EXPRESSION;
	node2->symbolExpression = exp2;
	node2->type = EXPRESSION;
	//conecto los nodos con los subconjuntos
	node1->next = node2;
	symbolSet->first = node1;
	symbolSet->tail = node2;	
	return symbolSet;
}

/*----- CONJUNTOS DE UN ELEMENTO SUELTO------*/

SymbolSet * SingularSymbolSetSemanticAction(Symbol * symbol) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SymbolSet * symbolSet = calloc(1, sizeof(SymbolSet));
	SymbolNode * node = calloc(1, sizeof(SymbolNode));
	node->symbol = symbol;
	node->type = ELEMENT;
	symbolSet->first= node;
	symbolSet->tail = node;
	return symbolSet;
}

StateSet * SingularStateSetSemanticAction(State * state) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	StateSet * stateSet = calloc(1, sizeof(StateSet));
	StateNode * node = calloc(1, sizeof(StateNode));
	node->state = state;
	node->type = ELEMENT;
	stateSet->first= node;
	stateSet->tail = node;
	stateSet->stateType = state->isFinal ? FINAL : state->isInitial ? INITIAL : REGULAR;
	return stateSet;
}

TransitionSet * SingularTransitionSetSemanticAction(Transition * transition) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	TransitionSet * transitionSet = calloc(1, sizeof(TransitionSet));
	TransitionNode * node = calloc(1, sizeof(TransitionNode));
	node->transition = transition;
	node->type = ELEMENT;
	transitionSet->first= node;
	transitionSet->tail = node;
	return transitionSet;
}

/*-----------------------CONJUNTO DE ESTADOS DE UN AUTOMATA POR TIPO--------------------*/
StateExpression * StateTypeSetSemanticAction(char * identifier, StateType type){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	StateExpression * stateExpression = calloc(1, sizeof(StateExpression));
	StateSet * stateSet = calloc(1, sizeof(StateSet));
	stateSet->identifier = identifier;
	stateSet->stateType = type;
	stateExpression->stateSet = stateSet;
	stateExpression->type = SET_EXPRESSION;
	return stateExpression;
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
	symbol->value= LAMBDA_STRING;
	return symbol;
}


Symbol * SymbolSemanticAction(char * value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Symbol * symbol = calloc(1, sizeof(Symbol));
	symbol->value = value;
	return symbol;
}

// se tiene que mandar un TransitionSet donde ya parseo las transitions en individuales (estado -simbolo-> estado) lo que me sirve para en el 
// backend hacer el manejo de operaciones con conjuntos de transiciones 
Transition * LeftTransitionSemanticAction(StateExpression * left, StateExpression * right, SymbolExpression * alphabet){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Transition * transition = calloc(1, sizeof(Transition));	
	transition->fromExpression= right;
	transition->toExpression= left;
	transition->symbolExpression = alphabet;
	return transition;
}

Transition * RightTransitionSemanticAction(StateExpression *left, StateExpression *right, SymbolExpression *alphabet){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Transition * transition = calloc(1, sizeof(Transition));
	transition->fromExpression= left;
	transition->toExpression= right;
	transition->symbolExpression = alphabet;
	return transition;	
}

TransitionExpression * BothSideTransitionSemanticAction(StateExpression *left, StateExpression *right, SymbolExpression *alphabet){
	_logSyntacticAnalyzerAction(__FUNCTION__);	
	Transition * transitionToLeft = LeftTransitionSemanticAction(left,right,alphabet);
	Transition * transitionToRight = RightTransitionSemanticAction(left,right,alphabet);
	TransitionExpression * transitionToLeftExpression = SingularTransitionExpressionSemanticAction(transitionToLeft);
	TransitionExpression * transitionToRightExpression = SingularTransitionExpressionSemanticAction(transitionToRight);
	return SetTransitionExpressionSemanticAction( TransitionExpressionsSemanticAction(transitionToLeftExpression, transitionToRightExpression));
}

/* ------------------------------------------------- PROGRAM ------------------------------------------------- */

// se tendría que manejar un conjunto de definiciones como resultado de un programa que luego se resuelven en el back
//Program * ExpressionProgramSemanticAction(CompilerState * compilerState, DefinitionSet* definitionSet) {
Program * ExpressionProgramSemanticAction(CompilerState * compilerState, Definition* definition) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	//program->definitionSet = definitionSet;
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

