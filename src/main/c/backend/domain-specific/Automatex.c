#include "Automatex.h"


static Logger * _logger = NULL;

/*-----------------PRIVATE FUNCTIONS ---------------------------------------------*/

static ComputationResult _computeFinalAndInitialStates(StateSet * set, Automata * automata);
static ComputationResult _checkAutomataRequirements(TransitionSet * transitions, StateSet * states, SymbolSet * alphabet, AutomataType automataType);
static void _filterStates( StateSet * set, StateType type);
static ComputationResult _checkTransitionStatesAndSymbols(TransitionSet * transitions, StateSet * states, SymbolSet * alphabet);
static ComputationResult containsLambda(SymbolSet * alphabet, AutomataType type);
static ComputationResult _isDFA(TransitionSet * transitions);

/*-----------------------------SET OPERATIONS --------------------------------------------*/

/*----------------------------- UNION -----------------------------------------------*/
static ComputationResult _transitionUnion(TransitionExpression * leftExp, TransitionExpression * rightExp);
static ComputationResult _stateUnion(StateExpression * leftExp, StateExpression * rightExp);
static ComputationResult _stateSetUnion(StateSet* leftSet, StateSet* rightSet);
static ComputationResult _symbolUnion(SymbolExpression * leftExp, SymbolExpression * rightExp);
static ComputationResult _symbolSetUnion(SymbolSet* leftSet, SymbolSet* rightSet);
/*------------------------------ INTERSECTION -------------------------------------*/
static ComputationResult _transitionIntersection(TransitionExpression * leftExp, TransitionExpression * rightExp);
static void _transitionIntersectionResolution(TransitionSet * leftSet, TransitionSet * rightSet, TransitionSet * result);
static ComputationResult _stateIntersection(StateExpression * leftExp, StateExpression * rightExp);
static void _stateIntersectionResolution(StateSet * leftSet, StateSet * rightSet, StateSet * result);
static ComputationResult _symbolIntersection(SymbolExpression * leftExp, SymbolExpression * rightExp);
static void _symbolIntersectionResolution(SymbolSet * leftSet, SymbolSet * rightSet, SymbolSet * result);
/*-------------------------------------- DIFFERENCE -----------------------------*/
static ComputationResult _transitionDifference(TransitionExpression * leftExp, TransitionExpression * rightExp);
static void _transitionDifferenceResolution(TransitionSet * leftSet, TransitionSet * rightSet, TransitionSet * result);
static ComputationResult _stateDifference(StateExpression * leftExp, StateExpression * rightExp);
static void _stateDifferenceResolution(StateSet * leftSet, StateSet * rightSet, StateSet * result);
static ComputationResult _symbolDifference(SymbolExpression * leftExp, SymbolExpression * rightExp);
static void _symbolDifferenceResolution(SymbolSet * leftSet, SymbolSet * rightSet, SymbolSet * result);

/*---------------------------------------------- DELETE REPETITIONS FROM SET -----------------------------------------*/
static void deleteRepetitionsFromTransitionSet(TransitionSet * set);
static void deleteRepetitionsFromStateSet(StateSet * set);
static void deleteRepetitionsFromSymbolSet(SymbolSet * set);

/*--------------------------------------------- INVALID OPERATORS -----------------------------------------*/
static ComputationResult _invalidComputation();


 void initializeAutomatexModule() {
	_logger = createLogger("Automatex");
    initializeTable();
}

void shutdownAutomatexModule() {
    if (_logger != NULL) {
		destroyLogger(_logger);
	}
}

ComputationResult computeDefinitionSet(DefinitionSet * definitionSet) {
    ComputationResult result = {
        .succeed = false,
        .isDefinitionSet = true
    };
    
    DefinitionNode * currentNode = definitionSet->first;
    while (currentNode != definitionSet->tail){
        ComputationResult result1 = computeDefinition(currentNode->definition);
        if ( !result1.succeed ){
            logError(_logger, "There has been a problem while processing a definition");
            return result;
        }
        currentNode = currentNode->next;
    }
    //para el ultimo nodo
    ComputationResult result2 = computeDefinition(currentNode->definition);
    if ( !result2.succeed ){
        logError(_logger, "There has been a problem while processing a definition");
        return result;
    }
    result.succeed = true;
    result.definitionSet = definitionSet;
    return result;
}

