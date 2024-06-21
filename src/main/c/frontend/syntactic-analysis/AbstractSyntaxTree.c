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


void releaseStateSet(StateSet * stateSet){
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if ( stateSet != NULL ){
		StateNode * stateNode = stateSet->first;
		/*
		while ( stateNode != NULL ){
			releaseStateExpression(stateNode->expression);
			currentDefinition = currentDefinition->next;
		}
		
		free(definitionNode);*/
	}
}

// todo 
void releaseDefinition(Definition * definition) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (definition != NULL) {
		switch (definition->type)
		{
		case STATE_DEFINITION:
			//releaseStateSet(definition->stateSet);
			break;
		case ALPHABET_DEFINITION:
			//releaseAlphabet(definition->symbolSet);
			break;
		case TRANSITION_DEFINITION:
			//releaseTransitionSet(definition->transitionSet);
			break;
		default:	// automata
			break;
		}
 		free(definition);
	}
}

void releaseDefinitionNode(DefinitionNode * definitionNode) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (definitionNode != NULL) {
		releaseDefinition(definitionNode->definition);	
		free(definitionNode);
	}
}

void releaseDefinitionSet(DefinitionSet * definitionSet) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (definitionSet != NULL) {
		DefinitionNode * currentDefinitionNode = definitionSet->first;
		while ( currentDefinitionNode != NULL ){							//!= definitionSet->tail
			releaseDefinitionNode(currentDefinitionNode);
			currentDefinitionNode = currentDefinitionNode->next;
		}
//		if ( definitionSet->tail != NULL )
//			releaseDefinitionNode(currentDefinitionNode);

		free(definitionSet);
	}
}

