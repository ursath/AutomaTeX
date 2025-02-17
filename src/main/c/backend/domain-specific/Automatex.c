#include "Automatex.h"


static Logger * _logger = NULL;

/*-----------------PRIVATE FUNCTIONS ---------------------------------------------*/

static ComputationResult _computeFinalAndInitialStates(StateSet * set, Automata * automata);
static ComputationResult _checkAutomataRequirements(TransitionSet * transitions, StateSet * states, SymbolSet * alphabet, AutomataType automataType);
static void _filterStates( StateSet * set, StateType type);
static ComputationResult _checkTransitionStatesAndSymbols(TransitionSet * transitions, StateSet * states, SymbolSet * alphabet);
static ComputationResult _containsLambda(const SymbolSet * alphabet, AutomataType type);
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
                    Automata * a = result.automata;
            }else{
                //error redefinition
                return _invalidComputation();
            }
            break;
        case TRANSITION_DEFINITION:
            identifier = definition->transitionSet->identifier;
            if ( !exists(identifier) ) {
                result = computeTransitionSet(definition->transitionSet,true);
                 if (!result.succeed){
                    return result;
                }
                definition->transitionSet = result.transitionSet;
                value.transitionSet = definition->transitionSet;
            }
            break;
        case ALPHABET_DEFINITION:
            identifier = definition->symbolSet->identifier;
            if ( !exists(identifier) ) {
                result = computeSymbolSet(definition->symbolSet,true);
                if (!result.succeed){
                    return result;
                }
                definition->symbolSet = result.symbolSet;
                value.symbolSet = definition->symbolSet;
            }
            break;
        case STATE_DEFINITION:
            identifier = definition->stateSet->identifier;
            if ( !exists(identifier) ) {
                result = computeStateSet(definition->stateSet,true);
                 if (!result.succeed){
                    return result;
                }
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
    logInformation(_logger,"Completed definition");
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
        return stateSetResult;   
    }
    
    result = _computeFinalAndInitialStates(stateSetResult.stateSet, automata);
    if ( !result.succeed ){
        return result;   
    }
    logInformation(_logger,"-----computed final and initial states-----");
    ComputationResult symbolSetResult = computeSymbolExpression(automata->alphabet, true);
    if ( !symbolSetResult.succeed ){
        return symbolSetResult;   
    }
    logInformation(_logger,"-----computed symbols -----");
    ComputationResult transitionSetResult = computeTransitionExpression(automata->transitions, true);
    if ( !transitionSetResult.succeed ){
        return transitionSetResult;   
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
    StateNode * finalTail =NULL;
    State * initialState = NULL; 
    StateSet * finalSet = calloc(1, sizeof(StateSet));
    while ( currentNode != NULL){
        currentState =  currentNode->state;
        if ( currentState->isFinal ) {
            StateNode * node = calloc(1,sizeof(StateNode));
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

    if ( finalSet->first == NULL ) {
        logError(_logger,"%s it does not have final states", AUTOMATA_NOT_CREATED);
        return _invalidComputation();
    }
    if ( initialState == NULL ) {
        logError(_logger,"%s it does not have an initial state", AUTOMATA_NOT_CREATED);
        return _invalidComputation();
    }

    automata->initials = calloc(1,sizeof(StateExpression));
    automata->initials->state = initialState;
    automata->finals = calloc(1,sizeof(StateExpression));
    finalSet->tail = finalTail;
    automata->finals->stateSet = finalSet; 
    ComputationResult result = { .succeed = true };


    return result; 
}

static ComputationResult _checkAutomataRequirements(TransitionSet * transitions, StateSet * states, SymbolSet * alphabet, AutomataType automataType) {
    logInformation(_logger, "checking automata requirements");
    ComputationResult result;
    
    if ( automataType==LNFA_AUTOMATA ) {
        // agrego lambda al alphabet
        Symbol * lambda = calloc(1,sizeof(Symbol));
        lambda->value = LAMBDA_STRING; 
        SymbolNode * node = calloc(1,sizeof(SymbolNode));
        node->symbol = lambda;
        alphabet->tail->next = node;
        alphabet->tail = node;
        if ( alphabet->first == NULL)
            alphabet->first = node;
    }else{    
        result = _containsLambda(alphabet, automataType);
        if ( !result.succeed)
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

static ComputationResult _containsLambda(const SymbolSet * alphabet, AutomataType type)  {
    logInformation(_logger, "looking for lambda..");
    Symbol * lambda = calloc(1,sizeof(Symbol));
    lambda->value = LAMBDA_STRING; 
    ComputationResult result = {
        .succeed = !containsSymbol(alphabet->first,lambda)
    };
    free(lambda);
    
    if ( result.succeed )
        return result;

    logError(_logger, "Lambda symbol was found in a %s automata", type==DFA_AUTOMATA? "DFA":"NFA");
    return _invalidComputation();
}

/* Chequeo si los simbolos y estados usados en las transiciones estan en la definicion del automata */
static ComputationResult _checkTransitionStatesAndSymbols(TransitionSet * transitions, StateSet * states, SymbolSet * alphabet)  {
    logInformation(_logger, "checking transitions");
    TransitionNode * currentTransNode = transitions->first;
    Transition * transition;
    ComputationResult result ={ .succeed=false };
            
    while (currentTransNode != NULL) {
        transition = currentTransNode->transition;
    
        if ( !containsState(states->first,transition->fromExpression->state ) || !containsState(states->first,transition->toExpression->state ) ) {
            logError(_logger,"%s its transitions use states that don't belong to the automata", AUTOMATA_NOT_CREATED);
            return result;
        }
        if ( !containsSymbol(alphabet->first,transition->symbolExpression->symbol ) ) {
            logError(_logger,"%s its transitions use symbol %s that doesn't belong to the automata", AUTOMATA_NOT_CREATED, transition->symbolExpression->symbol->value);
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
                return _transitionUnion(expression->leftExpression, expression->rightExpression);
                        break;
        case INTERSECTION_EXPRESSION:
                return _transitionIntersection(expression->leftExpression, expression->rightExpression);
                        break;
        case DIFFERENCE_EXPRESSION:
                return _transitionDifference(expression->leftExpression, expression->rightExpression);
                        break;
        case SET_EXPRESSION:
                return computeTransitionSet(expression->transitionSet,false);
                        break;
        case ELEMENT_EXPRESSION: 
                ComputationResult resultElement = computeTransition(expression->transition, isSingleElement);
                if (resultElement.succeed){
                    if (resultElement.isSingleElement){
                        expression->transition = resultElement.transition;
                    }else{
                        expression->transitionSet = resultElement.transitionSet;
                    }
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
                    return computeStateSet(expression->stateSet,false);
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
                return _symbolUnion(expression->leftExpression, expression->rightExpression);
                        break;
        case INTERSECTION_EXPRESSION:
                return _symbolIntersection(expression->leftExpression, expression->rightExpression);
                        break;
        case DIFFERENCE_EXPRESSION:
                return _symbolDifference(expression->leftExpression, expression->rightExpression);
                        break;
        case SET_EXPRESSION:
                return computeSymbolSet(expression->symbolSet,false);
        case ELEMENT_EXPRESSION:
                return computeSymbol(expression->symbol, isSingleElement);
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
        ComputationResult result1, result2;
        result1 = computeTransitionExpression(set->first->transitionExpression, true);
        if (result1.succeed){
            if (result1.isSingleElement){
                set->first->transition = result1.transition;
                set->first->type = ELEMENT;
            }
            else{
                TransitionNode * originalNext = set->first->next;
                set->first = result1.transitionSet->first; 
                result1.transitionSet->tail->next = originalNext;
            }
            result2 = computeTransitionExpression(set->tail->transitionExpression, true);
            if (result2.succeed){
                if (result2.isSingleElement){
                    set->tail->transition = result2.transition;
                    set->tail->type = ELEMENT;
                }
                else{
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
        if ( !result.found ){
            logError(_logger,"%s %s",set->identifier, CONST_NOT_DEFINED);
            return _invalidComputation();
        }
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
        TransitionNode * currentNode = set->first;
        TransitionNode * previousNode = set->first;
        while (currentNode != NULL){
            if (currentNode->type == EXPRESSION){
                ComputationResult result = computeTransitionExpression(currentNode->transitionExpression, true);
                if (result.succeed){
                    if (result.isSingleElement){
                        currentNode->transition = result.transition;
                        currentNode->type = ELEMENT;
                    }
                    else{
                        TransitionNode * originalNext = currentNode->next;
                        currentNode->transition = result.transitionSet->first->transition; 
                        result.transitionSet->tail->next = originalNext;
                        currentNode->next = result.transitionSet->first->next;
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
    logInformation(_logger, "Filtering...");
    StateNode * currentNode = set->first;
    State * currentState;
    StateNode * resultTail = NULL;
    while ( currentNode != NULL ){
    currentState =  currentNode->state;
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
            logError(_logger,"%s %s",set->identifier, CONST_NOT_DEFINED);
            return _invalidComputation();
        }
        StateSet * resultSet;
        if (set->isFromAutomata) {
            Automata * automata = result.value.automata; 
            switch(set->stateType){
                case FINAL: 
                    resultSet = cpyStateSet(automata->finals->stateSet); break;
                case INITIAL: 
                    StateNode * node = calloc(1,sizeof(StateNode));
                    node->state = calloc(1,sizeof(State));
                    node->state->symbol = automata->initials->state->symbol;
                    node->state->isFinal = automata->initials->state->isFinal;
                    node->state->isInitial = automata->initials->state->isInitial;
                    resultSet = calloc(1,sizeof(StateSet));
                    node->type = ELEMENT;
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
                ComputationResult result = computeStateExpression(currentNode->stateExpression, true);
                if (result.succeed){
                    if (result.isSingleElement){
                        currentNode->state= result.state;
                        currentNode->type = ELEMENT;
                        result.isSingleElement = true;
                    }
                    else{
                        if (result.stateSet->first != NULL){
                            StateNode * originalNext = currentNode->next;
                            currentNode->state = result.stateSet->first->state; 
                            result.stateSet->tail->next = originalNext;
                            currentNode->next = result.stateSet->first->next;
                            currentNode = result.stateSet->tail;
                        }
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
    
    if ( set->identifier != NULL && !isDefinition ) {
        EntryResult result = getValue(set->identifier, set->isFromAutomata? AUTOMATA : ALPHABET );
        if ( !result.found) {
            logError(_logger,"%s %s",set->identifier, CONST_NOT_DEFINED);
            return _invalidComputation();
        }
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
                        if (result.symbolSet->first != NULL){
                            SymbolNode * originalNext = currentNode->next;
                            currentNode->symbol = result.symbolSet->first->symbol; 
                            result.symbolSet->tail->next = originalNext;
                            currentNode->next = result.symbolSet->first->next;
                            currentNode = result.symbolSet->tail;
                        }
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
    deleteRepetitionsFromSymbolSet(set);
    result.symbolSet = set;
    return result;
}

ComputationResult computeTransition(Transition* transition, boolean isSingleElement){
    ComputationResult computationResult = {
		.succeed = false
	};
    logInformation(_logger, "--------Computing transition object------");
    StateExpression * stateExpression = transition->fromExpression;
    ComputationResult result1 = computeStateExpression(stateExpression, false);
    if (!result1.succeed){
        return computationResult;
    }
    logInformation(_logger, "From state set for transition created");
    
    SymbolExpression * symbolExpression = transition->symbolExpression;
    ComputationResult result2 = computeSymbolExpression(symbolExpression, false);
    if (!result2.succeed){
        return computationResult;
    }
    logInformation(_logger, "elements consumed created for transition created");

    stateExpression = transition->toExpression;
    ComputationResult result3 = computeStateExpression(stateExpression,false);
    if (!result3.succeed){
        return computationResult;
    }
    logInformation(_logger, "To state set for transition created");
    

    boolean isSimpleTransition = result1.stateSet->first == result1.stateSet->tail && result2.symbolSet->first == result2.symbolSet->tail && result3.stateSet->first == result3.stateSet->tail;
    if ( isSingleElement && isSimpleTransition){
        logInformation(_logger, "Simple transition detected ready to be returned");
        transition->fromExpression->state = result1.stateSet->first->state;
        transition->symbolExpression->symbol = result2.symbolSet->first->symbol;
        transition->toExpression->state = result3.stateSet->first->state;
        computationResult.transition = transition;
        computationResult.isSingleElement = true;
    } else {
        transition->fromExpression->stateSet = result1.stateSet;
        transition->symbolExpression->symbolSet = result2.symbolSet;
        transition->toExpression->stateSet = result3.stateSet;

        TransitionSet * set = malloc(sizeof(TransitionSet));        

        StateNode * pivotFromNode = transition->fromExpression->stateSet->first;
        SymbolNode * pivotSymbolNode = transition->symbolExpression->symbolSet->first;
        StateNode * pivotToNode = transition->toExpression->stateSet->first;

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
                    logInformation(_logger, "Transition added: %s -%s-> %s", newTransition->fromExpression->state->symbol.value, newTransition->symbolExpression->symbol->value, newTransition->toExpression->state->symbol.value);
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
        logInformation(_logger, "All from states were used, preparing to return set");
    }
    computationResult.succeed = true;
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
        if (leftSet->first != NULL){
            result->first = leftSet->first;
            leftSet->tail->next = rightSet->first; 
        }else{
            result->first = rightSet->first;
        }
        if (rightSet->first != NULL){
            result->tail = rightSet->tail;
        }else{
            result->tail = leftSet->tail;
        }
        if (result->first != NULL){
            deleteRepetitionsFromTransitionSet(result);
        }
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
    if (leftSet->first != NULL){
        result->first = leftSet->first;
        leftSet->tail->next = rightSet->first; 
    }else{
        result->first = rightSet->first;
    }
    if (rightSet->first != NULL){
        result->tail = rightSet->tail;
    }else{
        result->tail = leftSet->tail;
    }    

    if (result->first != NULL){
        deleteRepetitionsFromStateSet(result);
    }

    ComputationResult ret = {
        .succeed = true,
        .stateSet= result
    };
    return ret;    
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
    if (leftSet->first != NULL){
        result->first = leftSet->first;
        leftSet->tail->next = rightSet->first; 
    }else{
        result->first = rightSet->first;
    }
    if (rightSet->first != NULL){
        result->tail = rightSet->tail;
    }else{
        result->tail = leftSet->tail;
    }

    if (result->first != NULL){
        deleteRepetitionsFromSymbolSet(result);
    }
    ComputationResult ret = {
        .succeed = true,
        .symbolSet= result
    };
    return ret;    
}

/*------------------------------ INTERSECTION -------------------------------------*/
static ComputationResult _transitionIntersection(TransitionExpression * leftExp, TransitionExpression * rightExp){
    ComputationResult left = computeTransitionExpression(leftExp, false);
    ComputationResult right = computeTransitionExpression(rightExp, false);
    if (left.succeed && right.succeed){
        TransitionSet * result = calloc(1, sizeof(TransitionSet));
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
    TransitionNode * resultCurrentNode = NULL; 
    result->first = NULL;
    int found = 0;
    if (leftSet->tail == NULL || rightSet->tail == NULL){
        return;
    }
    while (leftCurrentNode != NULL && !found){
        //dejo un nodo para comparar contra todos los nodos del otro set
        while (rightCurrentNode != NULL && !found){
            if (transitionEquals(leftCurrentNode->transition, rightCurrentNode->transition)){
                found = 1;
                resultCurrentNode = calloc(1, sizeof(TransitionNode));
                resultCurrentNode->transition= leftCurrentNode->transition;
                if (result->tail == NULL){
                    result->first = resultCurrentNode;
                }
                if (result->tail != NULL){
                    result->tail->next = resultCurrentNode;
                }
                result->tail = resultCurrentNode;
                break;
            }
            rightCurrentNode = rightCurrentNode->next;
        }
        leftCurrentNode = leftCurrentNode->next;
        rightCurrentNode = rightSet->first;
        found = 0;
    }    
    freeTransitionSet(leftSet);
    freeTransitionSet(rightSet);
}

static ComputationResult _stateIntersection(StateExpression * leftExp, StateExpression * rightExp){
    ComputationResult left = computeStateExpression(leftExp, false);
    ComputationResult right = computeStateExpression(rightExp, false);
    if (left.succeed && right.succeed){
        StateSet * result = calloc(1, sizeof(StateSet));
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
    StateNode * resultCurrentNode = NULL;
    result->first = NULL;
    int found = 0;
    if (leftSet->tail == NULL || rightSet->tail == NULL){
        return;
    }
    while (leftCurrentNode != NULL && !found){
        //dejo un nodo para comparar contra todos los nodos del otro set
        while (rightCurrentNode != NULL && !found){
            if (stateEquals(leftCurrentNode->state, rightCurrentNode->state)){
                found = 1;
                resultCurrentNode = calloc(1, sizeof(StateNode));
                resultCurrentNode->state = leftCurrentNode->state;
                if (result->tail == NULL){
                    result->first = resultCurrentNode;
                }
                if (result->tail != NULL){
                    result->tail->next = resultCurrentNode;
                }
                result->tail = resultCurrentNode;
                break;
            }
            rightCurrentNode = rightCurrentNode->next;
        }
        leftCurrentNode = leftCurrentNode->next;
        rightCurrentNode = rightSet->first;
        found = 0;
    }        
    freeStateSet(leftSet);
    freeStateSet(rightSet);
}

static ComputationResult _symbolIntersection(SymbolExpression * leftExp, SymbolExpression * rightExp){
    ComputationResult left = computeSymbolExpression(leftExp, false);
    ComputationResult right = computeSymbolExpression(rightExp, false);
    if (left.succeed && right.succeed){
        SymbolSet * result = calloc(1, sizeof(SymbolSet));
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
    SymbolNode * resultCurrentNode = NULL; 
    result->first = NULL;
    int found = 0;
    if (leftSet->tail == NULL || rightSet->tail == NULL){
        return;
    }
    while (leftCurrentNode != NULL && !found){
        //dejo un nodo para comparar contra todos los nodos del otro set
        while (rightCurrentNode != NULL && !found){
            if (symbolEquals(leftCurrentNode->symbol, rightCurrentNode->symbol)){
                found = 1;
                resultCurrentNode = calloc(1, sizeof(SymbolNode));
                resultCurrentNode->symbol= leftCurrentNode->symbol;
                if (result->tail == NULL){
                    result->first = resultCurrentNode;
                }
                if (result->tail != NULL){
                    result->tail->next = resultCurrentNode;
                }
                result->tail = resultCurrentNode;
                break;
            }
            rightCurrentNode = rightCurrentNode->next;
        }
        leftCurrentNode = leftCurrentNode->next;
        rightCurrentNode = rightSet->first;
        found = 0;
    }    
    freeSymbolSet(leftSet);
    freeSymbolSet(rightSet);
}

/*-------------------------------------- DIFFERENCE -----------------------------*/
static ComputationResult _transitionDifference(TransitionExpression * leftExp, TransitionExpression * rightExp){
    ComputationResult left = computeTransitionExpression(leftExp, false);
    ComputationResult right = computeTransitionExpression(rightExp, false);
    if (left.succeed && right.succeed){
        TransitionSet * result = calloc(1, sizeof(TransitionSet));
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
    TransitionNode * resultCurrentNode = NULL; 
    result->first = NULL;      
    int found = 0;
    if (leftSet->tail == NULL){
        return;
    }
    if (rightSet->tail == NULL){
        result = leftSet;
        return;
    }
    while (leftCurrentNode != NULL && !found){
        //dejo un nodo para comparar contra todos los nodos del otro set
        while (rightCurrentNode != NULL && !found){
            if (transitionEquals(leftCurrentNode->transition, rightCurrentNode->transition)){
                found = 1;
                break;
            }
            rightCurrentNode = rightCurrentNode->next;
        }
        if (!found){
            resultCurrentNode = calloc(1, sizeof(TransitionNode));
            resultCurrentNode->transition= leftCurrentNode->transition;
            if (result->tail == NULL){
                result->first = resultCurrentNode;
            }
            if (result->tail != NULL){
                result->tail->next = resultCurrentNode;
            }
            result->tail = resultCurrentNode;
        }
        leftCurrentNode = leftCurrentNode->next;
        rightCurrentNode = rightSet->first;
        found = 0;
    }

    freeTransitionSet(leftSet);
    freeTransitionSet(rightSet);
}

static ComputationResult _stateDifference(StateExpression * leftExp, StateExpression * rightExp){
    ComputationResult left = computeStateExpression(leftExp, false);
    ComputationResult right = computeStateExpression(rightExp, false);
    if (left.succeed && right.succeed){
        StateSet * result = calloc(1, sizeof(StateSet));
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
    StateNode * resultCurrentNode = NULL; 
    result->first = resultCurrentNode;
    int found = 0;
    if (leftSet->first== NULL){
        return;
    }
    if (rightSet->first== NULL){
        result = leftSet;
        return;
    }
    while (leftCurrentNode != NULL && !found){
        //dejo un nodo para comparar contra todos los nodos del otro set
        while (rightCurrentNode != NULL && !found){
            if (stateEquals(leftCurrentNode->state, rightCurrentNode->state)){
                found = 1;
                break;
            }
            rightCurrentNode = rightCurrentNode->next;
        }
        if (!found){
            resultCurrentNode = calloc(1, sizeof(StateNode));
            resultCurrentNode->state = leftCurrentNode->state;
            if (result->tail == NULL){
                result->first = resultCurrentNode;
            }
            if (result->tail != NULL){
                result->tail->next = resultCurrentNode;
            }
            result->tail = resultCurrentNode;
        }
        leftCurrentNode = leftCurrentNode->next;
        rightCurrentNode = rightSet->first;
        found = 0;
    }
    //free the nodes from left and right sets
    freeStateSet(leftSet);
    freeStateSet(rightSet);
}

static ComputationResult _symbolDifference(SymbolExpression * leftExp, SymbolExpression * rightExp){
    ComputationResult left = computeSymbolExpression(leftExp, false);
    ComputationResult right = computeSymbolExpression(rightExp, false);
    if (left.succeed && right.succeed){
        SymbolSet * result = calloc(1, sizeof(SymbolSet));
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
    SymbolNode * resultCurrentNode = NULL; 
    result->first = NULL;
    int found = 0;
    if (leftSet->tail == NULL){
        return;
    }
    if (rightSet->tail == NULL){
        result = leftSet;
        return;
    }
    while (leftCurrentNode != NULL && !found){
        //dejo un nodo para comparar contra todos los nodos del otro set
        while (rightCurrentNode != NULL && !found){
            if (symbolEquals(leftCurrentNode->symbol, rightCurrentNode->symbol)){
                found = 1;
                break;
            }
            rightCurrentNode = rightCurrentNode->next;
        }
        if (!found){
            resultCurrentNode = calloc(1, sizeof(SymbolNode));
            resultCurrentNode->symbol= leftCurrentNode->symbol;
            if (result->tail == NULL){
                result->first = resultCurrentNode;
            }
            if (result->tail != NULL){
                result->tail->next = resultCurrentNode;
            }
            result->tail = resultCurrentNode;
        }
        leftCurrentNode = leftCurrentNode->next;
        rightCurrentNode = rightSet->first;
        found = 0;
    }
    freeSymbolSet(leftSet);
    freeSymbolSet(rightSet);
}

/*---------------------------------------------- DELETE REPETITIONS FROM SET -----------------------------------------*/
static void deleteRepetitionsFromTransitionSet(TransitionSet * set){
    TransitionNode * current = set->first;
    TransitionNode * next;
    TransitionNode * lastSeen;

    while (current != NULL){
        next = current->next;
        lastSeen = current;
        while(next != NULL){
            if (transitionEquals(current->transition, next->transition)){
                lastSeen->next = next->next;
                free(next);
                next = lastSeen->next;
            } else {
                lastSeen = next;
                next = next->next;
            }
        }
        current = current->next;
    }
}

static void deleteRepetitionsFromStateSet(StateSet * set){
    StateNode * current = set->first;
    StateNode * next;
    StateNode * lastSeen;

    while (current != NULL){
        next = current->next;
        lastSeen = current;
        while(next != NULL){
            if (stateEquals(current->state, next->state)){
                lastSeen->next = next->next;
                free(next);
                next = lastSeen->next;
            } else {
                lastSeen = next;
                next = next->next;
            }
        }
        current = current->next;
    }
}

static void deleteRepetitionsFromSymbolSet(SymbolSet * set){
    SymbolNode * current = set->first;
    SymbolNode * next;
    SymbolNode * lastSeen;

    while (current != NULL){
        next = current->next;
        lastSeen = current;
        while(next != NULL){
            if (symbolEquals(current->symbol, next->symbol)){
                lastSeen->next = next->next;
                free(next);
                next = lastSeen->next;
            } else {
                lastSeen = next;
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

