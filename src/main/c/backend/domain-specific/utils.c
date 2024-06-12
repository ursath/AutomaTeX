#include "utils.h"

SymbolSet * cpySymbolSet(SymbolSet * set) {
    SymbolNode * currentNode = set->first;
    SymbolNode * resultTail;
    SymbolNode * node;
    SymbolSet * resultSet = malloc(sizeof(SymbolSet));
    while ( currentNode != NULL ){
        node = malloc(sizeof(SymbolNode));
        node->symbolExpression = currentNode->symbolExpression;
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
        node->stateExpression = currentNode->stateExpression;
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
        node->transitionExpression = currentNode->transitionExpression;
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
