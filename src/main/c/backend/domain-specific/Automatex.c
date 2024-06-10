#include "Automatex.h"

ComputationResult computeDefinitionSet(DefinitionSet * definitionSet) {
    ComputationResult result {
        .succeed = true,
        .isDefinitionSet = true
    };
    
    return result;
}

ComputationResult computeDefinition(Definition * definition) {
    switch (definition->type) {
        case AUTOMATA_DEFINITION:
            return computeAutomata(definition->automata);
        case TRANSITION_DEFINITION:
            return computeTransitionSet(definition->transitionSet);
        case ALPHABET_DEFINITION:
            return computeSymbolSet(definition->symbolSet);
        case STATE_DEFINITION:
            return computeStateSet(definition->stateSet);
        default:
            break;
    }
}

ComputationResult computeAutomata(Automata * automata) {
    ComputationResult result = {
        .succeed = true,
        .isDefinitionSet = false,
        .type = AUTOMATA_DEFINITION,
        .automata = automata            
    };
    ComputationResult stateSetResult = computeStateExpression(automata->states);
    // check si initial y final(es)
    // y repes
    // agrego a tabla
    ComputationResult symbolSetResult = computeSymbolExpression(automata->alphabet);
    // check repes
    // agrego a tabla
    ComputationResult transitionSetResult = computeTransitionExpression(automata->transitions);
    // check si los estados y simbolos de las transiciones pertenecen a los del automata
    // check repes
    // agrego a tabla
    return result;
}

ComputationResult computeTransitionExpression(TransitionExpression * expression, boolean isSingleElement ){
    switch ( expression->type) {
        case UNION_EXPRESSION:
                return _transitionUnion(expression->left, expression->right);
                        break;
        case INTERSECTION_EXPRESSION:
                return _transitionIntersection(expression->left, expression->right);
                        break;
        case DIFFERENCE_EXPRESSION:
                return _transitionDifference(expression->left, expression->right);
                        break;
        case SET_EXPRESSION:
                return computeTransitionSet(expression->transitionSet);
                        break;
        case ELEMENT_EXPRESSION:
                return computeTransition(expression->transition, isSingleElement);
                        break;
        default:
            return _invalidComputation();
    }
}

ComputationResult computeStateExpression(StateExpression * expression,  boolean isSingleElement) {
    switch ( expression->type) {
        case UNION_EXPRESSION:
                    return _stateUnion(expression->left, expression->right);
                        break;
        case INTERSECTION_EXPRESSION:
                    return _stateIntersection(expression->left, expression->right);
                        break;
        case DIFFERENCE_EXPRESSION:
                    return _stateDifference(expression->left, expression->right);
                        break;
        case SET_EXPRESSION:
                return computeStateSet(expression->stateSet);
                        break;
        case ELEMENT_EXPRESSION:
                return computeState(expression->state, isSingleElement);
                        break;
        default:
            return _invalidComputation();
    }
}


ComputationResult computeSymbolExpression(SymbolExpression * expression, boolean isSingleElement) {
    switch ( expression->type) {
        case UNION_EXPRESSION:
                return _symbolUnion(expression->left, expression->right);
                        break;
        case INTERSECTION_EXPRESSION:
                return _symbolIntersection(expression->left, expression->right);
                        break;
        case DIFFERENCE_EXPRESSION:
                return _symbolDifference(expression->left, expression->right);
                        break;
        case SET_EXPRESSION:
                return computeSymbolSet(expression->symbolSet);
                break;
        case ELEMENT_EXPRESSION:
                return computeSymbol(expression->symbol, isSingleElement) ;
                break;
        default:
            return _invalidComputation();
    }
}

ComputationResult computeTransitionSet(TransitionSet* set) {
    ComputationResult result = {
        .succeed = true,
        .isDefinitionSet = false,
        .type = TRANSITION_DEFINITION
    };
    
    if ( set->isBothSidesTransition ) {
        ComputationResult * result = computeTransitionExpression(set->first, true);
        set->first = leftResult->transitionSet->first; 
        result = computeTransitionExpression(set->tail, true);
        set->tail = leftResult->transitionSet->first; 
        
    } else if ( set->identifier != NULL ) {
        EntryResult result = getValue(set->identifier, set->isFromAutomata? AUTOMATA_TRANSITIONS: TRANSITIONS );
        set = result.stateSet;
    }
    result->transitionSet = set;
    return result;
}