ComputationResult computeDefinition(Definition * definition) {
    ComputationResult result = {
        .isDefinitionSet = true,
        .succeed = false
    };
    // valido si ya existe en la tabla de simbolos
    // sino luego de computarlo lo inserto
    Value value;
    char * identifier;
    switch (definition->type) {
        case AUTOMATA_DEFINITION:
            identifier = definition->automata->identifier;
            if ( !exists(identifier) ) {
                result= computeAutomata(definition->automata);
                if (!result.succeed){
                    return result;
                }
                definition->automata = result.automata;
                value.automata=definition->automata; 
            
            }else{
                //error redefinition
                return _invalidComputation();
                /* me gustaría buscar si con un identifier distinto aparece 
                value = getValue(identifier, AUTOMATA);            else{
                definition->automata = result.automata;
                */
            }
            break;
        case TRANSITION_DEFINITION:
            identifier = definition->transitionSet->identifier;
            if ( !exists(identifier) ) {
                result = computeTransitionSet(definition->transitionSet,true);
                definition->transitionSet = result.transitionSet;
                value.transitionSet = definition->transitionSet;
            }
            break;
        case ALPHABET_DEFINITION:
            identifier = definition->symbolSet->identifier;
            if ( !exists(identifier) ) {
                result = computeSymbolSet(definition->symbolSet,true);
                definition->symbolSet = result.symbolSet;
                value.symbolSet = definition->symbolSet;
            }
            break;
        case STATE_DEFINITION:
            identifier = definition->stateSet->identifier;
            logWarning(_logger, "Identifier: %s", identifier);
            if ( !exists(identifier) ) {
                result = computeStateSet(definition->stateSet,true);
                logInformation(_logger,"Completed creation of set");
                definition->stateSet = result.stateSet;
                value.stateSet = definition->stateSet;
            }
            else{
                return _invalidComputation();
            }
            break;
        default:
            return _invalidComputation();
    }
    
    if ( !result.succeed ) {
        if ( result.isDefinitionSet )
            logError(_logger,"There cannot be 2 definitions with the same name");
        return _invalidComputation();
    }
    logInformation(_logger,"Completed definition of set");
    result.succeed = insert(identifier,definition->type,value); 
    if ( result.succeed )
        logInformation(_logger,"The definition was added to the symbol table");
    else
        logError(_logger,"The definition COUDNT BE added to the symbol table");
    return result;

}

ComputationResult computeAutomata(Automata * automata) {
    ComputationResult result = {
        .succeed = false,
        .isDefinitionSet = false,
    };
    ComputationResult stateSetResult = computeStateExpression(automata->states, true);
    if ( !stateSetResult.succeed ){
        return result;   
    }
    
    result = _computeFinalAndInitialStates(stateSetResult.stateSet, automata);
    if ( !result.succeed ){
        return result;   
    }
    logInformation(_logger,"-----computed final and initial states-----");
    ComputationResult symbolSetResult = computeSymbolExpression(automata->alphabet, true);
    if ( !symbolSetResult.succeed ){
        return result;   
    }
    logInformation(_logger,"-----computed symbols -----");
    ComputationResult transitionSetResult = computeTransitionExpression(automata->transitions, true);
    if ( !transitionSetResult.succeed ){
        return result;   
    }
    logInformation(_logger,"-----computed transitions-----");
    result = _checkAutomataRequirements(transitionSetResult.transitionSet, stateSetResult.stateSet, symbolSetResult.symbolSet, automata->automataType);
    if ( !result.succeed ){
        return result;   
    }
    logInformation(_logger,"-----checked transitions-----");
    result.type = AUTOMATA_DEFINITION,
    result.automata = automata;
    result.succeed = true;
    return result;
}

/* chequea y arma estados final(es) e inicial */
static ComputationResult _computeFinalAndInitialStates(StateSet * set, Automata * automata){
    StateNode * currentNode = set->first;
    State * currentState;
    StateNode * finalTail;
    State * initialState = NULL; 
    StateSet * finalSet = calloc(1, sizeof(StateSet));
        logInformation(_logger, "BEFORE WHILE in computeFinalAndInitialStates");
    while ( currentNode != set->tail){
        currentState =  currentNode->state;
            logInformation(_logger, "IN WHILE in computeFinalAndInitialStates");
            logInformation(_logger, "value %s",currentState->symbol.value);
        if ( currentState->isFinal ) {
            logInformation(_logger, "found final state");
            StateNode * node = malloc(sizeof(StateNode));
            node->state = currentNode->state;
            if ( finalSet->first==NULL )
                finalSet->first = node;
            else 
                finalTail->next = node;
            finalTail = node; 
        }
        if ( currentState->isInitial ){
            if ( initialState != NULL ) {
                logError(_logger,"%s it has more than one initial state",AUTOMATA_NOT_CREATED);
                return _invalidComputation();
            }
            initialState = currentState;
        }  
        currentNode = currentNode->next; 
    }
    logInformation(_logger, "checking last element");
    currentState =  currentNode->state;
            logInformation(_logger, "IN WHILE in computeFinalAndInitialStates");
            logInformation(_logger, "value %s",currentState->symbol.value);
        if ( currentState->isFinal ) {
            logInformation(_logger, "found final state");
            StateNode * node = malloc(sizeof(StateNode));
            node->state = currentNode->state;
            if ( finalSet->first==NULL )
                finalSet->first = node;
            else 
                finalTail->next = node;
            finalTail = node; 
        }
        if ( currentState->isInitial ){
            if ( initialState != NULL ) {
                logError(_logger,"%s it has more than one initial state",AUTOMATA_NOT_CREATED);
                return _invalidComputation();
            }
            initialState = currentState;
        }  
    finalSet->tail = finalTail;

    if ( finalSet->first == NULL ) {
        logError(_logger,"%s it does not have final states", AUTOMATA_NOT_CREATED);
        return _invalidComputation();
    }
    if ( initialState == NULL ) {
        logError(_logger,"%s it does not have an initial state", AUTOMATA_NOT_CREATED);
        return _invalidComputation();
    }

    automata->initials = malloc(sizeof(StateExpression));
    automata->initials->state = initialState;
    automata->finals = malloc(sizeof(StateExpression));
    automata->finals->stateSet = finalSet; 
    ComputationResult result = { .succeed = true };


    return result; 
}

static ComputationResult _checkAutomataRequirements(TransitionSet * transitions, StateSet * states, SymbolSet * alphabet, AutomataType automataType) {
    logInformation(_logger, "checking automata requirements");
    ComputationResult result;
    
    if ( automataType!=LNFA_AUTOMATA ) {
        result = containsLambda(alphabet, automataType);
        if ( result.succeed)
            return result;
    }

    result = _checkTransitionStatesAndSymbols(transitions, states, alphabet);
    if (!result.succeed)
        return result;

    if ( automataType==DFA_AUTOMATA)
        return _isDFA(transitions);
    return result;
    
}

