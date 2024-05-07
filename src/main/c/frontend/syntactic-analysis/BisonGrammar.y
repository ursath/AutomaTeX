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
	AutomataType * automataType;
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
%token <token> SYMBOL
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
//acá serían de tipo state (tienen un valor asociado)
%token <token> FINAL_STATE
%token <token> INITIAL_STATE
//identifier tendría un string
%token <token> IDENTIFIER

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
//revisar
//%type <function> function
//%type <forLoop> forLoop

/**
 * Precedence and associativity.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Precedence.html
 */
%left DIFFERENCE UNION
%left MUL DIV

%%

 
program: definition															{ $$ = ExpressionProgramSemanticAction(currentCompilerState(), $1); }
	/* EXPRESSION */
	;

definition: automataType IDENTIFIER OPEN_BRACKET automata CLOSE_BRACKET 							{ $$ = AutomataDefinitionSemanticAction($1, $2, $4); }
	/*| DEF name function
	| STATES_KEYWORD IDENTIFIER COLON OPEN_BRACE stateSet CLOSE_BRACE  								{ $$ = StateSetDefinitionSemanticAction($2, $5); }	
	| STATES_KEYWORD IDENTIFIER COLON state															{ $$ = SingularStateSetDefinitionSemanticAction($2, $4); }
	| ALPHABET_KEYWORD IDENTIFIER COLON OPEN_BRACE symbolSet CLOSE_BRACE 							{ $$ = SymbolSetDefinitionSemanticAction($2, $5); }
	| ALPHABET_KEYWORD IDENTIFIER COLON symbol 														{ $$ = SingularSymbolSetDefinitionSemanticAction($2, $4); }
	| TRANSITIONS_KEYWORD IDENTIFIER COLON OPEN_BRACE transitionSet	CLOSE_BRACE						{ $$ = TransitionSetDefinitionSemanticAction($2, $5); }
	| TRANSITIONS_KEYWORD IDENTIFIER COLON transition												{ $$ = SingularTransitionSetDefinitionSemanticAction($2, $4); }
	*/
	| STATES_KEYWORD IDENTIFIER COLON stateExpression															{ $$ = SingularStateSetDefinitionSemanticAction($2, $4); }
	| ALPHABET_KEYWORD IDENTIFIER COLON symbolExpression 														{ $$ = SingularSymbolSetDefinitionSemanticAction($2, $4); }
	| TRANSITIONS_KEYWORD IDENTIFIER COLON transitionExpression												{ $$ = SingularTransitionSetDefinitionSemanticAction($2, $4); }
	;
	
automata: STATES_KEYWORD COLON stateExpression[states] COMMA ALPHABET_KEYWORD COLON symbolExpression[symbols] COMMA TRANSITIONS_KEYWORD COLON transitionExpression[transitions]						{ $$ = AutomataSemanticAction($states, $symbols, $transitions); }
	;

automataType: DFA													{ $$ = AutomataTypeAction($1) }
	| NFA															{ $$ = AutomataTypeAction($1) }
	| LNFA															{ $$ = AutomataTypeAction($1) }
	;
/* function:  OPEN_BRACKET block CLOSE_BRACKET; */				

transitionExpression: OPEN_PARENTHESIS transitionExpression[left] UNION transitionExpression[right] CLOSE_PARENTHESIS			{ $$ = ArithmeticTransitionExpressionSemanticAction($left, $right, UNION); }
	| OPEN_PARENTHESIS transitionExpression[left] DIFFERENCE transitionExpression[right] CLOSE_PARENTHESIS						{ $$ = ArithmeticTransitionExpressionSemanticAction($left, $right, DIFFERENCE); }
	| OPEN_PARENTHESIS transitionExpression[left] INTERSECTION transitionExpression[right] CLOSE_PARENTHESIS					{ $$ = ArithmeticTransitionExpressionSemanticAction($left, $right, INTERSECTION); }																												
	| OPEN_BRACE transitionSet CLOSE_BRACE																						{ $$ = ArithmeticTransitionSetSemanticAction($2); }
	| transition																														{ $$ = ArithmeticSingularTransitionSetSemanticAction($1); }
	| EMPTY							{ $$ = ArithmeticTransitionSetSemanticAction(EmptyTransitionSetSemanticAction()) ;}	
	;