void filterStates( StateSet set, StateType type){
    StateNode * currentNode = set->first;
    State * currentState;
    StateNode * resultTail;
    while ( currentNode != set->tail ){
        currentState =  currentNode->state
        if ( type==FINAL && currentState->isFinal )
            if ( resultTail != NULL )
                resultTail = currentNode;
            else
                resultTail->next = currentNode;
        currentNode = currentNode->next; 
    }
}

ComputationResult computeStateSet(StateSet* set) {
    ComputationResult result = {
        .succeed = true,
        .isDefinitionSet = false,
        .type = STATE_DEFINITION
    };
    
    if ( set->identifier != NULL ) {
        EntryResult result = getValue(set->identifier, set->isFromAutomata? AUTOMATA_STATES: STATES );
        set = result.stateSet;
    }
    if ( set->stateType != MIXED )
        filterStates(set, set->stateType);
    result->stateSet = set;
    return result;
}

ComputationResult computeSymbolSet(SymbolSet* set) {
    ComputationResult result = {
        .succeed = true,
        .isDefinitionSet = false,
        .type = ALPHABET_DEFINITION        
    };
    
    if ( set->identifier != NULL ) {
        EntryResult result = getValue(set->identifier, set->isFromAutomata? AUTOMATA_ALPHABET: ALPHABET );
        set = result.stateSet;
    }

    result->symbolSet = set;
    return result;
}

ComputationResult computeTransition(Transition* transition, boolean isSingleElement){
    
    StateExpression * stateExpression = transition->fromExpression;
    ComputationResult result = computeStateExpression(stateExpression,true);
    stateExpression->stateSet = result.stateSet; 
    stateExpression->type = SET_EXPRESSION;
    
    SymbolExpression * symbolExpression = transition->symbolExpression;
    result = computeSymbolExpression(symbolExpression,true);
    symbolExpression->symbolSet = result.symbolSet;
    symbolExpression->type = SET_EXPRESSION;

    stateExpression = transition->toExpression;
    result = computeStateExpression(stateExpression,true);
    stateExpression->stateSet = result.stateSet;
    stateExpression->type = SET_EXPRESSION;
    
    ComputationResult computationResult = {
		.succeed = true
	};

    if ( isSingleElement ){
        computationResult.transition = transition;
    } else {
        TransitionSet * set = malloc(sizeof(TransitionSet));
        TransitionNode * node = malloc(sizeof(TransitionNode));
        node->type = EXPRESSION;
        set->first = node; 
        set->tail = node;   
        computationResult.transitionSet = set;
    }
    
    return computationResult;
}

ComputationResult computeSymbol(Symbol* symbol, boolean isSingleElement) {

    ComputationResult computationResult = {
		.succeed = true
	};

    if ( isSingleElement ){
        computationResult.symbol = symbol;
    } else {
        SymbolSet * set = malloc(sizeof(SymbolSet));
        SymbolNode * node = malloc(sizeof(SymbolNode));
        node->type = EXPRESSION;
        set->first = node;  
        set->tail = node;  
        computationResult.symbolSet = set;
    }
    
    return computationResult;
}

ComputationResult computeState(State* state, boolean isSingleElement ) {

    ComputationResult computationResult = {
		.succeed = true
	};

    if ( isSingleElement ){
        computationResult.state = state;
    } else {
        StateSet * set = malloc(sizeof(StateSet));
        StateNode * node = malloc(sizeof(StateNode));
        node->type = EXPRESSION;
        set->first = node;
        set->tail = node;  
        computationResult.stateSet = set;
    }
    
    return computationResult;
}

/*-----------------PRIVATE FUNCTIONS ---------------------------------------------*/

/*-----------------------------SET OPERATIONS --------------------------------------------*/