static ComputationResult _isDFA(TransitionSet * transitions) {
    TransitionNode * currentNode = transitions->first;
    TransitionNode * otherNode;
    State * fromState, * toState;
    Symbol * symbol;
    Transition * transition;
    while ( currentNode != NULL ){
        transition= currentNode->transition;
        fromState = transition->fromExpression->state;
        toState = transition->toExpression->state;
        symbol = transition->symbolExpression->symbol;
        otherNode = currentNode ->next;
        while ( otherNode!= NULL ){
            transition = otherNode->transition;
            if ( stateSymbolEquals(transition->fromExpression->state,fromState) && symbolEquals(transition->symbolExpression->symbol, symbol) 
                && !stateSymbolEquals(transition->toExpression->state,toState) ){    // ! tal vez se pueda sacar si no se sacaron los repetidos
                logError(_logger,"%s it is not deterministic due to the following transitions:",AUTOMATA_NOT_CREATED);
                logError(_logger,"\t|%s|-%s->|%s|", fromState->symbol.value, symbol->value, toState->symbol.value);
                logError(_logger,"\t|%s|-%s->|%s|", transition->fromExpression->state->symbol.value, transition->symbolExpression->symbol->value, transition->toExpression->state->symbol.value);
                return _invalidComputation();
            }
            otherNode = otherNode->next;
        }
        currentNode = currentNode->next; 
    }

    ComputationResult result = { .succeed = true };
    return result;
}

static ComputationResult containsLambda(SymbolSet * alphabet, AutomataType type)  {
    logInformation(_logger, "in contains Lambda");
    Symbol * lambda = malloc(sizeof(Symbol));
    lambda->value = LAMBDA_STRING; 
    ComputationResult result = {
        .succeed = containsSymbol(alphabet->first,lambda)
    };
    free(lambda);
    if ( !result.succeed ) 
        logInformation(_logger, "not lambda");
        return result;
    
    logError(_logger, "Lambda symbol was found in a %s automata", type==DFA_AUTOMATA? "DFA":"NFA");
    result.succeed = false;
    return result;
}

/* Chequeo si los simbolos y estados usados en las transiciones estan en la definicion del automata */
static ComputationResult _checkTransitionStatesAndSymbols(TransitionSet * transitions, StateSet * states, SymbolSet * alphabet)  {
    logInformation(_logger, "checking transitions");
    TransitionNode * currentTransNode = transitions->first;
    Transition * transition;
    ComputationResult result ={ .succeed=false };
    // todo: state or symbol set vacios =
    
    while (currentTransNode != NULL) {
        transition = currentTransNode->transition;
    
        if ( !containsState(states->first,transition->fromExpression->state ) || !containsState(states->first,transition->toExpression->state ) ) {
            logError(_logger,"%s its transitions use states that don't belong to the automata", AUTOMATA_NOT_CREATED);
            return result;
        }
        if ( !containsSymbol(alphabet->first,transition->symbolExpression->symbol )) {
            logError(_logger,"%s its transitions use symbols that don't belong to the automata", AUTOMATA_NOT_CREATED);
            return result;
        }

        currentTransNode = currentTransNode->next;
    }
    result.succeed = true;
    return result;
}

ComputationResult computeTransitionExpression(TransitionExpression * expression, boolean isSingleElement ){
    ComputationResult result;
    switch ( expression->type) {
        case UNION_EXPRESSION:
                logInformation(_logger, "Union transition expression");
                return _transitionUnion(expression->leftExpression, expression->rightExpression);
                        break;
        case INTERSECTION_EXPRESSION:
                return _transitionIntersection(expression->leftExpression, expression->rightExpression);
                        break;
        case DIFFERENCE_EXPRESSION:
                return _transitionDifference(expression->leftExpression, expression->rightExpression);
                        break;
        case SET_EXPRESSION:
                ComputationResult resultSet = computeTransitionSet(expression->transitionSet,false);
                return resultSet;
                        break;
        case ELEMENT_EXPRESSION: 
                logInformation(_logger, "computing transition object");
                ComputationResult resultElement = computeTransition(expression->transition, isSingleElement);
                if (resultElement.isSingleElement){
                    expression->transition = resultElement.transition;
                }else{
                    expression->transitionSet = resultElement.transitionSet;
                }
                return resultElement;
                        break;
        default:
            return _invalidComputation();
    }
}

ComputationResult computeStateExpression(StateExpression * expression,  boolean isSingleElement) {
    switch ( expression->type) {
        case UNION_EXPRESSION:
                    return _stateUnion(expression->leftExpression, expression->rightExpression);
                        break;
        case INTERSECTION_EXPRESSION:
                    return _stateIntersection(expression->leftExpression, expression->rightExpression);
                        break;
        case DIFFERENCE_EXPRESSION:
                    return _stateDifference(expression->leftExpression, expression->rightExpression);
                        break;
        case SET_EXPRESSION:
                logInformation(_logger, "The expression is a stateSet");
                ComputationResult resultSet = computeStateSet(expression->stateSet,false);
                //if (resultSet.stateSet->first == resultSet.stateSet->tail){
                //    expression->state = resultSet.stateSet->first->state;
                //    logInformation(_logger, "There is only one state: %s", expression->state->symbol.value);
                //}
                return resultSet;
                        break;
        case ELEMENT_EXPRESSION:
                ComputationResult resultElement = computeState(expression->state, isSingleElement);
                return resultElement;
                        break;
        default:
            return _invalidComputation();
    }
}


