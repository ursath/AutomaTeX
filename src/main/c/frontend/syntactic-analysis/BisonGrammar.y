%{

#include "BisonActions.h"

%}

%define api.value.union.name SemanticValue

%union {
	/** Terminals. */

	char * value;
	Token token;

	/** Non-terminals. */

	Program * program;
	Symbol * symbol;
	Transition * transition;
	State * state;
	SymbolSet * symbolSet;
	StateSet * stateSet;
	TransitionSet * transitionSet;
	Automata * automata;
	Definition * definition;
	TransitionExpression * transitionExpression;
	StateExpression * stateExpression;
	SymbolExpression * symbolExpression;
	AutomataType automataType;
	DefinitionSet * definitionSet;
	StateType stateType;
	StateNode * stateNode;
	SymbolNode * symbolNode;
	TransitionNode * transitionNode;
}

/**
 * Destructors. This functions are executed after the parsing ends, so if the
 * AST must be used in the following phases of the compiler you shouldn't used
 * this approach.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Destructor-Decl.html
 */
/*
%destructor { releaseConstant($$); } <constant>
%destructor { releaseExpression($$); } <expression>
%destructor { releaseFactor($$); } <factor>
%destructor { releaseProgram($$); } <program>
*/

/** Terminals. */
%token <token> COMMA 
%token <token> COLON
%token <token> OPEN_PARENTHESIS
%token <token> CLOSE_PARENTHESIS
%token <token> OPEN_BRACE
%token <token> CLOSE_BRACE
%token <token> OPEN_BRACKET
%token <token> CLOSE_BRACKET
%token <value> SYMBOL
%token <token> UNION
%token <token> DIFFERENCE
%token <token> INTERSECTION
%token <token> LAMBDA
%token <token> EMPTY
%token <token> BEGIN_RIGHT_TRANSITION
%token <token> END_RIGHT_TRANSITION
%token <token> BEGIN_LEFT_TRANSITION
%token <token> END_LEFT_TRANSITION
%token <token> TRANSITIONS_KEYWORD
%token <token> STATES_KEYWORD
%token <token> ALPHABET_KEYWORD
%token <token> DFA
%token <token> NFA
%token <token> LNFA
%token <token> REGULAR_STATES_KEYWORD
%token <token> FINAL_STATES_KEYWORD
%token <token> INITIAL_STATES_KEYWORD
%token <token> FINAL_STATE
%token <token> INITIAL_STATE
%token <value> IDENTIFIER
%token <token> NEW_LINE
%token <token> PERIOD
%token <token> PIPE
%token <token> UNKNOWN

/*faltarían agregar a flex también
%token <token> AND
%token <token> OR
%token <token> NOT
%token <token> EQUAL
%token <token> DEF
%token <token> IF
%token <token> ELSE
%token <token> IN
%token <token> DO
%token <token> FOR EVERY
%token <token> MINIMAL
*/
//faltaría ver el caso donde se tiene un automata y se acceden a sus estados iniciales finales y no finales ni iniciales

/** Non-terminals. */
%type <symbolExpression> symbolExpression
%type <stateExpression> stateExpression
%type <transitionExpression> transitionExpression
%type <program> program
%type <symbol> symbol
%type <state> state
%type <transition> transition
%type <symbolSet> symbolSet
%type <stateSet> stateSet
%type <transitionSet> transitionSet
%type <automata> automata
%type <definition> definition
%type <automataType> automataType
%type <definitionSet> definitionSet
%type <stateType> stateType
%type <stateNode> stateNode
%type <symbolNode> symbolNode
%type <transitionNode> transitionNode
//revisar
//%type <function> function
//%type <forLoop> forLoop

/**
 * Precedence and associativity.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Precedence.html
 */
%left OPEN_PARENTHESIS
%left CLOSE_PARENTHESIS
%left DIFFERENCE 
%left INTERSECTION 
%left UNION

%%

program: definitionSet																			{ $$ = ExpressionProgramSemanticAction(currentCompilerState(), $1); }
	;

