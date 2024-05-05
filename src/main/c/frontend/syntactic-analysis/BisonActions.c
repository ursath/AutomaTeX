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



TransitionSet * TransitionSetSemanticAction(TransitionSet * set) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	TransitionSet * transitionSet = calloc(1, sizeof(TransitionSet));
	transitionSet = set;
	return transitionSet;
}

TransitionSet * SingularTransitionSetSemanticAction(Transition * transition) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	TransitionSet * transitionSet = calloc(1, sizeof(TransitionSet));
	transitionSet->transitions = transition;
	return transitionSet;
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

Definition * AutomataDefinitionSemanticAction(AutomataType type, char * name, Automata * automata ) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Definition * definition = calloc(1, sizeof(Definition));
	automata->name = name;
	automata->type = type;
	definition->automata = automata;
	return definition;
}


Automata * AutomataSemanticAction(StateSet * state, SymbolSet * alphabet, TransitionSet * transition) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Automata * automata = calloc(1, sizeof(Automata));
	automata->states = states;
	automata->alphabet = alphabet;
	automata->automataType = automataType;
	return automata;
}

SymbolSet * SymbolSetSemanticAction(SymbolSet * SymbolSet) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SymbolSet * symbol = calloc(1, sizeof(SymbolSet));
	symbolS
}

SymbolSet * SingularSymbolSetSemanticAction(Symbol * symbolbolSet) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SymbolSet * symbolSet = calloc(1, sizeof(SymbolSet));
	symbolSet->alphabet = symbol;
	return symbolSet;
}

Symbol * LambdaSemanticAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Symbol * symbol = calloc(1, sizeof(Symbol));
	symbol->name = LAMBDA;
	return symbol;
}


Symbol * SymbolSemanticAction(const char * name) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Symbol * symbol = calloc(1, sizeof(Symbol));
	symbol->name = name;
	return symbol;
}

Expression * ArithmeticExpressionSemanticAction(Expression * leftExpression, Expression * rightExpression, ExpressionType type) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->leftExpression = leftExpression;
	expression->rightExpression = rightExpression;
	expression->type = type;
	return expression;
}

/*
Expression * ArithmeticExpressionSemanticAction(Expression * leftExpression, Expression * rightExpression, ExpressionType type) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->leftExpression = leftExpression;
	expression->rightExpression = rightExpression;
	expression->type = type;
	return expression;
}

Constant * IntegerConstantSemanticAction(const int value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Constant * constant = calloc(1, sizeof(Constant));
	constant->value = value;
	return constant;
}

Expression * FactorExpressionSemanticAction(Factor * factor) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->factor = factor;
	expression->type = FACTOR;
	return expression;
}

Automata * FactorExpressionSemanticAction(Set states, Set aplphabet,AutomataType automataType ) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Automata * automata = calloc(1, sizeof(Expression));
	automata->states = states;
	automata->alphabet = alphabet;
	automata->automataType = automataType;
	return automata;
}

Factor * ConstantFactorSemanticAction(Constant * constant) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Factor * factor = calloc(1, sizeof(Factor));
	factor->constant = constant;
	factor->type = CONSTANT;
	return factor;
}

Factor * ExpressionFactorSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Factor * factor = calloc(1, sizeof(Factor));
	factor->expression = expression;
	factor->type = EXPRESSION;
	return factor;
}

Program * ExpressionProgramSemanticAction(CompilerState * compilerState, Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	program->expression = expression;
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
*/
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