ComputationResult computeSymbolExpression(SymbolExpression * expression, boolean isSingleElement) {
    switch ( expression->type) {
        case UNION_EXPRESSION:
                return _symbolUnion(expression->leftExpression, expression->rightExpression);
                        break;
        case INTERSECTION_EXPRESSION:
                return _symbolIntersection(expression->leftExpression, expression->rightExpression);
                        break;
        case DIFFERENCE_EXPRESSION:
                return _symbolDifference(expression->leftExpression, expression->rightExpression);
                        break;
        case SET_EXPRESSION:
                //if (resultSet.symbolSet->first == resultSet.symbolSet->tail){
                //    expression->symbol = resultSet.symbolSet->first->symbol;
                //    logInformation(_logger, "There is only one symbol: %s", expression->symbol->value);
                //}
                return computeSymbolSet(expression->symbolSet,false);
        case ELEMENT_EXPRESSION:
                ComputationResult resultElement = computeSymbol(expression->symbol, isSingleElement) ;
                return resultElement;
                break;
        default:
            return _invalidComputation();
    }
}

ComputationResult computeTransitionSet(TransitionSet* set, boolean isDefinition) {
    ComputationResult result = {
        .succeed = true,
        .isDefinitionSet = false,
        .type = TRANSITION_DEFINITION
    };
    logInformation(_logger,"-----starting transition set creation -----");
    if ( set->isBothSidesTransition ) {
        logInformation(_logger, "bothsides transition");
//        logInformation(_logger, "Transition: %s -> %s -> %s", set->tail->transitionExpression->transition->fromExpression->state->symbol.value, set->tail->transitionExpression->transition->symbolExpression->symbol->value, set->tail->transitionExpression->transition->toExpression->state->symbol.value);        
//        logInformation(_logger, "Transition: %s -> %s -> %s", set->first->transitionExpression->transitionSet->first->transition->fromExpression->stateSet->first->state->symbol.value, set->first->transitionExpression->transitionSet->first->transition->symbolExpression->symbol->value, set->first->transitionExpression->transition->toExpression->state->symbol.value);
        ComputationResult result1, result2;
        result1 = computeTransitionExpression(set->first->transitionExpression, true);
        if (result1.succeed){
            if (result1.isSingleElement){
                //solo reemplazo la expresion por un elemento suelto
                set->first->transition = result1.transition;
                set->first->type = ELEMENT;
            }
            else{
                //cuando se tiene que una transicion representa varias al mismo tiempo
                //en vez de almacenarlo como un subset tomo los nodos y los conecto con el set al que forman parte como elementos sueltos
                TransitionNode * originalNext = set->first->next;
                set->first = result1.transitionSet->first; 
                result1.transitionSet->tail->next = originalNext;
            }
            result2 = computeTransitionExpression(set->tail->transitionExpression, true);
            if (result2.succeed){
                if (result2.isSingleElement){
                    //solo reemplazo la expresion por un elemento suelto
                    //set->first->next = set->tail;
                    logInformation(_logger, "I'm still standing in result2");
                    logInformation(_logger, "Transition: %s -> %s -> %s", result2.transition->fromExpression->state->symbol.value, result2.transition->symbolExpression->symbol->value, result2.transition->toExpression->state->symbol.value);
                    set->tail->transition = result2.transition;
                    set->tail->type = ELEMENT;
                }
                else{
                    logInformation(_logger, "I received a transitionSet");
                    //cuando se tiene que una transicion representa varias al mismo tiempo
                    //en vez de almacenarlo como un subset tomo los nodos y los conecto con el set al que forman parte como elementos sueltos
                    result1.transitionSet->tail->next = result2.transitionSet->first; 
                    set->tail = result2.transitionSet->tail;
                }
            }
        }
        if (!result1.succeed || !result2.succeed){
            logError(_logger, "Error when creating both sides transition");
            return _invalidComputation();
        }
    }      
            
    if ( set->identifier != NULL && !isDefinition ) {
        logInformation(_logger, "Getting value from table");
        EntryResult result = getValue(set->identifier, set->isFromAutomata? AUTOMATA : TRANSITIONS );
        if ( !result.found )
            return _invalidComputation();
        TransitionSet * resultSet;
        if ( set->isFromAutomata ) {
            Automata * automata = result.value.automata;
            resultSet = cpyTransitionSet( automata->transitions->transitionSet);
        } else     
            resultSet = cpyTransitionSet( result.value.transitionSet);
        set->first = resultSet->first;
        set->tail = resultSet->tail; 
    }
    else {
        //tengo que procesar todos los nodos para desglosar las transitions
        TransitionNode * currentNode = set->first;
        TransitionNode * previousNode = set->first;
        while (currentNode != NULL){
            if (currentNode->type == EXPRESSION){
                logInformation(_logger, "Entering currentNode as expression");
                ComputationResult result = computeTransitionExpression(currentNode->transitionExpression, true);
                if (result.succeed){
                    if (result.isSingleElement){
                        logInformation(_logger, "Assigning single transition to node");
                        currentNode->transition = result.transition;
                        currentNode->type = ELEMENT;
                    }
                    else{
                        logInformation(_logger, "Transforming set to to single transitions");
                        //cuando se tiene que una transicion representa varias al mismo tiempo
                        //en vez de almacenarlo como un subset tomo los nodos y los conecto con el set al que forman parte como elementos sueltos
                        TransitionNode * originalNext = currentNode->next;
                        currentNode->transition = result.transitionSet->first->transition; 
                        currentNode->next = result.transitionSet->first->next;
                        result.transitionSet->tail->next = originalNext;
                        currentNode = result.transitionSet->tail;
                    }
                }
                else{
                    logError(_logger, "There has been a problem processing transition expression");
                    return _invalidComputation();
                }
            }
            previousNode = currentNode; 
            currentNode = currentNode->next;
        } 
        
        set->tail = previousNode; 
    }
    logInformation(_logger, "Transition set created");
    deleteRepetitionsFromTransitionSet(set);
    logInformation(_logger, "Deleted repetitions from transition set");
    
    result.transitionSet = set;
    logInformation(_logger,"-----completed transition set creation -----");
    return result;
}


