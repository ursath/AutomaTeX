#include "AbstractSyntaxTree.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

void initializeAbstractSyntaxTreeModule() {
	_logger = createLogger("AbstractSyntxTree");
}

void shutdownAbstractSyntaxTreeModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
}

/** PUBLIC FUNCTIONS */





void releaseConstant(Constant * constant) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (constant != NULL) {
		free(constant);
	}
}

void releaseExpression(Expression * expression) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (expression != NULL) {
		switch (expression->type) {
			case ADDITION:
			case DIVISION:
			case MULTIPLICATION:
			case SUBTRACTION:
				releaseExpression(expression->leftExpression);
				releaseExpression(expression->rightExpression);
				break;
			case FACTOR:
				releaseFactor(expression->factor);
				break;
		}
		free(expression);
	}
}

void releaseFactor(Factor * factor) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (factor != NULL) {
		switch (factor->type) {
			case CONSTANT:
				releaseConstant(factor->constant);
				break;
			case EXPRESSION:
				releaseExpression(factor->expression);
				break;
		}
		free(factor);
	}
}




void releaseDefinition(Definition * definition) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (definition != NULL) {
		switch (definition->type)
		{
		case /* constant-expression */:
			/* code */
			break;
		
		default:
			break;
		}
		while ( currentDefinition != NULL ){
			releaseDefinition(currentDefinition);
			currentDefinition = currentDefinition->next;
		}
		
		free(definitionNode);
	}
}

void releaseDefinitionNode(DefinitionNode * definitionNode) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (definitionNode != NULL) {
		Definition currentDefinition = definition->definition;
		while ( currentDefinition != NULL ){
			releaseDefinition(currentDefinition);
			currentDefinition = currentDefinition->next;
		}
		
		free(definitionNode);
	}
}

void releaseDefinitionSet(DefinitionSet * definitionSet) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (definitionSet != NULL) {
		DefinitionNode currentDefinitionNode = definitionSet->first;
		while ( currentDefinitionNode != definitionSet->tail ){
			releaseDefinitionNode(currentDefinitionNode);
		}
		if ( definitionSet->tail != NULL )
			releaseDefinitionNode(currentDefinitionNode);

		free(definitionSet);
	}
}


void releaseProgram(Program * program) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (program != NULL) {
		releaseDefinitionSet(program->definitionSet);
		free(program);
	}
}
