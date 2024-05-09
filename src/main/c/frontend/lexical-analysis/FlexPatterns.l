%{

#include "FlexActions.h"

%}

/**
 * Enable start condition manipulation functions.
 */
%option stack

/**
 * Flex contexts (a.k.a. start conditions).
 *
 * @see https://westes.github.io/flex/manual/Start-Conditions.html
 */
%x MULTILINE_COMMENT
%x INLINE_COMMENT



/**
 * Reusable patterns.
 *
 * @see https://westes.github.io/flex/manual/Matching.html
 * @see https://westes.github.io/flex/manual/Patterns.html
 */

%%

"/*"								{ BEGIN(MULTILINE_COMMENT); BeginMultilineCommentLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>"*/"				{ EndMultilineCommentLexemeAction(createLexicalAnalyzerContext()); BEGIN(INITIAL); }
<MULTILINE_COMMENT>[[:space:]]+		{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>[^*]+			{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>.				{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }

"//"								{ BEGIN(INLINE_COMMENT); BeginInlineCommentLexemeAction(createLexicalAnalyzerContext()); }
<INLINE_COMMENT>"\n"				{ EndInlineCommentLexemeAction(createLexicalAnalyzerContext()); BEGIN(INITIAL); }
<INLINE_COMMENT>[^\n]+				{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }


"EMPTY"                             { return EmptyKeywordLexemeAction(createLexicalAnalyzerContext()); }

"DFA"                               { return AutomataLexemeAction(createLexicalAnalyzerContext()); }
"NFA"                               { return AutomataLexemeAction(createLexicalAnalyzerContext()); }
"LNFA"                              { return AutomataLexemeAction(createLexicalAnalyzerContext()); }

"states"                            { return StatesKeywordLexemeAction(createLexicalAnalyzerContext()); }
"transitions"                       { return TransitionsKeywordLexemeAction(createLexicalAnalyzerContext()); }
"alphabet"                          { return AlphabetKeywordLexemeAction(createLexicalAnalyzerContext()); }

"regular"                           { return StatesSetKeywordLexemeAction(createLexicalAnalyzerContext()); }
"initial"                           { return StatesSetKeywordLexemeAction(createLexicalAnalyzerContext()); }
"final"                             { return StatesSetKeywordLexemeAction(createLexicalAnalyzerContext()); }

"-"									{ return SetOperatorLexemeAction(createLexicalAnalyzerContext()); }
"+"									{ return SetOperatorLexemeAction(createLexicalAnalyzerContext()); }
"^"                                 { return SetOperatorLexemeAction(createLexicalAnalyzerContext()); }

"{"                                 { return BraceLexemeAction(createLexicalAnalyzerContext()); }
"}"                                 { return BraceLexemeAction(createLexicalAnalyzerContext()); }
"["                                 { return BracketLexemeAction(createLexicalAnalyzerContext()); }
"]"                                 { return BracketLexemeAction(createLexicalAnalyzerContext()); }
"("									{ return ParenthesisLexemeAction(createLexicalAnalyzerContext()); }
")"									{ return ParenthesisLexemeAction(createLexicalAnalyzerContext()); }
":"									{ return ColonLexemeAction(createLexicalAnalyzerContext()); }
","									{ return CommaLexemeAction(createLexicalAnalyzerContext()); }
"."                                 { return PeriodLexemeAction(createLexicalAnalyzerContext()); }

"->"                                { return TransitionLexemeAction(createLexicalAnalyzerContext()); }
"<-"                                { return TransitionLexemeAction(createLexicalAnalyzerContext()); }
"-|"                                { return TransitionLexemeAction(createLexicalAnalyzerContext()); }
"|-"                                { return TransitionLexemeAction(createLexicalAnalyzerContext()); }

"@"                                 { return LambdaLexemeAction(createLexicalAnalyzerContext()); }      

">"                                 { return StateTypeLexemeAction(createLexicalAnalyzerContext()); } 
"*"                                 { return StateTypeLexemeAction(createLexicalAnalyzerContext()); }

"\n"                                { return NewLineLexemeAction(createLexicalAnalyzerContext()); }

[[:digit:][:lower:]]+				{ return SymbolLexemeAction(createLexicalAnalyzerContext()); }
[[:upper:][:alpha:]*]               { return IdentifierLexemeAction(createLexicalAnalyzerContext()); }

[[:space:]]+						{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
.									{ return UnknownLexemeAction(createLexicalAnalyzerContext()); }

%%

/*
.									{ return UnknownLexemeAction(createLexicalAnalyzerContext());}
"minimal"                           { return MinimalKeywordLexemeAction(createLexicalAnalyzerContext()); }
"def"                               { return DefKeywordLexemeAction(createLexicalAnalyzerContext()); } 
 */
#include "FlexExport.h"