/* Elimina los estados que no son del tipo indicado en type */
static void _filterStates( StateSet * set, StateType type){
    StateNode * currentNode = set->first;
    State * currentState;
    StateNode * resultTail;
    while ( currentNode != NULL ){
    currentState =  currentNode->stateExpression->state;
    switch ( type ){
        case FINAL: 
            if ( !currentState->isFinal ) goto next;
            break;
        case INITIAL: 
            if ( !currentState->isInitial ) goto next;
            break;
        default:
            if ( currentState->isFinal || currentState->isInitial ) goto next;
    }
    if ( resultTail == NULL ) 
        set->first = currentNode;
    else 
        resultTail->next = currentNode;
    resultTail = currentNode; 
next:
    currentNode = currentNode->next; 
    }
    if ( resultTail != NULL )
        resultTail->next = NULL;
    set->tail = resultTail;
}


ComputationResult computeStateSet(StateSet* set, boolean isDefinition) {
    ComputationResult result = {
        .succeed = true,
        .isDefinitionSet = false,
        .type = STATE_DEFINITION
    };
    logInformation(_logger,"starting state set creation");
    if (set->identifier != NULL && !isDefinition) {
        EntryResult result = getValue(set->identifier, set->isFromAutomata? AUTOMATA : STATES );
        if ( !result.found ) { 
            logError(_logger,"%s", CONST_NOT_DEFINED);
            return _invalidComputation();
        }
        StateSet * resultSet;
        if (set->isFromAutomata) {
            Automata * automata = result.value.automata; 
            switch(set->stateType){
                case FINAL: 
                    resultSet = cpyStateSet(automata->finals->stateSet); break;
                case INITIAL: 
                    StateNode * node = malloc(sizeof(StateNode));
                    node->stateExpression->state = automata->initials->state;
                    resultSet = malloc(sizeof(StateSet));
                    resultSet->first = node;
                    resultSet->tail = node; 
                    break;
                default: 
                    resultSet = cpyStateSet( automata->states->stateSet); break;            
            }
        } else 
            resultSet = cpyStateSet( result.value.stateSet);
        set->first = resultSet->first;
        set->tail = resultSet->tail;
    }
    if ( (set->stateType != MIXED && !set->isFromAutomata) || set->stateType==REGULAR ) {
            _filterStates(set, set->stateType);
    } else {
        StateNode * currentNode = set->first;
        StateNode * previousNode = set->first;
        while (currentNode != NULL){
            if (currentNode->type == EXPRESSION){
                logInformation(_logger, "Ready to compute state expression");
                ComputationResult result = computeStateExpression(currentNode->stateExpression, true);
                if (result.succeed){
                    if (result.isSingleElement){
                        currentNode->state= result.state;
                        currentNode->type = ELEMENT;
                        result.isSingleElement = true;
                    }
                    else{
                        StateNode * originalNext = currentNode->next;
                        currentNode->state = result.stateSet->first->state; 
                        result.stateSet->tail->next = originalNext;
                        currentNode = result.stateSet->tail;
                    }
                }else{
                    logError(_logger,"Couldnt create state set");
                    return _invalidComputation();
                }
            }
            previousNode = currentNode;
            currentNode = currentNode->next;
        }
        set->tail = previousNode;
    }
    logDebugging(_logger,"GOING TO DELETE repetitions from state set");
    deleteRepetitionsFromStateSet(set);
    logDebugging(_logger,"Deleted repetitions from state set");
    result.stateSet = set;
    return result;
}

ComputationResult computeSymbolSet(SymbolSet* set, boolean isDefinition) {
    ComputationResult result = {
        .succeed = true,
        .isDefinitionSet = false,
        .type = ALPHABET_DEFINITION        
    };
    if ( set->first==NULL)
        logInformation(_logger,"SET IDENTIF: %s ", set->identifier == NULL? set->identifier:"null" );
    if ( set->identifier != NULL && !isDefinition ) {
        EntryResult result = getValue(set->identifier, set->isFromAutomata? AUTOMATA : ALPHABET );
        if ( !result.found)
            return _invalidComputation();
        SymbolSet * resultSet;
        if ( set->isFromAutomata ) {
            Automata * automata =result.value.automata;
            resultSet = cpySymbolSet( automata->alphabet->symbolSet);
        } else     
            resultSet = cpySymbolSet( result.value.symbolSet);
        set->first = resultSet->first;
        set->tail = resultSet->tail; 
    } else {
        SymbolNode * currentNode = set->first;
        SymbolNode * previousNode = set->first;
        while (currentNode != NULL){
            if (currentNode->type == EXPRESSION){
                ComputationResult result = computeSymbolExpression(currentNode->symbolExpression, true);
                if (result.succeed){
                    if (result.isSingleElement){
                        currentNode->symbol= result.symbol;
                        currentNode->type = ELEMENT;
                        result.isSingleElement = true;
                    }
                    else{
                        SymbolNode * originalNext = currentNode->next;
                        currentNode->symbol = result.symbolSet->first->symbol; 
                        result.symbolSet->tail->next = originalNext;
                        currentNode = result.symbolSet->tail;
                    }
                }
                else{
                    return _invalidComputation();
                }
            }
            previousNode = currentNode;
            currentNode = currentNode->next;
        }
        set->tail = previousNode;
    }
        logDebugging(_logger,"GOING TO DELETE repetitions from symbols set");
    deleteRepetitionsFromSymbolSet(set);
        logDebugging(_logger,"Deleted repetitions from symbols set");
    result.symbolSet = set;
    return result;
}

