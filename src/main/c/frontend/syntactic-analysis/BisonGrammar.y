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
//acá serían de tipo state (tienen un valor asociado)
%token <token> FINAL_STATE
%token <token> INITIAL_STATE
//identifier tendría un string
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
%left DIFFERENCE INTERSECTION UNION

%%

 
program: definition						{ $$ = ExpressionProgramSemanticAction(currentCompilerState(), $1); }
/* definitionSet																		{ $$ = ExpressionProgramSemanticAction(currentCompilerState(), $1); }*/
	/* EXPRESSION */
	;
/* PONER DE VUELTA
definitionSet: definition[left] NEW_LINE definitionSet[right]								{ $$ = DefinitionSetSemanticAction($left, $right); }
	| definition																			{ $$ = SingularDefinitionSetSemanticAction($1); }
	;
*/
definition: automataType[type] IDENTIFIER[identifier] OPEN_BRACKET automata[element] CLOSE_BRACKET 								{ $$ = AutomataDefinitionSemanticAction($type, $identifier, $element); }
	/*| DEF name function*/
	| STATES_KEYWORD IDENTIFIER[identifier] COLON OPEN_BRACE stateSet[set] CLOSE_BRACE  										{ $$ = StateSetDefinitionSemanticAction($identifier, $set); }	
	| STATES_KEYWORD IDENTIFIER[identifier] COLON state[element]																{ $$ = SingularStateSetDefinitionSemanticAction($identifier, $element); }
	| ALPHABET_KEYWORD IDENTIFIER[identifier] COLON symbolSet[set] 										{ $$ = SymbolSetDefinitionSemanticAction($identifier, $set); }
	| ALPHABET_KEYWORD IDENTIFIER[identifier] COLON symbol[element] 															{ $$ = SingularSymbolSetDefinitionSemanticAction($identifier, $element); }
	| TRANSITIONS_KEYWORD IDENTIFIER[identifier] COLON transitionSet[set] { $$ = TransitionSetDefinitionSemanticAction($identifier, $set); }
	| TRANSITIONS_KEYWORD IDENTIFIER[identifier] COLON transition[element]														{ $$ = SingularTransitionSetDefinitionSemanticAction($identifier, $element); }
	;		
	
automata: STATES_KEYWORD COLON stateExpression[states] COMMA ALPHABET_KEYWORD COLON symbolExpression[symbols] COMMA TRANSITIONS_KEYWORD COLON transitionExpression[transitions]						{ $$ = AutomataSemanticAction($states, $symbols, $transitions); }
	;

automataType: DFA													{ $$ = DFA_AUTOMATA; }
	| NFA															{ $$ = NFA_AUTOMATA; }
	| LNFA															{ $$ = LNFA_AUTOMATA; }
	;

transitionExpression: OPEN_PARENTHESIS transitionExpression[left] UNION transitionExpression[right] CLOSE_PARENTHESIS									{ $$ = TransitionExpressionSemanticAction($left, $right, UNION); }
	| OPEN_PARENTHESIS transitionExpression[left] DIFFERENCE transitionExpression[right] CLOSE_PARENTHESIS												{ $$ = TransitionExpressionSemanticAction($left, $right, DIFFERENCE); }
	| OPEN_PARENTHESIS transitionExpression[left] INTERSECTION transitionExpression[right] CLOSE_PARENTHESIS											{ $$ = TransitionExpressionSemanticAction($left, $right, INTERSECTION); }																												
	| PIPE stateExpression[left] PIPE END_LEFT_TRANSITION symbolExpression[middle] END_RIGHT_TRANSITION PIPE stateExpression[right] PIPE				{ $$ = BothSideTransitionSemanticAction($left, $right, $middle); }
	| transitionSet 																																	{ $$ = SetTransitionExpressionSemanticAction($1); }	
	| transition 																																		{ $$ = SingularTransitionExpressionSemanticAction($1); }	
	| EMPTY 																																			{ $$ = EmptySetTransitionExpressionSemanticAction() ;}
	| IDENTIFIER																																		{ $$ = IdentifierTransitionExpressionSemanticAction($1); }
	;

stateExpression: OPEN_PARENTHESIS stateExpression[left] UNION stateExpression[right] CLOSE_PARENTHESIS							{ $$ = StateExpressionSemanticAction($left, $right, UNION); }
	| OPEN_PARENTHESIS stateExpression[left] DIFFERENCE stateExpression[right] CLOSE_PARENTHESIS								{ $$ = StateExpressionSemanticAction($left, $right, DIFFERENCE); }
	| OPEN_PARENTHESIS stateExpression[left] INTERSECTION stateExpression[right] CLOSE_PARENTHESIS								{ $$ = StateExpressionSemanticAction($left, $right, INTERSECTION); }
	| stateSet																										 { $$ = SetStateExpressionSemanticAction($1); }
	| IDENTIFIER[identifier] PERIOD stateType[typeSet]																			{ $$ = StateTypeSetSemanticAction($identifier, $typeSet); }	
	| state																														{ $$ = SingularStateExpressionSemanticAction($1); }	
	| EMPTY 																													{ $$ = EmptySetStateExpressionSemanticAction() ;}	
	| IDENTIFIER																												{ $$ = IdentifierStateExpressionSemanticAction($1); }
	;

