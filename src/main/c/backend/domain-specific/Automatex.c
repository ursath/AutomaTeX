#include "Automatex.h"


static Logger * _logger = NULL;

/*-----------------PRIVATE FUNCTIONS ---------------------------------------------*/

static ComputationResult _computeFinalAndInitialStates(StateSet * set, StateExpression * finals, StateExpression * initial);
static ComputationResult _checkTransitions(TransitionSet * transitions, StateSet * states, SymbolSet * alphabet);
static void _filterStates( StateSet * set, StateType type);
static void _getTransitionStatesAndSymbols(TransitionSet * transitions, StateSet * states, SymbolSet * alphabet);

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

/*-------------------------------------- FREE NODE SETS --------------------*/
static void freeTransitionSet(TransitionSet * set);
static void freeStateSet(StateSet * set);
static void freeSymbolSet(SymbolSet * set);

/*----------------------------------------- SET EQUALS ----------------------------------------------------*/ 
static boolean transitionSetEquals(TransitionSet * set1, TransitionSet * set2);
static boolean stateSetEquals(StateSet * set1, StateSet * set2);
static boolean symbolSetEquals(SymbolSet *set1, SymbolSet *set2);
static boolean transitionEquals(Transition * trans1, Transition * trans2);
static boolean stateEquals(State * state1, State * state2);
static boolean symbolEquals(Symbol * symbol1, Symbol * symbol2);



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
    
    result = _computeFinalAndInitialStates(stateSetResult.stateSet, automata->finals, automata->initials);
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
    //TODO: validar si DFA, NFA o LNFA 
    result = _checkTransitions(transitionSetResult.transitionSet, stateSetResult.stateSet, symbolSetResult.symbolSet);
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
static ComputationResult _computeFinalAndInitialStates(StateSet * set, StateExpression * finals, StateExpression * initial){
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

    initial = malloc(sizeof(StateExpression));
    initial->state = initialState;
    finals = malloc(sizeof(StateExpression));
    finals->stateSet = finalSet; 
    ComputationResult result = { .succeed = true };


    return result; 
}

/* chequea si los estados y simbolos de las transiciones pertenecen a los del automata */
static ComputationResult _checkTransitions(TransitionSet * transitions, StateSet * states, SymbolSet * alphabet) {

    StateSet * transitionStateSet;
    SymbolSet * transitionSymbolSet;
    _getTransitionStatesAndSymbols(transitions, transitionStateSet, transitionSymbolSet);
    logInformation(_logger,"-----getTransitionStatesAndSymbols -----");
    ComputationResult result;
    result.succeed = stateSetEquals(transitionStateSet, states) && symbolSetEquals(transitionSymbolSet, alphabet);
    logInformation(_logger,"----- state set equals-----");
    freeStateSet(transitionStateSet);
    freeSymbolSet(transitionSymbolSet);
    if ( !result.succeed ){
        logError(_logger,"%s its transitions use symbols and states that don't belong to the automata", AUTOMATA_NOT_CREATED);
    }
    return result;
    
}

static void _getTransitionStatesAndSymbols(TransitionSet * transitions, StateSet * states, SymbolSet * alphabet)  {
    TransitionNode * currentNode = transitions->first;
    Transition * transition;
    StateSet * tStateSet;
    SymbolSet * tSymbolSet;
    ComputationResult result;
    while (currentNode != NULL) {
        transition = currentNode->transition;
        // solo tengo sets en una transicion 
        tStateSet = cpyStateSet( transition->toExpression->stateSet ) ;
        if ( states == NULL )
            states = tStateSet;
        else {
            result = _stateSetUnion(states, tStateSet);
            states = result.stateSet;
        }
        free(tStateSet);

        tStateSet = cpyStateSet( transition->fromExpression->stateSet );
        result = _stateSetUnion(states, tStateSet);
        states = result.stateSet;
        free(tStateSet);
        
        tSymbolSet = cpySymbolSet( transition->symbolExpression->symbolSet );
        if ( alphabet==NULL )
            alphabet = tSymbolSet;
        else {
            result = _symbolSetUnion(alphabet, tSymbolSet);
            alphabet = result.symbolSet;
        }
        free(tSymbolSet);
        
        currentNode = currentNode->next;
    }
    
}

