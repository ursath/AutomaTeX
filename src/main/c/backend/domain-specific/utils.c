#include "utils.h"

static Logger * _logger = NULL;


/*----------------------------------------- SET CONTAINS ----------------------------------------------------*/ 
boolean containsState(StateNode *first, State * state ) {
    StateNode * currentNode = first;
    State * currentState;
    while ( currentNode != NULL ){
        currentState = currentNode->state;
        if ( stateSymbolEquals(state,currentState) )   // pues no me imp si son final / initial
            return true; 
        
        currentNode = currentNode->next; 
    }
    return false;
}

boolean containsSymbol(SymbolNode *first, Symbol * symbol ) {
    SymbolNode * currentNode = first;
    Symbol * currentSymbol;
    while ( currentNode != NULL ){
        currentSymbol = currentNode->symbol;
        if ( symbolEquals(symbol,currentSymbol) )
            return true; 
        
        currentNode = currentNode->next; 
    }
    return false;
}

/*----------------------------------------- COPY SET ----------------------------------------------------*/ 
SymbolSet * cpySymbolSet(SymbolSet * set) {
    SymbolNode * currentNode = set->first;
    SymbolNode * resultTail;
    SymbolNode * node;
    SymbolSet * resultSet = malloc(sizeof(SymbolSet));
    while ( currentNode != NULL ){
        node = malloc(sizeof(SymbolNode));
        node->symbol = currentNode->symbol; 
        if ( resultSet->first==NULL )
            resultSet->first = node;
        else 
            resultTail->next = node;
        resultTail = node; 
        
        currentNode = currentNode->next; 
    }
    resultSet->tail = resultTail;
    return resultSet;
}

StateSet * cpyStateSet(StateSet * set){
    StateNode * currentNode = set->first;
    StateNode * resultTail;
    StateNode * node;
    StateSet * resultSet = malloc(sizeof(StateSet));
    while ( currentNode != NULL ){
        node = malloc(sizeof(StateNode));
        node->state = currentNode->state;
        if ( resultSet->first==NULL )
            resultSet->first = node;
        else 
            resultTail->next = node;
        resultTail = node; 
        
        currentNode = currentNode->next; 
    }
    resultSet->tail = resultTail;
    return resultSet;

}

TransitionSet * cpyTransitionSet(TransitionSet * set){
    TransitionNode * currentNode = set->first;
    TransitionNode * resultTail;
    TransitionNode * node;
    TransitionSet * resultSet = malloc(sizeof(TransitionSet));
    while ( currentNode != NULL ){
        node = malloc(sizeof(TransitionNode));
        node->transition = currentNode->transition;
        if ( resultSet->first==NULL )
            resultSet->first = node;
        else 
            resultTail->next = node;
        resultTail = node; 
        
        currentNode = currentNode->next; 
    }
    resultSet->tail = resultTail;
    return resultSet;
}


/*----------------------------------------- SET EQUALS ----------------------------------------------------*/ 
 boolean transitionSetEquals(TransitionSet * set1, TransitionSet * set2){
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


 boolean stateSetEquals(StateSet * set1, StateSet * set2){
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

 boolean symbolSetEquals(SymbolSet *set1, SymbolSet *set2){
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
 boolean transitionEquals(Transition * trans1, Transition * trans2){
    return stateEquals(trans1->fromExpression->state, trans2->fromExpression->state) && stateEquals(trans1->toExpression->state, trans2->toExpression->state) && symbolEquals(trans1->symbolExpression->symbol, trans2->symbolExpression->symbol);
}

 boolean stateEquals(State * state1, State * state2){
    return  symbolEquals(&state1->symbol, &state2->symbol) && state1->isFinal == state2->isFinal && state1->isInitial == state2->isInitial;
}

 boolean stateSymbolEquals(State * state1, State * state2){
    return  symbolEquals(&state1->symbol, &state2->symbol);
}

 boolean symbolEquals(Symbol * symbol1, Symbol * symbol2){
    return strcmp(symbol1->value, symbol2->value) == 0;
}

/*-------------------------------------- FREE NODE SETS --------------------*/
void freeTransitionSet(TransitionSet * set){
    TransitionNode * current = set->first;
    TransitionNode * next;
    while (current != NULL){
        next = current->next;
        free(current);
        current = next;
    }
    free(set);
}

void freeStateSet(StateSet * set){
    StateNode * current = set->first;
    StateNode * next;
    while (current != NULL){
        next = current->next;
        free(current);
        current = next;
    }
    free(set);
}

void freeSymbolSet(SymbolSet * set){
    SymbolNode * current = set->first;
    SymbolNode * next;
    while (current != NULL){
        next = current->next;
        free(current);
        current = next;
    }
    free(set);
} 