/*----------------------------- UNION -----------------------------------------------*/
ComputationResult _transitionUnion(TransitionExpression leftExp, TransitionExpression rightExp){
    ComputationResult left = computeTransitionExpression(leftExp, false);
    ComputationResult right = computeTransitionExpression(rightExp, false);
    if (left.succeed && right.succeed){
        TransitionSet * result = calloc(sizeof(TransitionSet));
        result->first = leftSet->fist;
        leftSet->tail->next = rightSet->first; 
        result->tail = rightSet->tail;
        return computeTransitionSet(result);
    }
    return _invalidComputation();
}

ComputationResult _stateUnion(StateExpression leftExp, StateExpression rightExp){
    ComputationResult left = computeStateExpression(leftExp, false);
    ComputationResult right = computeStateExpression(rightExp, false);
    if (left.succeed && right.succeed){
        StateSet * result = calloc(sizeof(StateSet));
        result->first = leftSet->fist;
        leftSet->tail->next = rightSet->first; 
        result->tail = rightSet->tail;
        return computeStateSet(result);
    }
    return _invalidComputation();
}

ComputationResult _symbolUnion(SymbolExpression leftExp, SymbolExpression rightExp){
    ComputationResult left = computeSymbolExpression(leftExp, false);
    ComputationResult right = computeSymbolExpression(rightExp, false);
    if (left.succeed && right.succeed){
        SymbolSet * result = calloc(sizeof(SymbolSet));
        result->first = leftSet->fist;
        leftSet->tail->next = rightSet->first; 
        result->tail = rightSet->tail;
        return computeSymbolSet(result);
    }
    return _invalidComputation();
}

/*------------------------------ INTERSECTION -------------------------------------*/
ComputationResult _transitionIntersection(TransitionExpression leftExp, TransitionExpression rightExp){
    ComputationResult left = computeTransitionExpression(leftExp, false);
    ComputationResult right = computeTransitionExpression(rightExp, false);
    if (left.succeed && right.succeed){
        TransitionSet * result = malloc(sizeof(TransitionSet));
        _transitionIntersectionResolution(left.transitionSet, right.transitionSet, result);
        ComputationResult computationResult = {
            .succeed = true,
            .transitionSet = result
        };
        return computationResult;
    }
    return _invalidComputation();
}

static void _transitionIntersectionResolution(TransitionSet * leftSet, TransitionSet * rightSet, TransitionSet * result){
    TransitionNode * leftCurrentNode = leftSet->first;
    TransitionNode * rightCurrentNode = rightSet->first;
    TransitionNode * pivotNode = NULL;
    TransitionNode * resultCurrentNode = NULL; 
    result->first = resultCurrentNode;
    int found = 0;
    if (leftSet->tail == NULL || rightSet->tail == NULL){
        return;
    }
    pivotNode = leftCurrentNode;
    while (rightCurrentNode != rightSet->tail && !found){
            if (transitionEquals(pivotNode->transition, rightCurrentNode->transition)){
                found = 1;
                resultCurrentNode = calloc(sizeof(TransitionNode));
                resultCurrentNode->transition = leftCurrentNode->transition;
                resultCurrentNode = resultCurrentNode->next;
                break;
            }
            rightCurrentNode = rightCurrentNode->next;
        }
        if (!found){
            if(transitionEquals(pivotNode->transition, rightCurrentNode->transition)){
                found = 1;
                resultCurrentNode = calloc(sizeof(TransitionNode));
                resultCurrentNode->transition = leftCurrentNode->transition;
                resultCurrentNode = resultCurrentNode->next;
            }
    }
    rightCurrentNode = rightSet->first;
    do {
        leftCurrentNode = leftCurrentNode->next;
        //dejo un nodo para comparar contra todos los nodos del otro set
        pivotNode = leftCurrentNode;
        while (rightCurrentNode != rightSet->tail && !found){
            if (transitionEquals(pivotNode->transition, rightCurrentNode->transition)){
                found = 1;
                resultCurrentNode = calloc(sizeof(TransitionNode));
                resultCurrentNode->transition = leftCurrentNode->transition;
                resultCurrentNode = resultCurrentNode->next;
                break;
            }
            rightCurrentNode = rightCurrentNode->next;
        }
        if (!found){
            if(transitionEquals(pivotNode->transition, rightCurrentNode->transition)){
                found = 1;
                resultCurrentNode = calloc(sizeof(TransitionNode));
                resultCurrentNode->transition = leftCurrentNode->transition;
                resultCurrentNode = resultCurrentNode->next;
            }
        }
        rightCurrentNode = rightSet->first;
    }
    while (leftCurrentNode != leftSet->tail && !found);
    // busco el último nodo
    TransitionNode * lastNode = result->first;
    while(lastNode->next != NULL){
        lastNode = lastNode->next;
    }
    result->tail = lastNode;
    //free de los nodos de left y right sets
}

