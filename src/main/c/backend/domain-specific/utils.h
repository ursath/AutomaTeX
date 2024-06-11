#ifndef UTILS_HEADER
#define UTILS_HEADER


#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"

SymbolSet * cpySymbolSet(SymbolSet * symbolSet);

StateSet * cpyStateSet(StateSet * StateSet);

TransitionSet * cpyTransitionSet(TransitionSet * StateSet);