definitionSet: definition[left] NEW_LINE definitionSet[right]									{ $$ = DefinitionSetSemanticAction($left, $right); }
	| definition NEW_LINE																		{ $$ = SingularDefinitionSetSemanticAction($1); }
	;

definition: automataType[type] IDENTIFIER[identifier] automata[element] 						{ $$ = AutomataDefinitionSemanticAction($type, $identifier, $element); }
	| TRANSITIONS_KEYWORD IDENTIFIER[identifier] COLON transitionExpression[expression] 				{ $$ = TransitionExpressionDefinitionSemanticAction($identifier, $expression); }
	| ALPHABET_KEYWORD IDENTIFIER[identifier] COLON symbolExpression[expression] 						{ $$ = SymbolExpressionDefinitionSemanticAction($identifier, $expression); }	
	| STATES_KEYWORD IDENTIFIER[identifier] COLON stateExpression[expression] 							{ $$ = StateExpressionDefinitionSemanticAction($identifier, $expression); }	
	;		
	
automata: OPEN_BRACKET STATES_KEYWORD COLON stateExpression[states] COMMA ALPHABET_KEYWORD COLON symbolExpression[symbols] COMMA TRANSITIONS_KEYWORD COLON transitionExpression[transitions] CLOSE_BRACKET						{ $$ = AutomataSemanticAction($states, $symbols, $transitions); }
	;

automataType: DFA																				{ $$ = DFA_AUTOMATA; }
	| NFA																						{ $$ = NFA_AUTOMATA; }
	| LNFA																						{ $$ = LNFA_AUTOMATA; }
	;

transitionExpression:  transitionExpression[left] UNION transitionExpression[right] 			{ $$ = TransitionExpressionSemanticAction($left, $right, UNION); }
	| transitionExpression[left] DIFFERENCE transitionExpression[right] 						{ $$ = TransitionExpressionSemanticAction($left, $right, DIFFERENCE); }
	| transitionExpression[left] INTERSECTION transitionExpression[right] 						{ $$ = TransitionExpressionSemanticAction($left, $right, INTERSECTION); }																												
	| transitionSet 																			{ $$ = SetTransitionExpressionSemanticAction($1); }	
	| transition 																				{ $$ = SingularTransitionExpressionSemanticAction($1); }	
	| OPEN_PARENTHESIS transitionExpression CLOSE_PARENTHESIS									{ $$ = $2 ;}																														
	;

stateExpression:  stateExpression[left] UNION stateExpression[right] 							{ $$ = StateExpressionSemanticAction($left, $right, UNION); }
	| stateExpression[left] DIFFERENCE stateExpression[right] 									{ $$ = StateExpressionSemanticAction($left, $right, DIFFERENCE); }
	| stateExpression[left] INTERSECTION stateExpression[right] 								{ $$ = StateExpressionSemanticAction($left, $right, INTERSECTION); }
	| stateSet																					{ $$ = SetStateExpressionSemanticAction($1); }
	| state																						{ $$ = SingularStateExpressionSemanticAction($1); }	
	| OPEN_PARENTHESIS stateExpression CLOSE_PARENTHESIS										{ $$ = $2; }
	;

symbolExpression: symbolExpression[left] UNION symbolExpression[right]							{ $$ = SymbolExpressionSemanticAction($left, $right, UNION); }
	| symbolExpression[left] DIFFERENCE symbolExpression[right]									{ $$ = SymbolExpressionSemanticAction($left, $right, DIFFERENCE); }
	| symbolExpression[left] INTERSECTION symbolExpression[right] 								{ $$ = SymbolExpressionSemanticAction($left, $right, INTERSECTION); }
	| symbolSet 																				{ $$ = SetSymbolExpressionSemanticAction($1); }
	| symbol																					{ $$ = SingularSymbolExpressionSemanticAction($1); }
	| OPEN_PARENTHESIS symbolExpression CLOSE_PARENTHESIS										{ $$ = $2; }	
	;
	
stateNode: stateExpression															{ $$ = SingularExpressionStateNodeSemanticAction($1); }				
		| stateExpression COMMA stateNode											{ $$ = ExpressionsStateNodeSemanticAction($1, $3); }
		;