static ComputationResult _stateIntersection(StateExpression leftExp, StateExpression rightExp){
    ComputationResult left = computeStateExpression(leftExp, false);
    ComputationResult right = computeStateExpression(rightExp, false);
    if (left.succeed && right.succeed){
        StateSet * result = malloc(sizeof(StateSet));
        _stateIntersectionResolution(left.stateSet, right.stateSet, result);
        ComputationResult computationResult = {
            .succeed = true,
            .stateSet = result
        };
        return computationResult;
    }
    return _invalidComputation();
}

static void _stateIntersectionResolution(StateSet * leftSet, StateSet * rightSet, StateSet * result){
    StateNode * leftCurrentNode = leftSet->first;
    StateNode * rightCurrentNode = rightSet->first;
    StateNode * pivotNode = NULL;
    StateNode * resultCurrentNode = NULL; 
    result->first = resultCurrentNode;
    int found = 0;
    if (leftSet->tail == NULL || rightSet->tail == NULL){
        return;
    }
    pivotNode = leftCurrentNode;
    while (rightCurrentNode != rightSet->tail && !found){
            if (stateEquals(pivotNode->state, rightCurrentNode->state)){
                found = 1;
                resultCurrentNode = calloc(sizeof(StateNode));
                resultCurrentNode->state = leftCurrentNode->state;
                resultCurrentNode = resultCurrentNode->next;
                break;
            }
            rightCurrentNode = rightCurrentNode->next;
        }
        if (!found){
            if(stateEquals(pivotNode->state, rightCurrentNode->state)){
                found = 1;
                resultCurrentNode = calloc(sizeof(StateNode));
                resultCurrentNode->state = leftCurrentNode->state;
                resultCurrentNode = resultCurrentNode->next;
            }
    }
    rightCurrentNode = rightSet->first;
    do {
        leftCurrentNode = leftCurrentNode->next;
        //dejo un nodo para comparar contra todos los nodos del otro set
        pivotNode = leftCurrentNode;
        while (rightCurrentNode != rightSet->tail && !found){
            if (stateEquals(pivotNode->state, rightCurrentNode->state)){
                found = 1;
                resultCurrentNode = calloc(sizeof(StateNode));
                resultCurrentNode->state = leftCurrentNode->state;
                resultCurrentNode = resultCurrentNode->next;
                break;
            }
            rightCurrentNode = rightCurrentNode->next;
        }
        if (!found){
            if(stateEquals(pivotNode->state, rightCurrentNode->state)){
                found = 1;
                resultCurrentNode = calloc(sizeof(StateNode));
                resultCurrentNode->state = leftCurrentNode->state;
                resultCurrentNode = resultCurrentNode->next;
            }
        }
        rightCurrentNode = rightSet->first;
    }
    while (leftCurrentNode != leftSet->tail && !found);
    // busco el último nodo
    StateNode * lastNode = result->first;
    while(lastNode->next != NULL){
        lastNode = lastNode->next;
    }
    result->tail = lastNode;
    //free de los nodos de left y right sets
}

static ComputationResult _symbolIntersection(SymbolExpression leftExp, SymbolExpression rightExp){
    ComputationResult left = computeSymbolExpression(leftExp, false);
    ComputationResult right = computeSymbolExpression(rightExp, false);
    if (left.succeed && right.succeed){
        SymbolSet * result = malloc(sizeof(SymbolSet));
        _symbolIntersectionResolution(left.symbolSet, right.symbolSet, result);
        ComputationResult computationResult = {
            .succeed = true,
            .symbolSet = result
        };
        return computationResult;
    }
    return _invalidComputation();
}