ComputationResult computeTransition(Transition* transition, boolean isSingleElement){
    logInformation(_logger, "--------Computing transition object------");
    StateExpression * stateExpression = transition->fromExpression;
    ComputationResult result1 = computeStateExpression(stateExpression, false);
    //stateExpression->stateSet = result1.stateSet;
//    stateExpression->type = result1.isSingleElement? ELEMENT_EXPRESSION : SET_EXPRESSION;
    logInformation(_logger, "From state: %s", result1.stateSet->first->state->symbol.value);
    logInformation(_logger, "From state set for transition created");
    
    SymbolExpression * symbolExpression = transition->symbolExpression;
    ComputationResult result2 = computeSymbolExpression(symbolExpression, false);
   // symbolExpression->symbolSet = result2.symbolSet;
    //stateExpression->type = result2.isSingleElement? ELEMENT_EXPRESSION : SET_EXPRESSION;
    logInformation(_logger, "elements consumed created for transition created");

    stateExpression = transition->toExpression;
    ComputationResult result3 = computeStateExpression(stateExpression,false);
    //stateExpression->stateSet = result3.stateSet;
    //stateExpression->type = result3.isSingleElement? ELEMENT_EXPRESSION : SET_EXPRESSION;
    logInformation(_logger, "To state set for transition created");
    
    ComputationResult computationResult = {
		.succeed = true
	};

    boolean isSimpleTransition = result1.stateSet->first == result1.stateSet->tail && result2.symbolSet->first == result2.symbolSet->tail && result3.stateSet->first == result3.stateSet->tail;
    if ( isSingleElement && isSimpleTransition){
        logInformation(_logger, "Simple transition detected ready to be returned");
        transition->fromExpression->state = result1.stateSet->first->state;
        transition->symbolExpression->symbol = result2.symbolSet->first->symbol;
        transition->toExpression->state = result3.stateSet->first->state;
        computationResult.transition = transition;
        logInformation(_logger, "Transition: %s -> %s -> %s", transition->fromExpression->state->symbol.value, transition->symbolExpression->symbol->value, transition->toExpression->state->symbol.value);
        computationResult.isSingleElement = true;
    } else {
        transition->fromExpression->stateSet = result1.stateSet;
        transition->symbolExpression->symbolSet = result2.symbolSet;
        transition->toExpression->stateSet = result3.stateSet;
        TransitionSet * set = malloc(sizeof(TransitionSet));        
        //necesito desglosar la transicion -> combinatoria
        //por cada elemento en el conjunto de estados de from armo una transicion
        //mi pivot es un nodo de cada conjunto
//        logInformation(_logger, "state initial: %s",transition->fromExpression->stateSet->first->state->symbol.value);
        StateNode * pivotFromNode = transition->fromExpression->stateSet->first;
        SymbolNode * pivotSymbolNode = transition->symbolExpression->symbolSet->first;
//        logInformation(_logger, "symbol initial: %s",transition->symbolExpression->symbolSet->first->symbol->value);
        StateNode * pivotToNode = transition->toExpression->stateSet->first;
//        logInformation(_logger, "state initial: %s",transition->toExpression->stateSet->first->state->symbol.value);
        TransitionNode * firstNode = calloc(1, sizeof(TransitionNode));
        set->first = firstNode;

        while(pivotFromNode != NULL){
            while(pivotSymbolNode != NULL){
                while(pivotToNode != NULL){
                    Transition * newTransition = malloc(sizeof(Transition));
                    StateExpression * newFromExpression = calloc(1, sizeof(StateExpression));
                    SymbolExpression * newSymbolExpression = calloc(1, sizeof(SymbolExpression));
                    StateExpression* newToExpression = calloc(1, sizeof(StateExpression));
                    newFromExpression->state = pivotFromNode->state;
                    newTransition->fromExpression = newFromExpression;
 //                   logInformation(_logger, "from state: %s",pivotFromNode->state->symbol.value);

                    newSymbolExpression->symbol = pivotSymbolNode->symbol;
                    newTransition->symbolExpression = newSymbolExpression;

                    newToExpression->state = pivotToNode->state;
                    newTransition->toExpression = newToExpression;

                    if (firstNode->transition != NULL){
                        TransitionNode * newNode = malloc(sizeof(TransitionNode));
                        newNode->transition = newTransition;
                        newNode->type = ELEMENT;
                        if (set->tail != NULL){
                            set->tail->next = newNode;
                        }
                        set->tail = newNode;
                    }else{
                        firstNode->transition = newTransition;
                        firstNode->type = ELEMENT;
                        set->tail = firstNode;
                    }
                    logInformation(_logger, "transition added: %s -%s-> %s", newTransition->fromExpression->state->symbol.value, newTransition->symbolExpression->symbol->value, newTransition->toExpression->state->symbol.value);
                    pivotToNode = pivotToNode->next;
                }
                logInformation(_logger, "All to states were used");
                pivotSymbolNode = pivotSymbolNode->next;
                pivotToNode = transition->toExpression->stateSet->first;
            }
            logInformation(_logger, "All symbols were consumed");
            pivotFromNode = pivotFromNode->next;
            pivotSymbolNode = transition->symbolExpression->symbolSet->first;
        }
        computationResult.transitionSet = set;
        computationResult.isSingleElement = set->first == set->tail;
        logInformation(_logger, "all from states were used, preparing to return set");
    }
    
    return computationResult;
}