stateSet: OPEN_BRACE stateNode[node] CLOSE_BRACE									{ $$ = NodeStateSetSemanticAction($node); }
	| EMPTY																			{ $$ = EmptyStateSetSemanticAction();}	
	| IDENTIFIER																	{ $$ = IdentifierStateSetSemanticAction($1,false); }
	| IDENTIFIER PERIOD STATES_KEYWORD												{ $$ = IdentifierStateSetSemanticAction($1,true); }	
	| IDENTIFIER[identifier] PERIOD STATES_KEYWORD PERIOD stateType[typeSet]		{ $$ = StateTypeSetSemanticAction($identifier, $typeSet, true); }	
	| IDENTIFIER[identifier] PERIOD stateType[typeSet]								{ $$ = StateTypeSetSemanticAction($identifier, $typeSet, false); }	
	;

state: symbol																{ $$ = StateSemanticAction(false, false, $1); }
	| FINAL_STATE symbol													{ $$ = StateSemanticAction(false, true, $2); }
	| INITIAL_STATE	symbol													{ $$ = StateSemanticAction(true, false, $2); }
	| INITIAL_STATE FINAL_STATE symbol										{ $$ = StateSemanticAction(true, true, $3); }
	| FINAL_STATE INITIAL_STATE symbol										{ $$ = StateSemanticAction(true, true, $3); }
	;

stateType: REGULAR_STATES_KEYWORD											{ $$ = REGULAR; }
	| INITIAL_STATES_KEYWORD												{ $$ = INITIAL;}
	| FINAL_STATES_KEYWORD													{ $$ = FINAL;}
	;

transitionNode: transitionExpression										{ $$ = SingularExpressionTransitionNodeSemanticAction($1);}
	| transitionExpression COMMA transitionNode 							{ $$ = ExpressionsTransitionNodeSemanticAction($1, $3); }
	;

transitionSet: OPEN_BRACE transitionNode[node] CLOSE_BRACE																						{ $$ = NodeTransitionSetSemanticAction($node);}
	| EMPTY																																		{ $$ = EmptyTransitionSetSemanticAction();}	
	| IDENTIFIER																																{ $$ = IdentifierTransitionSetSemanticAction($1, false);}
	| IDENTIFIER PERIOD TRANSITIONS_KEYWORD																										{ $$ = IdentifierTransitionSetSemanticAction($1, true);}
	| PIPE stateExpression[left] PIPE END_LEFT_TRANSITION symbolExpression[middle] END_RIGHT_TRANSITION PIPE stateExpression[right] PIPE		{ $$ = BothSideTransitionSemanticAction($left, $right, $middle); }
	;

transition: PIPE stateExpression[left] PIPE END_LEFT_TRANSITION symbolExpression[middle] BEGIN_LEFT_TRANSITION stateExpression[right] PIPE		{ $$ = LeftTransitionSemanticAction($left, $right, $middle); }
	| PIPE stateExpression[left] BEGIN_RIGHT_TRANSITION symbolExpression[middle] END_RIGHT_TRANSITION PIPE stateExpression[right] PIPE			{ $$ = RightTransitionSemanticAction($left, $right, $middle); }
	;

symbolNode: symbolExpression												{ $$ = SingularExpressionSymbolNodeSemanticAction($1);}
	| symbolExpression COMMA symbolNode										{ $$ = ExpressionsSymbolNodeSemanticAction($1, $3); }							
	;

symbolSet: OPEN_BRACE symbolNode[node] CLOSE_BRACE							{ $$ = NodeSymbolSetSemanticAction($node);}
	| EMPTY																	{ $$ = EmptySymbolSetSemanticAction();}	
	| IDENTIFIER															{ $$ = IdentifierSymbolSetSemanticAction($1,false); }
	| IDENTIFIER PERIOD ALPHABET_KEYWORD									{ $$ = IdentifierSymbolSetSemanticAction($1,true); }
	;

symbol: SYMBOL 																{ $$ = SymbolSemanticAction($1); }
	| LAMBDA 																{ $$ = LambdaSemanticAction($1); }
	;

%%