static void _symbolIntersectionResolution(SymbolSet * leftSet, SymbolSet * rightSet, SymbolSet * result){
    SymbolNode * leftCurrentNode = leftSet->first;
    SymbolNode * rightCurrentNode = rightSet->first;
    SymbolNode * pivotNode = NULL;
    SymbolNode * resultCurrentNode = NULL; 
    result->first = resultCurrentNode;
    int found = 0;
    if (leftSet->tail == NULL || rightSet->tail == NULL){
        return;
    }
    pivotNode = leftCurrentNode;
    while (rightCurrentNode != rightSet->tail && !found){
            if (symbolEquals(pivotNode->symbol, rightCurrentNode->symbol)){
                found = 1;
                resultCurrentNode = calloc(sizeof(SymbolNode));
                resultCurrentNode->symbol = leftCurrentNode->symbol;
                resultCurrentNode = resultCurrentNode->next;
                break;
            }
            rightCurrentNode = rightCurrentNode->next;
        }
        if (!found){
            if(symbolEquals(pivotNode->symbol, rightCurrentNode->symbol)){
                found = 1;
                resultCurrentNode = calloc(sizeof(SymbolNode));
                resultCurrentNode->symbol = leftCurrentNode->symbol;
                resultCurrentNode = resultCurrentNode->next;
            }
    }
    rightCurrentNode = rightSet->first;
    do {
        leftCurrentNode = leftCurrentNode->next;
        //dejo un nodo para comparar contra todos los nodos del otro set
        pivotNode = leftCurrentNode;
        while (rightCurrentNode != rightSet->tail && !found){
            if (symbolEquals(pivotNode->symbol, rightCurrentNode->symbol)){
                found = 1;
                resultCurrentNode = calloc(sizeof(SymbolNode));
                resultCurrentNode->symbol = leftCurrentNode->symbol;
                resultCurrentNode = resultCurrentNode->next;
                break;
            }
            rightCurrentNode = rightCurrentNode->next;
        }
        if (!found){
            if(symbolEquals(pivotNode->symbol, rightCurrentNode->symbol)){
                found = 1;
                resultCurrentNode = calloc(sizeof(SymbolNode));
                resultCurrentNode->symbol = leftCurrentNode->symbol;
                resultCurrentNode = resultCurrentNode->next;
            }
        }
        rightCurrentNode = rightSet->first;
    }
    while (leftCurrentNode != leftSet->tail && !found);
    // busco el último nodo
    SymbolNode * lastNode = result->first;
    while(lastNode->next != NULL){
        lastNode = lastNode->next;
    }
    result->tail = lastNode;
    //free de los nodos de left y right sets
}

/*-------------------------------------- DIFFERENCE -----------------------------*/
ComputationResult _transitionDifference(TransitionExpression leftExp, TransitionExpression rightExp){
    ComputationResult left = computeTransitionExpression(leftExp, false);
    ComputationResult right = computeTransitionExpression(rightExp, false);
    if (left.succeed && right.succeed){
        TransitionSet * result = malloc(sizeof(TransitionSet));
        _transitionDifferenceResolution(left.transitionSet, right.transitionSet, result);
        ComputationResult computationResult = {
            .succeed = true,
            .transitionSet = result
        };
        return computationResult;
    }
    return _invalidComputation();
}