symbolExpression: OPEN_PARENTHESIS symbolExpression[left] UNION symbolExpression[right] CLOSE_PARENTHESIS						{ $$ = SymbolExpressionSemanticAction($left, $right, UNION); }
	| OPEN_PARENTHESIS symbolExpression[left] DIFFERENCE symbolExpression[right] CLOSE_PARENTHESIS								{ $$ = SymbolExpressionSemanticAction($left, $right, DIFFERENCE); }
	| OPEN_PARENTHESIS symbolExpression[left] INTERSECTION symbolExpression[right] CLOSE_PARENTHESIS							{ $$ = SymbolExpressionSemanticAction($left, $right, INTERSECTION); }
	| symbolSet 																												{ $$ = SetSymbolExpressionSemanticAction($1); }
	| symbol																													{ $$ = SingularSymbolExpressionSemanticAction($1); }
	| EMPTY																														{ $$ = EmptySetSymbolExpressionSemanticAction() ;}	
	| IDENTIFIER																												{ $$ = IdentifierSymbolExpressionSemanticAction($1); }
	;
	
stateNode: stateExpression					//	{ $$ = CreateStateNodeFromExpression($1);}
		| stateExpression COMMA stateNode		
		;

stateSet: OPEN_BRACE stateNode CLOSE_BRACE								{ $$ = SingularNodeStateSetSemanticAction($1); }
	;

/*
stateSet: /*OPEN_BRACE stateSet CLOSE_BRACE 							{ $$ = $2 } /
	stateExpression[left] COMMA stateExpression[right]						{ $$ = StateExpressionsSemanticAction($left, $right); }
	| stateExpression															{ $$ = SingularExpressionStateSetSemanticAction($1); }
/*	| state 														{ $$ = SingularStateSetSemanticAction($1) }
	| EMPTY															{ $$ = EmptyStateSetSemanticAction();}	/
	;
*/
state: symbol														{ $$ = StateSemanticAction(false, false, $1); }
	| FINAL_STATE symbol											{ $$ = StateSemanticAction(false, true, $2); }
	| INITIAL_STATE	symbol											{ $$ = StateSemanticAction(true, false, $2); }
	| INITIAL_STATE FINAL_STATE symbol								{ $$ = StateSemanticAction(true, true, $3); }
	| FINAL_STATE INITIAL_STATE symbol								{ $$ = StateSemanticAction(true, true, $3); }
	;

stateType: REGULAR_STATES_KEYWORD[regularStates]								{ $$ = REGULAR; }
	| INITIAL_STATES_KEYWORD[initialStates]										{ $$ = INITIAL;}
	| FINAL_STATES_KEYWORD[finalStates]											{ $$ = FINAL;}
	;

transitionNode: transitionExpression
	| transitionExpression COMMA transitionNode
	;

transitionSet: OPEN_BRACE transitionNode CLOSE_BRACE										{ $$ = SingularNodeTransitionSetSemanticAction()}
			;

/*transitionSet: /*OPEN_BRACE transitionSet CLOSE_BRACE 				{ $$ = $2; }
	transitionExpression[left] COMMA transitionExpression[right]				{ $$ = TransitionExpressionsSemanticAction($left, $right); }	
	| transitionExpression														{ $$ = SingularExpressionTransitionSetSemanticAction($1); }*/
/*	| transition													{ $$ = SingularTransitionSetSemanticAction($1); }
	| EMPTY															{ $$ = EmptyTransitionSetSemanticAction(); } */
	;

//lo pasaría a Transition Expression directamente porque con esto devolverían un set (aunque tenga un solo elemento) adentro de una expression (porque no devolvería una transition necesariamente y es preferible que sea uniforme lo que devuelva)
transition: PIPE stateExpression[left] END_LEFT_TRANSITION symbolExpression[middle] BEGIN_LEFT_TRANSITION PIPE stateExpression[right] PIPE	{ $$ = LeftTransitionSemanticAction($left, $right, $middle); }
	| PIPE stateExpression[left] BEGIN_RIGHT_TRANSITION symbolExpression[middle] END_RIGHT_TRANSITION PIPE stateExpression[right] PIPE		{ $$ = RightTransitionSemanticAction($left, $right, $middle); }
	;

symbolNode: symbolExpression
		| symbolExpression COMMA symbolNode
			;

symbolSet: //symbolNode COMMA symbolSet 							{ $$ = SymbolExpressionsSemanticAction($1, $3) }
	OPEN_BRACE symbolNode CLOSE_BRACE												//{ $$ =  }
	;

/*symbolSet: OPEN_BRACE symbolSet OPEN_BRACE							{ $$ = $2; }
	symbolExpression[left] COMMA symbolExpression[right]						{ $$ = SymbolExpressionsSemanticAction($left, $right); }
	| symbolExpression												{ $$ = SingularExpressionSymbolSetSemanticAction($1); }*/
/*	| symbol 														{ $$ = SingularSymbolSetSemanticAction($1); }
	| EMPTY															{ $$ = EmptySymbolSetSemanticAction(); } */
	;

symbol: SYMBOL 														{ $$ = SymbolSemanticAction($1); }
	| LAMBDA 														{ $$ = LambdaSemanticAction($1); }
	;

%%