ComputationResult computeSymbol(Symbol* symbol, boolean isSingleElement) {

    ComputationResult computationResult = {
		.succeed = true
	};

    if ( isSingleElement ){
        computationResult.symbol = symbol;
        computationResult.isSingleElement = true;
    } else {
        SymbolSet * set = malloc(sizeof(SymbolSet));
        SymbolNode * node = calloc(1,sizeof(SymbolNode));
        node->symbol = symbol;
        node->type = ELEMENT;
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
        logInformation(_logger, "Single state detected");
        computationResult.state = state;
        computationResult.isSingleElement = true;
    } else {
        StateSet * set = malloc(sizeof(StateSet));
        StateNode * node = calloc(1,sizeof(StateNode));
        node->state = state;
        node->type = ELEMENT;
        set->first = node;
        set->tail = node;  
        computationResult.stateSet = set;
        computationResult.type = SET_EXPRESSION;
        logInformation(_logger, "state as set created");
    }
    
    return computationResult;
}



/*-----------------------------SET OPERATIONS --------------------------------------------*/

/*----------------------------- UNION -----------------------------------------------*/
static ComputationResult _transitionUnion(TransitionExpression * leftExp, TransitionExpression * rightExp){
    ComputationResult left = computeTransitionExpression(leftExp, false);
    ComputationResult right = computeTransitionExpression(rightExp, false);
    if (left.succeed && right.succeed){
        TransitionSet * result = calloc(1, sizeof(TransitionSet));
        TransitionSet * leftSet = left.transitionSet;
        TransitionSet * rightSet = right.transitionSet;
        logInformation(_logger, "sets asigned correctly before starting union");
        result->first = leftSet->first;
        logInformation(_logger, "%s -%s-> %s", leftSet->first->transition->fromExpression->state->symbol.value, leftSet->first->transition->symbolExpression->symbol->value, leftSet->first->transition->toExpression->state->symbol.value);
        logInformation(_logger, "%s -%s-> %s", rightSet->first->transition->fromExpression->state->symbol.value, rightSet->first->transition->symbolExpression->symbol->value, rightSet->first->transition->toExpression->state->symbol.value);        
        leftSet->tail->next = rightSet->first; 
        result->tail = rightSet->tail;
        ComputationResult ret = {
            .succeed = true,
            .transitionSet = result
        };
        return ret;
    }
    return _invalidComputation();
}

static ComputationResult _stateUnion(StateExpression * leftExp, StateExpression * rightExp){
    ComputationResult left = computeStateExpression(leftExp, false);
    ComputationResult right = computeStateExpression(rightExp, false);
    if (left.succeed && right.succeed){
        return _stateSetUnion(left.stateSet, right.stateSet);
    }
    return _invalidComputation();
}

static ComputationResult _stateSetUnion(StateSet * leftSet, StateSet * rightSet){
    StateSet * result = calloc(1, sizeof(StateSet));
    result->first = leftSet->first;
    leftSet->tail->next = rightSet->first; 
    result->tail = rightSet->tail;
    return computeStateSet(result,false);
}

static ComputationResult _symbolUnion(SymbolExpression * leftExp, SymbolExpression * rightExp){
    ComputationResult left = computeSymbolExpression(leftExp, false);
    ComputationResult right = computeSymbolExpression(rightExp, false);
    if (left.succeed && right.succeed){
        return _symbolSetUnion( left.symbolSet, right.symbolSet);
    }
    return _invalidComputation();
}

static ComputationResult _symbolSetUnion(SymbolSet * leftSet, SymbolSet * rightSet){
    logInformation(_logger, "entering SymbolSetUnion");
    SymbolSet * result = calloc(1, sizeof(SymbolSet));
    result->first = leftSet->first;
    leftSet->tail->next = rightSet->first; 
    result->tail = rightSet->tail;
    return computeSymbolSet(result,false);
}

