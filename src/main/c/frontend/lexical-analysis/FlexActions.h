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

/**
 * Flex lexeme processing actions.
 */

void BeginMultilineCommentLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
void EndMultilineCommentLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
void BeginInlineCommentLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
void EndInlineCommentLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
void IgnoredLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

Token AutomataLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);

Token StatesKeywordLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token TransitionsKeywordLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token AlphabetKeywordLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

Token StatesSetKeywordLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);

Token SetOperatorLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);

Token BraceLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);
Token BracketLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);
Token ParenthesisLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);
Token ColonLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token CommaLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token PeriodLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

Token TransitionLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);

Token LambdaLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

Token StateTypeLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);

Token SymbolLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token IdentifierLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

Token EmptyKeywordLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

Token NewLineLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

Token UnknownLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
#endif