stateExpression: OPEN_PARENTHESIS stateExpression[left] UNION stateExpression[right] CLOSE_PARENTHESIS							{ $$ = ArithmeticStateExpressionSemanticAction($left, $right, UNION); }
	| OPEN_PARENTHESIS stateExpression[left] DIFFERENCE stateExpression[right] CLOSE_PARENTHESIS								{ $$ = ArithmeticStateExpressionSemanticAction($left, $right, DIFFERENCE); }
	| OPEN_PARENTHESIS stateExpression[left] INTERSECTION stateExpression[right] CLOSE_PARENTHESIS								{ $$ = ArithmeticStateExpressionSemanticAction($left, $right, INTERSECTION); }
	| OPEN_BRACE stateSet CLOSE_BRACE																							{ $$ = ArithmeticStateSetSemanticAction($2); }
	| state																														{ $$ = ArithmeticSingularStateSetSemanticAction($1); }	
	| EMPTY 									{ $$ = ArithmeticStateSetSemanticAction(EmptyStateSetSemanticAction()) ;}	
	;

symbolExpression: OPEN_PARENTHESIS symbolExpression[left] UNION symbolExpression[right] CLOSE_PARENTHESIS						{ $$ = ArithmeticSymbolExpressionSemanticAction($left, $right, UNION); }
	| OPEN_PARENTHESIS symbolExpression[left] DIFFERENCE symbolExpression[right] CLOSE_PARENTHESIS								{ $$ = ArithmeticSymbolExpressionSemanticAction($left, $right, DIFFERENCE); }
	| OPEN_PARENTHESIS symbolExpression[left] INTERSECTION symbolExpression[right] CLOSE_PARENTHESIS							{ $$ = ArithmeticSymbolExpressionSemanticAction($left, $right, INTERSECTION); }
	| OPEN_BRACE symbolSet CLOSE_BRACE																							{ $$ = ArithmeticSymbolSetSemanticAction($2); }
	| symbol																													{ $$ = ArithmeticSingularSymbolSetSemanticAction($1); }
	| EMPTY 																							 			{ $$ = ArithmeticSymbolSetSemanticAction(EmptySymbolSetSemanticAction()) ;}	
	;


stateSet: /*OPEN_BRACE stateSet CLOSE_BRACE 							{ $$ = $2 }*/
	stateExpression[left] COMMA stateExpression[right]							{ $$ = StateSetsSemanticAction($left, $right); }
	| stateExpression												{ $$ = ArithmeticStateSetSemanticAction($1) }
/*	| state 														{ $$ = SingularStateSetSemanticAction($1) }
	| EMPTY															{ $$ = EmptyStateSetSemanticAction();}	*/
	;

state: SYMBOL														{ $$ = StateSemanticAction(false, false, $1) }
	| FINAL_STATE SYMBOL											{ $$ = StateSemanticAction(false, true, $2) }
	| INITIAL_STATE	SYMBOL											{ $$ = StateSemanticAction(true, false, $2) }
	| INITIAL_STATE FINAL_STATE SYMBOL								{ $$ = StateSemanticAction(true, true, $3) }
	| FINAL_STATE INITIAL_STATE SYMBOL								{ $$ = StateSemanticAction(true, true, $3) }
	;

transitionSet: /*OPEN_BRACE transitionSet CLOSE_BRACE 				{ $$ = $2; }*/
	transitionExpression[left] COMMA transitionExpression[right]				{ $$ = TransitionSetSemanticAction($left, $right); }	
	| transitionExpression											{ $$ = ArithmeticTransitionSetSemanticAction($1); }
/*	| transition													{ $$ = SingularTransitionSetSemanticAction($1); }
	| EMPTY															{ $$ = EmptyTransitionSetSemanticAction(); } */
	;

transition: stateExpression[left] END_LEFT_TRANSITION symbolExpression[middle] BEGIN_LEFT_TRANSITION stateExpression[right]		{ $$ = LeftTransitionSemanticAction($left, $right, $middle); }
	| stateExpression[left] BEGIN_RIGHT_TRANSITION symbolExpression[middle] END_RIGHT_TRANSITION stateExpression[right]			{ $$ = RightTransitionSemanticAction($left, $right, $middle); }
	| stateExpression[left] END_LEFT_TRANSITION symbolExpression[middle] END_RIGHT_TRANSITION stateExpression[right]				{ $$ = BothSideTransitionSemanticAction($left, $right, $middle); }
	;

symbolSet: /*OPEN_BRACE symbolSet OPEN_BRACE							{ $$ = $2; } */
	symbolExpression[left] COMMA symbolExpression[right]						{ $$ = SymbolSetSemanticAction($left, $right); }
	| symbolExpression												{ $$ = ArithmeticSymbolSetSemanticAction($1); }
/*	| symbol 														{ $$ = SingularSymbolSetSemanticAction($1); }
	| EMPTY															{ $$ = EmptySymbolSetSemanticAction(); } */
	;

symbol: SYMBOL 														{ $$ = SymbolSemanticAction($1); }
	| LAMBDA 														{ $$ = LambdaSemanticAction(); }
	;

%%
