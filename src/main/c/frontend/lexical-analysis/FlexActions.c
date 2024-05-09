#include "FlexActions.h"
#include <string.h>

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;
static boolean _logIgnoredLexemes = true;

void initializeFlexActionsModule() {
	_logIgnoredLexemes = getBooleanOrDefault("LOG_IGNORED_LEXEMES", _logIgnoredLexemes);
	_logger = createLogger("FlexActions");
}

void shutdownFlexActionsModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
}

/* PRIVATE FUNCTIONS */

static void _logLexicalAnalyzerContext(const char * functionName, LexicalAnalyzerContext * lexicalAnalyzerContext);

/**
 * Logs a lexical-analyzer context in DEBUGGING level.
 */
static void _logLexicalAnalyzerContext(const char * functionName, LexicalAnalyzerContext * lexicalAnalyzerContext) {
	char * escapedLexeme = escape(lexicalAnalyzerContext->lexeme);
	logDebugging(_logger, "%s: %s (context = %d, length = %d, line = %d)",
		functionName,
		escapedLexeme,
		lexicalAnalyzerContext->currentContext,
		lexicalAnalyzerContext->length,
		lexicalAnalyzerContext->line);
	free(escapedLexeme);
}

/* PUBLIC FUNCTIONS */

void BeginMultilineCommentLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	if (_logIgnoredLexemes) {
		_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	}
}

void EndMultilineCommentLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	if (_logIgnoredLexemes) {
		_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	}
}

void BeginInlineCommentLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	if (_logIgnoredLexemes) {
		_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	}
}

void EndInlineCommentLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	if (_logIgnoredLexemes) {
		_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	}
}

void IgnoredLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	if (_logIgnoredLexemes) {
		_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	}
}

Token AutomataLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	Token token;
	if (!strcmp(lexicalAnalyzerContext->lexeme, DFA_STRING)) {
		token = DFA;
	}else if (!strcmp(lexicalAnalyzerContext->lexeme, NFA_STRING)) {
		token = NFA;
	}else{
		token = LNFA;
	}
	lexicalAnalyzerContext->semanticValue->token = token;
	return token;
}


Token BraceLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	Token token;
	switch (lexicalAnalyzerContext->lexeme[0]) {
		case '{': token = OPEN_BRACE; break;
		case '}': token = CLOSE_BRACE; break;
	}
	lexicalAnalyzerContext->semanticValue->token = token;
	return token;
}

Token BracketLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	Token token;
	switch (lexicalAnalyzerContext->lexeme[0]) {
		case '[': token = OPEN_BRACKET; break;
		case ']': token = CLOSE_BRACKET; break;
	}
	lexicalAnalyzerContext->semanticValue->token = token;
	return token;
}


Token ParenthesisLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	Token token;
	switch (lexicalAnalyzerContext->lexeme[0]) {
		case '(': token = OPEN_PARENTHESIS; break;
		case ')': token = CLOSE_PARENTHESIS; break;
	}
	lexicalAnalyzerContext->semanticValue->token = token;
	return token;
}

Token ColonLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->token = COLON;
	return lexicalAnalyzerContext->semanticValue->token ;	
}

Token CommaLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->token = COMMA;
	return lexicalAnalyzerContext->semanticValue->token;	
}
Token PeriodLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext){
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->token = PERIOD;
	return lexicalAnalyzerContext->semanticValue->token;
}

Token SetOperatorLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	Token token;
	switch (lexicalAnalyzerContext->lexeme[0]) {
		case '-': token = DIFFERENCE; break;
		case '+': token = UNION; break;
		case '*': token = INTERSECTION; break;
	}
	lexicalAnalyzerContext->semanticValue->token = token;
	return token;
}

Token LambdaLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->token = LAMBDA;
	return lexicalAnalyzerContext->semanticValue->token;
}

Token TransitionLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	Token token;
	if( !strcmp(END_RIGHT_TRANSITION_STRING, lexicalAnalyzerContext->lexeme ) )
		token = END_RIGHT_TRANSITION;
	else if( !strcmp(END_RIGHT_TRANSITION_STRING, lexicalAnalyzerContext->lexeme ) )
		token = END_RIGHT_TRANSITION;
	else if( !strcmp(BEGIN_LEFT_TRANSITION_STRING, lexicalAnalyzerContext->lexeme ) )
		token = BEGIN_LEFT_TRANSITION;
	else 
		token = BEGIN_RIGHT_TRANSITION;

	lexicalAnalyzerContext->semanticValue->token = token;
	return token;
}


Token TransitionsKeywordLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->token = TRANSITIONS_KEYWORD;
	return lexicalAnalyzerContext->semanticValue->token;
}


Token StatesKeywordLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->token = STATES_KEYWORD;
	return lexicalAnalyzerContext->semanticValue->token;
}


Token AlphabetKeywordLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->token = ALPHABET_KEYWORD;
	return lexicalAnalyzerContext->semanticValue->token;
}

Token StatesSetKeywordLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	Token token;
	if (!strcmp(lexicalAnalyzerContext->lexeme, FINAL_STATES_KEYWORD_STRING)) {
		token = FINAL_STATES_KEYWORD;
	}
	else if(!strcmp(INITIAL_STATES_KEYWORD_STRING, lexicalAnalyzerContext->lexeme ) )
		token = INITIAL_STATES_KEYWORD;
	else 
		token = REGULAR_STATES_KEYWORD;

	lexicalAnalyzerContext->semanticValue->token = token;
	return token;
}

Token UnknownLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	return UNKNOWN;
}


Token StateTypeLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	Token token;
	switch(lexicalAnalyzerContext->lexeme[0]){
		case '*': token = FINAL_STATE; break;
		case '>': token = INITIAL_STATE; break;
		default: token = SYMBOL; break;
}
	lexicalAnalyzerContext->semanticValue->token = token;
	return token;
}

Token SymbolLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->token = SYMBOL;
	// * guardo valor del symbol
	lexicalAnalyzerContext->semanticValue->value = lexicalAnalyzerContext->lexeme;
	return lexicalAnalyzerContext->semanticValue->token;
}

Token IdentifierLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->token = IDENTIFIER;
	lexicalAnalyzerContext->semanticValue->value = lexicalAnalyzerContext->lexeme;
	return lexicalAnalyzerContext->semanticValue->token;
}

Token NewLineLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->token = NEW_LINE;
	return lexicalAnalyzerContext->semanticValue->token;
}

Token EmptyKeywordLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext){
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->token = EMPTY;
	return lexicalAnalyzerContext->semanticValue->token;
}