/*------------------------------ INTERSECTION -------------------------------------*/
static ComputationResult _transitionIntersection(TransitionExpression * leftExp, TransitionExpression * rightExp){
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
                resultCurrentNode = calloc(1, sizeof(TransitionNode));
                resultCurrentNode->transition = leftCurrentNode->transition;
                resultCurrentNode = resultCurrentNode->next;
                break;
            }
            rightCurrentNode = rightCurrentNode->next;
        }
        if (!found){
            if(transitionEquals(pivotNode->transition, rightCurrentNode->transition)){
                found = 1;
                resultCurrentNode = calloc(1, sizeof(TransitionNode));
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
                resultCurrentNode = calloc(1, sizeof(TransitionNode));
                resultCurrentNode->transition = leftCurrentNode->transition;
                resultCurrentNode = resultCurrentNode->next;
                break;
            }
            rightCurrentNode = rightCurrentNode->next;
        }
        if (!found){
            if(transitionEquals(pivotNode->transition, rightCurrentNode->transition)){
                found = 1;
                resultCurrentNode = calloc(1, sizeof(TransitionNode));
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
    freeTransitionSet(leftSet);
    freeTransitionSet(rightSet);
}

static ComputationResult _stateIntersection(StateExpression * leftExp, StateExpression * rightExp){
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
                resultCurrentNode = calloc(1, sizeof(StateNode));
                resultCurrentNode->state = leftCurrentNode->state;
                resultCurrentNode = resultCurrentNode->next;
                break;
            }
            rightCurrentNode = rightCurrentNode->next;
        }
        if (!found){
            if(stateEquals(pivotNode->state, rightCurrentNode->state)){
                found = 1;
                resultCurrentNode = calloc(1, sizeof(StateNode));
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
                resultCurrentNode = calloc(1, sizeof(StateNode));
                resultCurrentNode->state = leftCurrentNode->state;
                resultCurrentNode = resultCurrentNode->next;
                break;
            }
            rightCurrentNode = rightCurrentNode->next;
        }
        if (!found){
            if(stateEquals(pivotNode->state, rightCurrentNode->state)){
                found = 1;
                resultCurrentNode = calloc(1, sizeof(StateNode));
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
    freeStateSet(leftSet);
    freeStateSet(rightSet);
}

static ComputationResult _symbolIntersection(SymbolExpression * leftExp, SymbolExpression * rightExp){
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
                resultCurrentNode = calloc(1, sizeof(SymbolNode));
                resultCurrentNode->symbol = leftCurrentNode->symbol;
                resultCurrentNode = resultCurrentNode->next;
                break;
            }
            rightCurrentNode = rightCurrentNode->next;
        }
        if (!found){
            if(symbolEquals(pivotNode->symbol, rightCurrentNode->symbol)){
                found = 1;
                resultCurrentNode = calloc(1, sizeof(SymbolNode));
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
                resultCurrentNode = calloc(1, sizeof(SymbolNode));
                resultCurrentNode->symbol = leftCurrentNode->symbol;
                resultCurrentNode = resultCurrentNode->next;
                break;
            }
            rightCurrentNode = rightCurrentNode->next;
        }
        if (!found){
            if(symbolEquals(pivotNode->symbol, rightCurrentNode->symbol)){
                found = 1;
                resultCurrentNode = calloc(1, sizeof(SymbolNode));
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
    freeSymbolSet(leftSet);
    freeSymbolSet(rightSet);
}

/*-------------------------------------- DIFFERENCE -----------------------------*/
static ComputationResult _transitionDifference(TransitionExpression * leftExp, TransitionExpression * rightExp){
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
    result->first = resultCurrentNode;      //todo: se esta guardando? xq parece q queda apuntando a NULL
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
            resultCurrentNode = calloc(1, sizeof(TransitionNode));
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
                resultCurrentNode = calloc(1, sizeof(TransitionNode));
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
    freeTransitionSet(leftSet);
    freeTransitionSet(rightSet);
}

static ComputationResult _stateDifference(StateExpression * leftExp, StateExpression * rightExp){
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
            resultCurrentNode = calloc(1, sizeof(StateNode));
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
                resultCurrentNode = calloc(1, sizeof(StateNode));
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
    freeStateSet(leftSet);
    freeStateSet(rightSet);
}

static ComputationResult _symbolDifference(SymbolExpression * leftExp, SymbolExpression * rightExp){
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
            resultCurrentNode = calloc(1, sizeof(SymbolNode));
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
                resultCurrentNode = calloc(1, sizeof(SymbolNode));
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
    freeSymbolSet(leftSet);
    freeSymbolSet(rightSet);
}

/*---------------------------------------------- DELETE REPETITIONS FROM SET -----------------------------------------*/
static void deleteRepetitionsFromTransitionSet(TransitionSet * set){
    TransitionNode * current = set->first;
    TransitionNode * next;
    while (current != NULL){
        if ( current == set->tail)
            return;
        next = current->next;
        while (next != NULL){
            if (transitionEquals(current->transition, next->transition)){
                current->next = next->next;
                free(next);
                next = current->next;
            } else {
                next = next->next;
            }
        }
        current = current->next;
    }
}

static void deleteRepetitionsFromStateSet(StateSet * set){
    StateNode * current = set->first;
    StateNode * next;
    while (current != NULL){
        next = current->next;
        while (next != NULL){
            if (stateEquals(current->state, next->state)){
                current->next = next->next;
                free(next);
                next = current->next;
            } else {
                next = next->next;
            }
        }
        current = current->next;
    }
}

static void deleteRepetitionsFromSymbolSet(SymbolSet * set){
    SymbolNode * current = set->first;
    SymbolNode * next;
    while (current != NULL){
        next = current->next;
        while (next != NULL){
            if (symbolEquals(current->symbol, next->symbol)){
                current->next = next->next;
                free(next);
                next = current->next;
            } else {
                next = next->next;
            }
        }
        current = current->next;
    }
}

/*--------------------------------------------- INVALID OPERATORS -----------------------------------------*/
/**
 * A computation that always returns an invalid result.
 */
static ComputationResult _invalidComputation() {
	ComputationResult computationResult = {
		.succeed = false,
	};
	return computationResult;
}

