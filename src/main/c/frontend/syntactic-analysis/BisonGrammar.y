%{

#include "BisonActions.h"

%}

%define api.value.union.name SemanticValue

%union {
	/** Terminals. */

	int integer;
	Token token;

	/** Non-terminals. */

	Constant * constant;
	Expression * expression;
	Factor * factor;
	Program * program;
	Symbol * symbol;
	Transition * transition;
	State * state;
	SymbolSet * symbolSet;
	StateSet * stateSet;
	TransitionSet * transitionSet;
	Automata * automata;
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
%token <token> REGULAR_STATE
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

/** Non-terminals. */
%type <constant> constant
%type <expression> expression
%type <factor> factor
%type <program> program
%type <symbol> symbol
%type <state> state
%type <transition> transition
%type <symbolSet> symbolSet
%type <stateSet> stateSet
%type <transitionSet> transitionSet
%type <automata> automata
%type <definition> definition
%type <set> set
%type <setOperations> setOperations
%type <set_type> set_type
%type <automata_type> automata_type
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

program: definition													{ $$ = ExpressionProgramSemanticAction(currentCompilerState(), $1); }
	/* EXPRESSION */
	;

definition: automata_type IDENTIFIER OPEN_BRACKET automata CLOSE_BRACKET 	{ $$ = AutomataDefinitionSemanticAction($1, $2, $4); }
	/*| DEF name function*/
	| set_type IDENTIFIER COLON expression
	;
	
automata: stateSet COMMA transitionSet COMMA symbolSet;						{ $$ = AutomataSemanticAction($1, $2, $4); }
 
// todo: expresiones separadas por State, Transition y alphabet
expression: OPEN_PARENTHESIS expression[left] UNION expression[right] CLOSE_PARENTHESIS			{ $$ = ArithmeticExpressionSemanticAction($left, $right, ADDITION); }
	| OPEN_PARENTHESIS expression[left] DIFFERENCE expression[right] CLOSE_PARENTHESIS			{ $$ = ArithmeticExpressionSemanticAction($left, $right, DIVISION); }
	| OPEN_PARENTHESIS expression[left] INTERSECTION expression[right] CLOSE_PARENTHESIS		{ $$ = ArithmeticExpressionSemanticAction($left, $right, MULTIPLICATION); }
	| set																						{ $$ = ConstantFactorSemanticAction($1); }
	;

set_type: TRANSITIONS_KEYWORD										{ $$ = SetTypeAction($1) }
	| STATES_KEYWORD												{ $$ = SetTypeAction($1) }
	| ALPHABET_KEYWORD												{ $$ = SetTypeAction($1) }
	;

automata_type: DFA													{ $$ = AutomataTypeAction($1) }
	| NFA															{ $$ = AutomataTypeAction($1) }
	| LNFA;															{ $$ = AutomataTypeAction($1) }
/* function:  OPEN_BRACKET block CLOSE_BRACKET; */												

set: OPEN_BRACE stateSet CLOSE_BRACE								{ $$ = StateSetAction($1) }
	| OPEN_BRACE transitionSet CLOSE_BRACE							{ $$ = TransitionSetAction($1) }
	| OPEN_BRACE symbolSet CLOSE_BRACE								{ $$ = SymbolSetAction($1) }
	| EMPTY															{ $$ = EmptySetAction($1) }
	;

stateSet: OPEN_BRACE stateSet CLOSE_BRACE 							{ $$ = StateSetSemanticAction($2) }
	| stateSet COMMA stateSet
	|  state 														{ $$ = SingularStateSetSemanticAction($1) }
	;

transitionSet: OPEN_BRACE transitionSet CLOSE_BRACE 				{ $$ = TransitionSetSemanticAction($2); }
	| transitionSet COMMA transitionSet
	| transition													{ $$ = SingularTransitionSetSemanticAction($1); }
	;

transition: stateSet[left] END_LEFT_TRANSITION symbolSet[middle] BEGIN_LEFT_TRANSITION stateSet[right]		{ $$ = LeftTransitionSemanticAction($left, $right, $middle); }
	| stateSet[left] BEGIN_RIGHT_TRANSITION symbolSet[middle] END_RIGHT_TRANSITION stateSet[right]			{ $$ = RightTransitionSemanticAction($left, $right, $middle); }
	| stateSet[left] END_LEFT_TRANSITION symbolSet[middle] END_RIGHT_TRANSITION stateSet[right]				{ $$ = BothSideTransitionSemanticAction($left, $right, $middle); }
	;

symbolSet: OPEN_BRACE symbolSet OPEN_BRACE							{ $$ = SymbolSetSemanticAction($2); }
	| symbolSet COMMA symbolSet	
	| symbol 														{ $$ = SingularSymbolSetSemanticAction($1); }
	;

symbol: SYMBOL 														{ $$ = SymbolSemanticAction($1); }
	| LAMBDA 														{ $$ = LambdaSemanticAction($1); }
	;


/*
expression: expression[left] ADD expression[right]					{ $$ = ArithmeticExpressionSemanticAction($left, $right, ADDITION); }
	| expression[left] DIV expression[right]						{ $$ = ArithmeticExpressionSemanticAction($left, $right, DIVISION); }
	| expression[left] MUL expression[right]						{ $$ = ArithmeticExpressionSemanticAction($left, $right, MULTIPLICATION); }
	| expression[left] SUB expression[right]						{ $$ = ArithmeticExpressionSemanticAction($left, $right, SUBTRACTION); }
	| set														{ $$ = FactorExpressionSemanticAction($1); }
	;

factor: OPEN_PARENTHESIS expression CLOSE_PARENTHESIS				{ $$ = ExpressionFactorSemanticAction($2); }
	| constant														{ $$ = ConstantFactorSemanticAction($1); }
	;

constant: INTEGER													{ $$ = IntegerConstantSemanticAction($1); }
	;
		*/

%%