static void _transitionDifferenceResolution(TransitionSet * leftSet, TransitionSet * rightSet, TransitionSet * result){
    TransitionNode * leftCurrentNode = leftSet->first;
    TransitionNode * rightCurrentNode = rightSet->first;
    TransitionNode * pivotNode = NULL;
    TransitionNode * resultCurrentNode = NULL; 
    result->first = resultCurrentNode;
    int found = 0;
    if (leftSet->tail == NULL){
        return;
    }
    if (rightSet->tail == NULL){
        result = leftSet;
        return;
    }
    pivotNode = leftCurrentNode;
    while (rightCurrentNode != rightSet->tail && !found){
            if (transitionEquals(pivotNode->transition, rightCurrentNode->transition)){
                found = 1;
                break;
            }
            rightCurrentNode = rightCurrentNode->next;
    }
    if (!found){
        if(!transitionEquals(pivotNode->transition, rightCurrentNode->transition)){
            resultCurrentNode = calloc(sizeof(TransitionNode));
            resultCurrentNode->transition = leftCurrentNode->transition;
            resultCurrentNode = resultCurrentNode->next;
        }
    }
    rightCurrentNode = rightSet->first;
    do {
        leftCurrentNode = leftCurrentNode->next;
        //dejo un nodo para comparar contra todos los nodos del otro set
        pivotNode = leftCurrentNode;
        while (rightCurrentNode != rightSet->tail && !found){
            if (transitionEquals(pivotNode->transition, rightCurrentNode->transition)){
                found = 1;
                break;
            }
            rightCurrentNode = rightCurrentNode->next;
        }
        if (!found){
            if(!transitionEquals(pivotNode->transition, rightCurrentNode->transition)){
                resultCurrentNode = calloc(sizeof(TransitionNode));
                resultCurrentNode->transition = leftCurrentNode->transition;
                resultCurrentNode = resultCurrentNode->next;
            }
        }
        rightCurrentNode = rightSet->first;
    }
    while (leftCurrentNode != leftSet->tail && !found);
    //free the nodes from left and right sets
    TransitionNode * lastNode = result->first;
    while(lastNode->next != NULL){
        lastNode = lastNode->next;
    }
    result->tail = lastNode;

}

static ComputationResult _stateDifference(StateExpression leftExp, StateExpression rightExp){
    ComputationResult left = computeStateExpression(leftExp, false);
    ComputationResult right = computeStateExpression(rightExp, false);
    if (left.succeed && right.succeed){
        StateSet * result = malloc(sizeof(StateSet));
        _stateDifferenceResolution(left.stateSet, right.stateSet, result);
        ComputationResult computationResult = {
            .succeed = true,
            .stateSet = result
        };
        return computationResult;
    }
    return _invalidComputation();
}

static void _stateDifferenceResolution(StateSet * leftSet, StateSet * rightSet, StateSet * result){
    StateNode * leftCurrentNode = leftSet->first;
    StateNode * rightCurrentNode = rightSet->first;
    StateNode * pivotNode = NULL;
    StateNode * resultCurrentNode = NULL; 
    result->first = resultCurrentNode;
    int found = 0;
    if (leftSet->tail == NULL){
        return;
    }
    if (rightSet->tail == NULL){
        result = leftSet;
        return;
    }
    pivotNode = leftCurrentNode;
    while (rightCurrentNode != rightSet->tail && !found){
            if (stateEquals(pivotNode->state, rightCurrentNode->state)){
                found = 1;
                break;
            }
            rightCurrentNode = rightCurrentNode->next;
    }
    if (!found){
        if(!stateEquals(pivotNode->state, rightCurrentNode->state)){
            resultCurrentNode = calloc(sizeof(StateNode));
            resultCurrentNode->state = leftCurrentNode->state;
            resultCurrentNode = resultCurrentNode->next;
        }
    }
    rightCurrentNode = rightSet->first;
    do {
        leftCurrentNode = leftCurrentNode->next;
        //dejo un nodo para comparar contra todos los nodos del otro set
        pivotNode = leftCurrentNode;
        while (rightCurrentNode != rightSet->tail && !found){
            if (stateEquals(pivotNode->state, rightCurrentNode->state)){
                found = 1;
                break;
            }
            rightCurrentNode = rightCurrentNode->next;
        }
        if (!found){
            if(!stateEquals(pivotNode->state, rightCurrentNode->state)){
                resultCurrentNode = calloc(sizeof(StateNode));
                resultCurrentNode->state = leftCurrentNode->state;
                resultCurrentNode = resultCurrentNode->next;
            }
        }
        rightCurrentNode = rightSet->first;
    }
    while (leftCurrentNode != leftSet->tail && !found);
    //free the nodes from left and right sets
    StateNode * lastNode = result->first;
    while(lastNode->next != NULL){
        lastNode = lastNode->next;
    }
    result->tail = lastNode;
}

static ComputationResult _symbolDifference(SymbolExpression leftExp, SymbolExpression rightExp){
    ComputationResult left = computeSymbolExpression(leftExp, false);
    ComputationResult right = computeSymbolExpression(rightExp, false);
    if (left.succeed && right.succeed){
        SymbolSet * result = malloc(sizeof(SymbolSet));
        _symbolDifferenceResolution(left.symbolSet, right.symbolSet, result);
        ComputationResult computationResult = {
            .succeed = true,
            .symbolSet = result
        };
        return computationResult;
    }
    return _invalidComputation();
}