ComputationResult computeTransitionExpression(TransitionExpression * expression, boolean isSingleElement ){
    ComputationResult result;
    switch ( expression->type) {
        case UNION_EXPRESSION:
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
                ComputationResult resultElement = computeTransition(expression->transition, isSingleElement);
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
                if (resultSet.stateSet->first == resultSet.stateSet->tail){
                    expression->state = resultSet.stateSet->first->state;
                    logInformation(_logger, "There is only one state: %s", expression->state->symbol.value);
                }
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
                ComputationResult resultSet = computeSymbolSet(expression->symbolSet,false);
                if (resultSet.symbolSet->first == resultSet.symbolSet->tail){
                    expression->symbol = resultSet.symbolSet->first->symbol;
                    logInformation(_logger, "There is only one symbol: %s", expression->symbol->value);
                }
                return resultSet;
                break;
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
                    set->tail->transition = result2.transition;
                    set->tail->type = ELEMENT;
                }
                else{
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
            
    else if ( set->identifier != NULL && !isDefinition ) {
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
        while (currentNode != set->tail){
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
                        result.transitionSet->tail->next = originalNext;
                        currentNode = result.transitionSet->tail;
                    }
                }
                else{
                    logError(_logger, "There has been a problem processing transition expression");
                    return _invalidComputation();
                }
            }
           
            currentNode = currentNode->next;
        } 
        logInformation(_logger, "Entering LastNode as expression");
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
                result.transitionSet->tail->next = originalNext;
                currentNode = result.transitionSet->tail;
            }
        }
        else{
            logError(_logger, "There has been a problem processing transition expression");
            return _invalidComputation();
        }
        set->tail = currentNode; 
    }
    logInformation(_logger, "Transition set created");
