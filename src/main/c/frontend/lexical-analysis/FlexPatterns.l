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

"DFA"                               { return AutomataLexemeAction(createLexicalAnalyzerContext(), DFA); }
"NFA"                               { return AutomataLexemeAction(createLexicalAnalyzerContext(), NFA); }
"LNFA"                              { return AutomataLexemeAction(createLexicalAnalyzerContext(), LNFA); }

"states"                            { return StatesKeywordLexemeAction(createLexicalAnalyzerContext()); }
"transitions"                       { return TransitionsKeywordLexemeAction(createLexicalAnalyzerContext()); }
"alphabet"                          { return AlphabetKeywordLexemeAction(createLexicalAnalyzerContext()); }

"regular"                           { return StatesSetKeywordLexemeAction(createLexicalAnalyzerContext(), REGULAR_STATES_KEYWORD); }
"initial"                           { return StatesSetKeywordLexemeAction(createLexicalAnalyzerContext(), INITIAL_STATES_KEYWORD); }
"final"                             { return StatesSetKeywordLexemeAction(createLexicalAnalyzerContext(), FINAL_STATES_KEYWORD); }

"EMPTY"                             { return EmptyKeywordLexemeAction(createLexicalAnalyzerContext()); }
"{}"                                { return EmptyKeywordLexemeAction(createLexicalAnalyzerContext()); }

"-"									{ return SetOperatorLexemeAction(createLexicalAnalyzerContext(), DIFFERENCE); }
"+"									{ return SetOperatorLexemeAction(createLexicalAnalyzerContext(), UNION); }
"^"                                 { return SetOperatorLexemeAction(createLexicalAnalyzerContext(), INTERSECTION); }

"{"                                 { return BraceLexemeAction(createLexicalAnalyzerContext(), OPEN_BRACE); }
"}"                                 { return BraceLexemeAction(createLexicalAnalyzerContext(), CLOSE_BRACE); }
"["                                 { return BracketLexemeAction(createLexicalAnalyzerContext(), OPEN_BRACKET); }
"]"                                 { return BracketLexemeAction(createLexicalAnalyzerContext(), CLOSE_BRACKET); }
"("									{ return ParenthesisLexemeAction(createLexicalAnalyzerContext(), OPEN_PARENTHESIS); }
")"									{ return ParenthesisLexemeAction(createLexicalAnalyzerContext(), CLOSE_PARENTHESIS); }
":"									{ return ColonLexemeAction(createLexicalAnalyzerContext()); }
","									{ return CommaLexemeAction(createLexicalAnalyzerContext()); }
"."                                 { return PeriodLexemeAction(createLexicalAnalyzerContext()); }

"|"                                 { return TransitionLexemeAction(createLexicalAnalyzerContext(), PIPE); }
"->"                                { return TransitionLexemeAction(createLexicalAnalyzerContext(), END_RIGHT_TRANSITION); }
"<-"                                { return TransitionLexemeAction(createLexicalAnalyzerContext(), END_LEFT_TRANSITION); }
"-|"                                { return TransitionLexemeAction(createLexicalAnalyzerContext(), BEGIN_LEFT_TRANSITION); }
"|-"                                { return TransitionLexemeAction(createLexicalAnalyzerContext(), BEGIN_RIGHT_TRANSITION); }

"@"                                 { return LambdaLexemeAction(createLexicalAnalyzerContext()); }      

">"                                 { return StateTypeLexemeAction(createLexicalAnalyzerContext(), INITIAL_STATE); } 
"*"                                 { return StateTypeLexemeAction(createLexicalAnalyzerContext(), FINAL_STATE); }

";"                                 { return NewLineLexemeAction(createLexicalAnalyzerContext()); }

[a-z][A-Za-z0-9]*    				{ return SymbolLexemeAction(createLexicalAnalyzerContext()); }
[A-Z][A-Za-z0-9]*                   { return IdentifierLexemeAction(createLexicalAnalyzerContext()); }

[[:space:]]+						{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }

.									{ return UnknownLexemeAction(createLexicalAnalyzerContext());}
%%

#include "FlexExport.h"
