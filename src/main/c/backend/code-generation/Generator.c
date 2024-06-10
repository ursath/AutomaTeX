#include "Generator.h"

/* MODULE INTERNAL STATE */

const char _indentationCharacter = ' ';
const char _indentationSize = 4;
static Logger * _logger = NULL;

void initializeGeneratorModule() {
	_logger = createLogger("Generator");
}

void shutdownGeneratorModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
}

/** PRIVATE FUNCTIONS */

static const char _expressionTypeToCharacter(const ExpressionType type);
static void _generateConstant(const unsigned int indentationLevel, Constant * constant);
static void _generateEpilogue(const int value);
static void _generateAutomata(Automata * automata);
static void _generateExpression(const unsigned int indentationLevel, Expression * expression);
static void _generateFactor(const unsigned int indentationLevel, Factor * factor);
static void _generateProgram(Program * program);
static void _generatePrologue(void);
static char * _indentation(const unsigned int indentationLevel);
static void _output(const unsigned int indentationLevel, const char * const format, ...);

/**
 * Converts and expression type to the proper character of the operation
 * involved, or returns '\0' if that's not possible.
 */
static const char _expressionTypeToCharacter(const ExpressionType type) {
	switch (type) {
		case ADDITION: return '+';
		case DIVISION: return '/';
		case MULTIPLICATION: return '*';
		case SUBTRACTION: return '-';
		default:
			logError(_logger, "The specified expression type cannot be converted into character: %d", type);
			return '\0';
	}
}


/**
 * Generates the output of a constant.
 */
static void _generateConstant(const unsigned int indentationLevel, Constant * constant) {
	_output(indentationLevel, "%s", "[ $C$, circle, draw, black!20\n");
	_output(1 + indentationLevel, "%s%d%s", "[ $", constant->value, "$, circle, draw ]\n");
	_output(indentationLevel, "%s", "]\n");
}

/**
 * Generates the output of an expression.
 */


static void _generateExpression(const unsigned int indentationLevel, Expression * expression) {
	_output(indentationLevel, "%s", "[ $E$, circle, draw, black!20\n");
	switch (expression->type) {
		case ADDITION:
		case DIVISION:
		case MULTIPLICATION:
		case SUBTRACTION:
			_generateExpression(1 + indentationLevel, expression->leftExpression);
			_output(1 + indentationLevel, "%s%c%s", "[ $", _expressionTypeToCharacter(expression->type), "$, circle, draw, purple ]\n");
			_generateExpression(1 + indentationLevel, expression->rightExpression);
			break;
		case FACTOR:
			_generateFactor(1 + indentationLevel, expression->factor);
			break;
		default:
			logError(_logger, "The specified expression type is unknown: %d", expression->type);
			break;
	}
	_output(indentationLevel, "%s", "]\n");
}


/**
 * Generates the output of a factor.
 */


static void _generateFactor(const unsigned int indentationLevel, Factor * factor) {
	_output(indentationLevel, "%s", "[ $F$, circle, draw, black!20\n");
	switch (factor->type) {
		case CONSTANT:
			_generateConstant(1 + indentationLevel, factor->constant);
			break;
		case EXPRESSION:
			_output(1 + indentationLevel, "%s", "[ $($, circle, draw, purple ]\n");
			_generateExpression(1 + indentationLevel, factor->expression);
			_output(1 + indentationLevel, "%s", "[ $)$, circle, draw, purple ]\n");
			break;
		default:
			logError(_logger, "The specified factor type is unknown: %d", factor->type);
			break;
	}
	_output(indentationLevel, "%s", "]\n");
}


/**
 * Generates the output of the program.
 */
static void _generateProgram(Program * program) {
	_generateDefinitionSet( program->expression);
}

static void _generateDefinitionSet(DefinitionSet * definitionSet) {
	
	DefinitionNode currentNode = program->definitionSet->first;
	while ( currentNode!= program->definitionSet->tail ){
	
		_generateDefinition(currentNode->definition);
		currentNode = currentNode->next;
	}
	
	if ( program->definitionSet->tail!=NULL )
		_generateDefinition(program->definitionSet->tail->definition);
}

static void _generateDefinition(Definition * definition) {
	
	if (definition->type != AutomataType)  
		return;
	else {
		_generateAutomata(definition->automata);
		_genetateTransitionsTable(definition->automata);
	}
	
}