//    deleteRepetitionsFromTransitionSet(set);
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
    logDebugging(_logger,"starting state set creation");
    if (set->identifier != NULL && !isDefinition) {
        EntryResult result = getValue(set->identifier, set->isFromAutomata? AUTOMATA : STATES );
        if ( !result.found)
            return _invalidComputation();
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
        logInformation(_logger, "last element is state: %s", set->tail->stateExpression->state->symbol.value);
        while (currentNode != set->tail){
                ComputationResult result = computeStateExpression(currentNode->stateExpression, true);
                if (result.succeed){
                    if (result.isSingleElement){
                        logInformation(_logger, "is single element, state: %s", result.state);
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
            currentNode = currentNode->next;
        }
        //ultimo nodo
        logInformation(_logger, "computing lastNode from stateSet");
        ComputationResult result = computeStateExpression(currentNode->stateExpression, true);
        if (result.succeed){
            if (result.isSingleElement){
                logInformation(_logger, "is single element and the state is: %s", result.state->symbol.value);
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
        set->tail = currentNode;
    }
//    deleteRepetitionsFromStateSet(set);
    logDebugging(_logger,"created state set");
    result.stateSet = set;
    return result;
}

ComputationResult computeSymbolSet(SymbolSet* set, boolean isDefinition) {
    ComputationResult result = {
        .succeed = true,
        .isDefinitionSet = false,
        .type = ALPHABET_DEFINITION        
    };
    
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
        while (currentNode != set->tail){
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
            currentNode = currentNode->next;
        }
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
        set->tail = currentNode;
    }
//    deleteRepetitionsFromSymbolSet(set);
    result.symbolSet = set;
    return result;
}

ComputationResult computeTransition(Transition* transition, boolean isSingleElement){
    logInformation(_logger, "--------Computing transition object------");
    StateExpression * stateExpression = transition->fromExpression;
    ComputationResult result1 = computeStateExpression(stateExpression, false);
    //stateExpression->stateSet = result1.stateSet;
//    stateExpression->type = result1.isSingleElement? ELEMENT_EXPRESSION : SET_EXPRESSION;
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

//    boolean withoutExpressionOperators = transition->fromExpression->type == ELEMENT_EXPRESSION && transition->symbolExpression->type == ELEMENT_EXPRESSION && transition->toExpression->type == ELEMENT_EXPRESSION;
    //acá no uso equals porque es literalmente apuntar a la misma dirección de memoria para indicar que hay un solo elemento
    //boolean isSimpleTransition = transition->fromExpression->stateSet->first == transition->fromExpression->stateSet->tail && transition->symbolExpression->symbolSet->first == transition->symbolExpression->symbolSet->tail && transition->toExpression->stateSet->first == transition->toExpression->stateSet->tail;
    boolean isSimpleTransition = result1.stateSet->first == result1.stateSet->tail && result2.symbolSet->first == result2.symbolSet->tail && result3.stateSet->first == result3.stateSet->tail;
    if ( isSingleElement && isSimpleTransition){
        logInformation(_logger, "Simple transition detected ready to be returned");
        logInformation(_logger, "Transition: %s -> %s -> %s", transition->fromExpression->state->symbol.value, transition->symbolExpression->symbol->value, transition->toExpression->state->symbol.value);
        //transition->fromExpression->state = result1.stateSet->first->state;
        //transition->symbolExpression->symbol = result2.symbolSet->first->symbol;
        //transition->toExpression->state = result3.stateSet->first->state;
        computationResult.transition = transition;
        computationResult.isSingleElement = true;
    } else {
        transition->fromExpression->stateSet = result1.stateSet;
        transition->symbolExpression->symbolSet = result2.symbolSet;
        transition->toExpression->stateSet = result3.stateSet;
        TransitionSet * set = malloc(sizeof(TransitionSet));        
        //necesito desglosar la transicion -> combinatoria
        //por cada elemento en el conjunto de estados de from armo una transicion
        //mi pivot es un nodo de cada conjunto
        StateNode * pivotFromNode = transition->fromExpression->stateSet->first;
        SymbolNode * pivotSymbolNode = transition->symbolExpression->symbolSet->first;
        StateNode * pivotToNode = transition->toExpression->stateSet->first;
        TransitionNode * firstNode = calloc(1, sizeof(TransitionNode));
        set->first = firstNode;

        while(pivotFromNode != transition->fromExpression->stateSet->tail && pivotSymbolNode != transition->symbolExpression->symbolSet->tail && pivotToNode != transition->toExpression->stateSet->tail){
            while(pivotSymbolNode != transition->symbolExpression->symbolSet->tail && pivotToNode != transition->toExpression->stateSet->tail){
                while(pivotToNode != transition->toExpression->stateSet->tail){
                    Transition * newTransition = malloc(sizeof(Transition));
                    newTransition->fromExpression->state = pivotFromNode->state;
                    logInformation(_logger, "from state: %s",pivotFromNode->state);
                    newTransition->symbolExpression->symbol = pivotSymbolNode->symbol;
                    newTransition->toExpression->state = pivotToNode->state;
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
                    pivotToNode = pivotToNode->next;
                }
                logInformation(_logger, "All to states were used");
                pivotSymbolNode = pivotSymbolNode->next;
            }
            logInformation(_logger, "All symbols were consumed");
            pivotFromNode = pivotFromNode->next;
        }
        logInformation(_logger, "Creating Last Node");
        Transition * lastTransition = malloc(sizeof(Transition));
        lastTransition->fromExpression->state = pivotFromNode->state;
        lastTransition->symbolExpression->symbol = pivotSymbolNode->symbol;
        lastTransition->toExpression->state = pivotToNode->state;
        if (firstNode->transition == NULL){
            firstNode->transition = lastTransition;
            firstNode->type = ELEMENT;
            set->tail = firstNode;
        }else {
            TransitionNode * newNode = calloc(1, sizeof(TransitionNode));
            newNode->transition = lastTransition;
            newNode->type = ELEMENT;
            if (set->tail != NULL){
                set->tail->next = newNode;
            }
            set->tail = newNode;
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
        node->type = ELEMENT;
        set->first = node;  
        set->tail = node;  
        computationResult.symbolSet = set;
        computationResult.isSingleElement = true;
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
        node->type = ELEMENT;
        set->first = node;
        set->tail = node;  
        computationResult.stateSet = set;
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
        result->first = leftSet->first;
        leftSet->tail->next = rightSet->first; 
        result->tail = rightSet->tail;
        return computeTransitionSet(result,false);
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
        logInformation(_logger,"STARTING DELETE");
    if (current->transition != NULL) {
        logInformation(_logger,"Is not null");
        logInformation(_logger,"Transition: %s",current->transition);
    }
    while (current != NULL){
        if ( current == set->tail)
            return;
        next = current->next;
        while (next != NULL){
            logInformation(_logger,"Is deleting..");
            if (transitionEquals(current->transition, next->transition)){
                logInformation(_logger ,"is equal");
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
    found = transitionEquals(pivot->transition, current2->transition);
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
    found = stateEquals(pivot->state, current2->state);
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
    found = symbolEquals(pivot->symbol, current2->symbol);
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
            if (symbolEquals(pivot->symbol, current2->symbol)){
                found=1;
            }
        }
        current2 = set2->first;
    }while(current1 != set1->tail && found);
    return found;
}

/*------------------- ELEMENT EQUALS --------------------------------------------*/
static boolean transitionEquals(Transition * trans1, Transition * trans2){
    StateSet * trans1From = trans1->fromExpression->stateSet;
    StateSet * trans2From = trans2->fromExpression->stateSet;
    StateSet * trans1To = trans1->toExpression->stateSet;
    StateSet * trans2To = trans2->toExpression->stateSet;
    logInformation(_logger,"starting equals");    
    return stateSetEquals(trans1From, trans2From) && stateSetEquals(trans1To, trans2To) && symbolSetEquals(trans1->symbolExpression->symbolSet, trans2->symbolExpression->symbolSet);
}

static boolean stateEquals(State * state1, State * state2){
    return  symbolEquals(&state1->symbol, &state2->symbol) && state1->isFinal == state2->isFinal && state1->isInitial == state2->isInitial;
}

static boolean symbolEquals(Symbol * symbol1, Symbol * symbol2){
    return strcmp(symbol1->value, symbol2->value) == 0;
}