static void _symbolDifferenceResolution(SymbolSet * leftSet, SymbolSet * rightSet, SymbolSet * result){
    SymbolNode * leftCurrentNode = leftSet->first;
    SymbolNode * rightCurrentNode = rightSet->first;
    SymbolNode * pivotNode = NULL;
    SymbolNode * resultCurrentNode = NULL; 
    result->first = resultCurrentNode;
    int found = 0;
    if (leftSet->tail == NULL){
        return;
    }
    if (rightSet->tail == NULL){
        result = leftSet;
        return;
    }
    pivotNode = leftCurrentNode;
    while (rightCurrentNode != rightSet->tail && !found){
            if (symbolEquals(pivotNode->symbol, rightCurrentNode->symbol)){
                found = 1;
                break;
            }
            rightCurrentNode = rightCurrentNode->next;
    }
    if (!found){
        if(!symbolEquals(pivotNode->symbol, rightCurrentNode->symbol)){
            resultCurrentNode = calloc(sizeof(SymbolNode));
            resultCurrentNode->symbol = leftCurrentNode->symbol;
            resultCurrentNode = resultCurrentNode->next;
        }
    }
    rightCurrentNode = rightSet->first;
    do {
        leftCurrentNode = leftCurrentNode->next;
        //dejo un nodo para comparar contra todos los nodos del otro set
        pivotNode = leftCurrentNode;
        while (rightCurrentNode != rightSet->tail && !found){
            if (symbolEquals(pivotNode->symbol, rightCurrentNode->symbol)){
                found = 1;
                break;
            }
            rightCurrentNode = rightCurrentNode->next;
        }
        if (!found){
            if(!symbolEquals(pivotNode->symbol, rightCurrentNode->symbol)){
                resultCurrentNode = calloc(sizeof(SymbolNode));
                resultCurrentNode->symbol = leftCurrentNode->symbol;
                resultCurrentNode = resultCurrentNode->next;
            }
        }
        rightCurrentNode = rightSet->first;
    }
    while (leftCurrentNode != leftSet->tail && !found);
    //free the nodes from left and right sets
    SymbolNode * lastNode = result->first;
    while(lastNode->next != NULL){
        lastNode = lastNode->next;
    }
    result->tail = lastNode;
}

/*--------------------------------------------- INVALID OPERATORS -----------------------------------------*/
static ComputationResult _invalidBinaryOperator(TransitionExpression leftExp, TransitionExpression rightExp) {
	return _invalidComputation();
}

/**
 * A computation that always returns an invalid result.
 */
static ComputationResult _invalidComputation() {
	ComputationResult computationResult = {
        .succeed = false,
    };
    return computationResult;
}
	

/*-------------------------------------- FREE NODE SETS --------------------*/
static void freeTransitionSet(TransitionSet * set){
    TransitionNode * current = set->first;
    TransitionNode * next;
    while (current != NULL){
        next = current->next;
        free(current);
        current = next;
    }
    free(set);
}

static void freeStateSet(StateSet * set){
    StateNode * current = set->first;
    StateNode * next;
    while (current != NULL){
        next = current->next;
        free(current);
        current = next;
    }
    free(set);
}

static void freeSymbolSet(SymbolSet * set){
    SymbolNode * current = set->first;
    SymbolNode * next;
    while (current != NULL){
        next = current->next;
        free(current);
        current = next;
    }
    free(set);
}