static void _generateAutomata(Automata * automata) {
	_output(0, "%s", 
		"\\begin{figure}[ht]\n"
        "\\centering\n"
        "\\begin{tikzpicture}[auto]\n"
		"\\node[state,initial,accepting] (0) {$" + automata->initials->state->symbol + "$};"
	)

	StateNode currentStateNode = automata->states->stateSet->first;
	while ( currentStateNode != NULL ){
		switch(currentStateNode->state) {
			case state->isInitial && state->isFinal:
				_output(0, "\\node[state, initial, accepting] (%s) {$%s$};", currentStateNode->state->symbol, currentStateNode->state->symbol);
				break;
			case state->isInitial:
				_output(0, "\\node[state, initial] (%s) {$%s$};", currentStateNode->state->symbol, currentStateNode->state->symbol);
				break;
			case state->isFinal:
				_output(0, "\\node[state, accepting] (%s) [right-of=%s] {$%s$};", currentStateNode->state->symbol, "to-do" , currentStateNode->state->symbol);
				break;
			default:
				_output(0, "\\node[state] (%s) [right-of=%s] {$%s$};", currentStateNode->state->symbol, "to-do" , currentStateNode->state->symbol);
		}
		currentStateNode = currentStateNode->next;
	}
	
	_output(0, "%s", "\\path[->]\n");
	TransitionNode currentTransitionNode = automata->transitions->transitionSet->first;
	while ( currentTransitionNode != NULL ){
		switch(currentTransitionNode->state) {
		}
		currentTransitionNode = currentTransitionNode->next;
	}

	_output(0, "%s",
	 	"\\end{tikzpicture}\n"
        "\\caption{Caption of the FSM}\n"
        "\\label{fig:my_label}\n"
    	"\\end{figure}\n"
	);
}

static void _generateTransitionsTable(Automata * automata) {
	_output(0, "%s", 
		"\\begin{table}[h]\n"
		"\\centering\n"
		"\\begin{tabular}{|c|c|c|}\n"
		"\\hline\n"
		"$\\delta$ & " + + " & " + + " \\\\ \n"
		" \\hline\n"
		"\\end{tabular}\n"
		"\\caption{Automata Transitions Table}\n"
		"\\end{table}\n"
	);
}


/**
 * Creates the prologue of the generated output, a Latex document that renders
 * a tree thanks to the Forest package.
 *
 * @see https:ctan.dcc.uchile.cl/graphics/pgf/contrib/forest/forest-doc.pdf
 */
static void _generatePrologue(void) {
	_output(0, "%s",
		"\\documentclass{article}\n"
		"\\usepackage{tikz}\n"
		"\\usetikzlibrary{automata, positioning, arrows}\n"
		"\\usepackage{caption}\n"
		"\\usepackage{geometry}\n"
		"\\geometry{a4paper, margin=1in}\n"
		"\\begin{document}\n"
		"\\tikzset{ state/.style={ circle, draw, fill={rgb:black,1;white,10}, minimum size=2em, inner sep=0pt, scale=2.5, node distance=2cm } }\n"
	);
}

/**
 * Creates the epilogue of the generated output, that is, the final lines that
 * completes a valid Latex document.
 */
static void _generateEpilogue(const int value) {
	_output(0, "%s%d%s",
		"\\end{document}\n\n"
	);
}

/**
 * Generates an indentation string for the specified level.
 */
static char * _indentation(const unsigned int level) {
	return indentation(_indentationCharacter, level, _indentationSize);
}

/**
 * Outputs a formatted string to standard output.
 */
static void _output(const unsigned int indentationLevel, const char * const format, ...) {
	va_list arguments;
	va_start(arguments, format);
	char * indentation = _indentation(indentationLevel);
	char * effectiveFormat = concatenate(2, indentation, format);
	vfprintf(stdout, effectiveFormat, arguments);
	free(effectiveFormat);
	free(indentation);
	va_end(arguments);
}

/** PUBLIC FUNCTIONS */

void generate(CompilerState * compilerState) {
	logDebugging(_logger, "Generating final output...");
	_generatePrologue();
	_generateProgram(compilerState->abstractSyntaxtTree);
	_generateEpilogue(compilerState->value);
	logDebugging(_logger, "Generation is done.");
}

