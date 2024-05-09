#ifndef FLEX_ACTIONS_HEADER
#define FLEX_ACTIONS_HEADER

#include "../../shared/Environment.h"
#include "../../shared/Logger.h"
#include "../../shared/String.h"
#include "../../shared/Type.h"
#include "../syntactic-analysis/AbstractSyntaxTree.h"
#include "../syntactic-analysis/BisonParser.h"
#include "LexicalAnalyzerContext.h"
#include <stdio.h>
#include <stdlib.h>

/** Initialize module's internal state. */
void initializeFlexActionsModule();

/** Shutdown module's internal state. */
void shutdownFlexActionsModule();

#define END_LEFT_TRANSITION_STRING "|-"
#define BEGIN_LEFT_TRANSITION_STRING "-|"
#define BEGIN_RIGHT_TRANSITION_STRING "<-"
#define END_RIGHT_TRANSITION_STRING "->"
#define NFA_STRING "NFA"
#define DFA_STRING "DFA"
#define LNFA_STRING "LNFA"

#define REGULAR_STATES_KEYWORD_STRING "regular"
#define FINAL_STATES_KEYWORD_STRING "final"
#define INITIAL_STATES_KEYWORD_STRING "initial"
/**
 * Flex lexeme processing actions.
 */

void BeginMultilineCommentLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
void EndMultilineCommentLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
void BeginInlineCommentLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
void EndInlineCommentLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
void IgnoredLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

// todo s:, for every, where and is /is not*/
//todo: implement {
Token DefKeywordLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token MinimalKeywordLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
//todo: }
Token AutomataLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

Token StatesKeywordLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token TransitionsKeywordLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token AlphabetKeywordLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

Token StatesSetKeywordLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

Token SetOperatorLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

Token BraceLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token BracketLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token ParenthesisLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token ColonLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token CommaLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token PeriodLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

Token TransitionLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

Token LambdaLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

Token StateTypeLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

Token SymbolLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token IdentifierLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

Token EmptyKeywordLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

Token NewLineLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

Token UnknownLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
#endif