/*----------------------------------------- SET EQUALS ----------------------------------------------------*/ 
static boolean transitionSetEquals(TransitionSet * set1, TransitionSet * set2){
    TransitionNode * current1 = set1->first;
    TransitionNode * current2 = set2->first;
    TransitionNode * pivot = current1;
    int found;
    if (set1->tail == NULL && set2->tail != NULL | set1->tail != NULL && set2->tail == NULL){
        return false;
    }
    found = transitionEquals(pivot->transition, current2->transition)
    if (!found){
        do{
            current2 = current2->next;
            if (transitionEquals(pivot->transition, current2->transition)){
                found=1;
                break;
            }
        }while(current2 != set2->tail && found);
        if (!found){
            return false;
        }
    }
    current2 = set2->first;
    do{
        found = 0;
        current1 = current1->next;
        pivot = current1;
        while (current2 != set2->tail){
            if (transitionEquals(pivot->transition, current2->transition)){
                found=1;
                break;
            }
            current2 = current2->next;
        }
        if (!found){
            if (transitionEquals(pivot->transition, current2->transition)){
                found=1;
            }
        }
        current2 = set2->first;
    }while(current1 != set1->tail && found);
    return found;
}


static boolean stateSetEquals(StateSet * set1, StateSet * set2){
    StateNode * current1 = set1->first;
    StateNode * current2 = set2->first;
    StateNode * pivot = current1;
    int found;
    if (set1->tail == NULL && set2->tail != NULL | set1->tail != NULL && set2->tail == NULL){
        return false;
    }
    //veo el primer caso aparte porque después me pongo a revisar desde el siguiente en el do-while
    //Esto lo hago para que me tome el último caso adentro del ciclo
    found = stateEquals(pivot->state, current2->state)
    if (!found){
        do{
            current2 = current2->next;
            if (stateEquals(pivot->state, current2->state)){
                found=1;
                break;
            }
        }while(current2 != set2->tail && found);
        //si ya hay uno que no está salgo de una sin hacer el siguiente do-while
        if (!found){
            return false;
        }
    }
    current2 = set2->first;
    do{
        found = 0;
        current1 = current1->next;
        pivot = current1;
        while (current2 != set2->tail){
            if (stateEquals(pivot->state, current2->state)){
                found=1;
                break;
            }
            current2 = current2->next;
        }
        //veo el caso de que haya llegado al ultimo y no lo encontré antes
        if (!found){
            if (stateEquals(pivot->state, current2->state)){
                found=1;
            }
        }
        current2 = set2->first;
    }while(current1 != set1->tail && found);
    return found;
}

static boolean symbolSetEquals(SymbolSet *set1, SymbolSet *set2){
    SymbolNode * current1 = set1->first;
    SymbolNode * current2 = set2->first;
    SymbolNode * pivot = current1;
    int found;
    if (set1->tail == NULL && set2->tail != NULL | set1->tail != NULL && set2->tail == NULL){
        return false;
    }
    found = symbolEquals(pivot->symbol, current2->symbol)
    if (!found){
        do{
            current2 = current2->next;
            if (symbolEquals(pivot->symbol, current2->symbol)){
                found=1;
                break;
            }
        }while(current2 != set2->tail && found);
        if (!found){
            return false;
        }
    }
    current2 = set2->first;
    do{
        found = 0;
        current1 = current1->next;
        pivot = current1;
        while (current2 != set2->tail){
            if (symbolEquals(pivot->symbol, current2->symbol)){
                found=1;
                break;
            }
            current2 = current2->next;
        }
        if (!found){
            if (stateEquals(pivot->state, current2->state)){
                found=1;
            }
        }
        current2 = set2->first;
    }while(current1 != set1->tail && found);
    return found;
}

/*------------------- ELEMENT EQUALS --------------------------------------------*/
static boolean transitionEquals(Transition * trans1, Transition * trans2){
    TransitionSet * trans1From = trans1->fromExpression->stateSet;
    TransitionSet * trans2From = trans2->fromExpression->stateSet;
    TransitionSet * trans1To = trans1->toExpression->stateSet;
    TransitionSet * trans2To = trans2->toExpression->stateSet;
    return stateSetEquals(trans1From, trans2From) && stateSetEquals(trans1To, trans2To) && symbolSetEquals(trans1->symbolExpression->symbolSet, trans2->symbolExpression->symbolSet);
}

static boolean stateEquals(State * state1, State * state2){
    return  symbolEquals(state1->symbol, state2->symbol) && state1->isFinal == state2->isFinal && state1->isInitial == state2->isInitial;
}

static boolean symbolEquals(Symbol * symbol1, Symbol * symbol2){
    return strcmp(symbol1->value, symbol2->value) == 0;
}
