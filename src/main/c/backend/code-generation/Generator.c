#include "Generator.h"

/* MODULE INTERNAL STATE */

const char _indentationCharacter = ' ';
const char _indentationSize = 4;
static Logger * _logger = NULL;

int automataCount = 0;

TransitionMatrixCell ** transitionMatrix;
AutomataMatrixCell ** automataMatrix;

void initializeGeneratorModule() {
	_logger = createLogger("Generator");
}

void shutdownGeneratorModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
}

/** PRIVATE FUNCTIONS */

static void _generateEpilogue(const int value);
static void _generateDefinitionSet(DefinitionSet * definitionSet);
static boolean _generateDefinition(Definition * definition);
static void _generateAutomataAndTable(Automata * automata);
static void _generateAutomata(Automata * automata, State * states[], Symbol * symbols[], int statesCount, int symbolsCount);
static void _generateTransitionsTable(State * states[], Symbol * symbols[], int statesCount, int symbolsCount, StateSet * finalStates);
static void _generateEmptyPage();
static void _generateProgram(Program * program);
static void _generatePrologue(void);
static char * _indentation(const unsigned int indentationLevel);
static void _output(const unsigned int indentationLevel, const char * const format, ...);

void initTransitionMatrix(int numStates, int numSymbols);
void initAutomataMatrix(int numStates);
void setTransitionMatrix(State * states[], Symbol * symbols[], TransitionSet * transitionSet, int statesCount, int symbolsCount);
void setAutomataMatrix(State * states[], TransitionSet * transitionSet, int statesCount);
int getStateIndex(char * stateValue, State * states[], int statesCount);
int getSymbolIndex(char * symbolValue, Symbol * symbols[], int symbolsCount);
void initializeStates(State * states[], StateSet * stateSet);
void initializeSymbols(Symbol * symbols[], SymbolSet * symbolSet);
int getStatesCount(StateNode * currentNode);
int getAlphabetLength(SymbolNode * currentNode);
boolean isFinalState(State * state, StateSet * finalStates);
boolean stateHasTransition(Symbol stateSymbol, int numStates, int numSymbols, State * states[]);
void freeTransitionMatrix(int numStates, int numSymbols);
void freeAutomataMatrix(int numStates);
void freeStatesAndSymbols(State * states[], int numStates, Symbol * symbols[], int numSymbols);


/**
 * Generates the output of the program.
 */
static void _generateProgram(Program * program) {
	_generateDefinitionSet( program->definitionSet);
}

static void _generateDefinitionSet(DefinitionSet * definitionSet) {	
	DefinitionNode * currentNode = definitionSet->first;
	boolean empty = true;
	while ( currentNode!= NULL ){
		if(_generateDefinition(currentNode->definition)) empty = false;
		currentNode = currentNode->next;
	}
	if(empty) _generateEmptyPage();
}

static boolean _generateDefinition(Definition * definition) {
	if (definition->type == AUTOMATA_DEFINITION) {
		automataCount = ++automataCount;
		_generateAutomataAndTable(definition->automata);
		return true;
	}
	return false;
}

static void _generateAutomataAndTable(Automata * automata) {
	int statesCount = getStatesCount(automata->states->stateSet->first);
	int alphabetLength = getAlphabetLength(automata->alphabet->symbolSet->first);

	State * states[statesCount];
	Symbol * symbols[alphabetLength];

	initializeStates(states, automata->states->stateSet);
	initializeSymbols(symbols, automata->alphabet->symbolSet);

	initTransitionMatrix(statesCount, alphabetLength);
	setTransitionMatrix(states, symbols, automata->transitions->transitionSet, statesCount, alphabetLength);

	initAutomataMatrix(statesCount);
	setAutomataMatrix(states, automata->transitions->transitionSet, statesCount);

	_generateAutomata(automata, states, symbols, statesCount, alphabetLength);
	_generateTransitionsTable(states, symbols, statesCount, alphabetLength, automata->finals->stateSet);

	freeTransitionMatrix(statesCount, alphabetLength);
	freeAutomataMatrix(statesCount);
	freeStatesAndSymbols(states, statesCount, symbols, alphabetLength);
}

static void _generateAutomata(Automata * automata, State * states[], Symbol * symbols[], int statesCount, int symbolsCount) {

	// Apertura del autómata (seteo)
	_output(0, 
		"\\begin{figure} [h!]\n"
		"\\section*{Automata %d: %s}\n"
    	"\\centering\n"
		"\\begin{tikzpicture}\n"
		"\\begin{dot2tex}[neato,mathmode]\n"
        "digraph G {\n"
        "edge [lblstyle=\"auto\"];\n"
		"d2ttikzedgelabels = true;\n"
		"node [shape=circle];\n",
		automataCount, automata->identifier
	);

	// Agrego el estado inicial
	Symbol initialState = automata->initials->state->symbol;
	_output(0, "%s [style=\"initial\"];\n", initialState.value);

	// Agrego todas las transiciones al automata
	for(int i=0; i<statesCount; i++) {
		for(int j=0; j<statesCount; j++) {
			SymbolMatrixNode * currentNode = automataMatrix[i][j].first;
			if(currentNode != NULL) {
				_output(0, "%s -> %s [label=\"", states[i]->symbol.value, states[j]->symbol.value);
				while(currentNode != NULL) {
					if(strcmp(currentNode->symbol->value, "@") == 0) {
						_output(0, "\\lambda");
					} else _output(0, "%s", currentNode->symbol->value);
					if(currentNode->next != NULL) {
						_output(0, ", ");
					}
 					currentNode = currentNode->next;
				}
				_output(0, "\"];\n");
			}
		}
	}

	// Agrego el doble circulo a todos los estados que sean finales
	StateNode * currentFinalNode = automata->finals->stateSet->first;
	while( currentFinalNode != NULL ){
		if(stateHasTransition(currentFinalNode->state->symbol, statesCount, symbolsCount, states)) {
			_output(0, "%s [style=\"accepting\"];\n", currentFinalNode->state->symbol.value);
		}
		currentFinalNode = currentFinalNode->next;
	}

	char * automataType;
	switch (automata->automataType) {
	case DFA_AUTOMATA:
		automataType = "DFA";
		break;
	case NFA_AUTOMATA:
		automataType = "NFA";
		break;
	case LNFA_AUTOMATA:
		automataType = "LNFA";
		break;
	default:
		break;
	}

	// Cierre del autómata
	_output(0,
	 	"}\n"
		"\\end{dot2tex}\n"
		"\\end{tikzpicture}\n"
		"\\caption{%s automata}\n"
    	"\\label{fig:mi_grafo}\n"
		"\\end{figure}\n", 
		automataType
	);
}

static void _generateTransitionsTable(State * states[], Symbol * symbols[], int statesCount, int symbolsCount, StateSet * finalStates) {

	// Apertura de la tabla
	_output(0, "%s", 
		"\\begin{table} [h!]\n"
		"\\centering\n"
		"\\begin{tabular}{|c|"
	);

	// Agrego una columna por cada símbolo del alfabeto
    for(int i=0; i<symbolsCount; i++) {
        _output(0, "c|");
    }

	_output(0, "%s",
        "}\n"
        "\\hline\n"
        "$\\delta$"
	);

	// Agrego todos los símbolos del alfabeto al encabezado de la tabla (un símbolo por cada columna)
	for(int i=0; i<symbolsCount; i++) {
		if(strcmp(symbols[i]->value, "@") == 0) {
			_output(0, " & $\\lambda$");
		} else _output(0, " & %s", symbols[i]->value);
	}

	_output(0, "%s",
		"\\\\\n"
		"\\hline\n"
	);

	// Agrego todas las transiciones a la tabla
	for(int i=0; i<statesCount; i++) {
		if(states[i]->isInitial) _output(0, "%s", "$\\rightarrow$");
		if(states[i]->isFinal) _output(0, "%s", "*");
		_output(0, "%s &", states[i]->symbol.value);
		for(int j=0; j<symbolsCount; j++) {
			MatrixNode * currentNode = transitionMatrix[i][j].first;
			while(currentNode != NULL) {
				if(isFinalState(currentNode->state, finalStates)) {
					_output(0, "*");
				}
				_output(0, "%s", currentNode->state->symbol.value);
				if(currentNode->next != NULL) {
					_output(0, ", ");
				}
				currentNode = currentNode->next;
			}
			if(j != symbolsCount-1){
				_output(0, " & ");
			}
		}
		_output(0, "%s",
			" \\\\\n"
			"\\hline\n"
		);
	}

	// Cierro la tabla
	_output(0, "%s", 
		"\\end{tabular}\n" 
		"\\caption{Transitions Table}\n"
		"\\label{tab:example}\n"
		"\\end{table}\n"
	);
}


void initTransitionMatrix(int numStates, int numSymbols) {
    transitionMatrix = (TransitionMatrixCell **)malloc(numStates * sizeof(TransitionMatrixCell *));
    for (int i = 0; i < numStates; i++) {
        transitionMatrix[i] = (TransitionMatrixCell *)malloc(numSymbols * sizeof(TransitionMatrixCell));
        for (int j = 0; j < numSymbols; j++) {
            transitionMatrix[i][j].first = NULL;
			transitionMatrix[i][j].tail = NULL;
        }
    }
}

void initAutomataMatrix(int numStates) {
    automataMatrix = (AutomataMatrixCell **)malloc(numStates * sizeof(AutomataMatrixCell *));
    for (int i = 0; i < numStates; i++) {
        automataMatrix[i] = (AutomataMatrixCell *)malloc(numStates * sizeof(AutomataMatrixCell));
        for (int j = 0; j < numStates; j++) {
            automataMatrix[i][j].first = NULL;
			automataMatrix[i][j].tail = NULL;
        }
    }
}

void setTransitionMatrix(State * states[], Symbol * symbols[], TransitionSet * transitionSet, int statesCount, int symbolsCount) {
	TransitionNode * currentNode = transitionSet->first;
	int stateIndex = 0;
	int symbolIndex = 0;
	while(currentNode != NULL) {
		
		stateIndex = getStateIndex(currentNode->transition->fromExpression->state->symbol.value, states, statesCount);
		symbolIndex = getSymbolIndex(currentNode->transition->symbolExpression->symbol->value, symbols, symbolsCount);
		MatrixNode * newNode = (MatrixNode *)calloc(1,sizeof(MatrixNode));
		newNode->state=currentNode->transition->toExpression->state;
		if(transitionMatrix[stateIndex][symbolIndex].first == NULL) {
			transitionMatrix[stateIndex][symbolIndex].first = newNode;
		} else {
			transitionMatrix[stateIndex][symbolIndex].tail->next = newNode;
		}
		transitionMatrix[stateIndex][symbolIndex].tail = newNode;
		currentNode = currentNode->next;
	}
}

void setAutomataMatrix(State * states[], TransitionSet * transitionSet, int statesCount) {
	TransitionNode * currentNode = transitionSet->first;
	int fromStateIndex = 0;
	int toStateIndex = 0;
	while(currentNode != NULL) {
		fromStateIndex = getStateIndex(currentNode->transition->fromExpression->state->symbol.value, states, statesCount);
        toStateIndex = getStateIndex(currentNode->transition->toExpression->state->symbol.value, states, statesCount);
		Symbol * symbol = currentNode->transition->symbolExpression->symbol;
        SymbolMatrixNode * newNode = (SymbolMatrixNode *)malloc(sizeof(SymbolMatrixNode));
        newNode->symbol = symbol;
        newNode->next = NULL;
		if (automataMatrix[fromStateIndex][toStateIndex].first == NULL) {
            automataMatrix[fromStateIndex][toStateIndex].first = newNode;
        } else {
            automataMatrix[fromStateIndex][toStateIndex].tail->next = newNode;
        }
        automataMatrix[fromStateIndex][toStateIndex].tail = newNode;
        currentNode = currentNode->next;
	}
}

int getStateIndex(char * stateValue, State * states[], int statesCount) {
	for(int i=0; i<statesCount; i++) {
		if(strcmp(stateValue, states[i]->symbol.value) == 0) {
			return i;
		}
	}
	return -1;
}

int getSymbolIndex(char * symbolValue, Symbol * symbols[], int symbolsCount) {
	for(int i=0; i<symbolsCount; i++) {
		if(strcmp(symbolValue, symbols[i]->value) == 0) {
			return i;
		}
	}
	return -1;
}

void initializeStates(State * states[], StateSet * stateSet) {
    StateNode * currentStateNode = stateSet->first;
	int numStates = 0;
    while (currentStateNode != NULL) {
        states[numStates++] = currentStateNode->state;
        currentStateNode = currentStateNode->next;
    }
}

void initializeSymbols(Symbol * symbols[], SymbolSet * symbolSet) {
    SymbolNode * currentSymbolNode = symbolSet->first;
	int numSymbols = 0;
    while (currentSymbolNode != NULL) {
        symbols[numSymbols++] = currentSymbolNode->symbol;
        currentSymbolNode = currentSymbolNode->next;
    }
}

int getStatesCount(StateNode * currentNode) {
	int count = 0;
	while(currentNode != NULL) {
		count++;
		currentNode = currentNode->next;
	}
	return count;
}

int getAlphabetLength(SymbolNode * currentNode) {
	int count = 0;
	while(currentNode != NULL) {
		count++;
		currentNode = currentNode->next;
	}
	return count;
}

boolean isFinalState(State * state, StateSet * finalStates) {
	StateNode * currentFinalNode = finalStates->first;
	while( currentFinalNode != NULL ){
		if(strcmp(state->symbol.value, currentFinalNode->state->symbol.value) == 0) {
			return true;
		}
		currentFinalNode = currentFinalNode->next;
	}
	return false;
}


boolean stateHasTransition(Symbol stateSymbol, int numStates, int numSymbols, State * states[]) {
	int stateIndex = getStateIndex(stateSymbol.value, states, numStates);

	for (int i = 0; i < numStates; i++) {
        for (int j = 0; j < numSymbols; j++) {
			if(transitionMatrix[i][j].first != NULL) {
				if(i == stateIndex) {
					return true;
				} else {
					MatrixNode * currentNode = transitionMatrix[i][j].first;
					while(currentNode != NULL) {
						if(strcmp(currentNode->state->symbol.value, stateSymbol.value) == 0) return true;
						currentNode = currentNode->next;
					}
				}
			}
        }
    }
	return false;
}

void freeTransitionMatrix(int numStates, int numSymbols) {
    for (int i = 0; i < numStates; i++) {
        for (int j = 0; j < numSymbols; j++) {
            MatrixNode * currentNode = transitionMatrix[i][j].first;
            while (currentNode != NULL) {
                MatrixNode * temp = currentNode;
                currentNode = currentNode->next;
                free(temp);
            }
        }
        free(transitionMatrix[i]);
    }
    free(transitionMatrix);
}

void freeAutomataMatrix(int numStates) {
    for (int i = 0; i < numStates; i++) {
        for (int j = 0; j < numStates; j++) {
            SymbolMatrixNode * currentNode = automataMatrix[i][j].first;
            while (currentNode != NULL) {
                SymbolMatrixNode * temp = currentNode;
                currentNode = currentNode->next;
                free(temp);
            }
        }
        free(automataMatrix[i]);
    }
    free(automataMatrix);
}

void freeStatesAndSymbols(State * states[], int numStates, Symbol * symbols[], int numSymbols) {
    for (int i = 0; i < numStates; i++) {
        free(states[i]);
    }

    for (int i = 0; i < numSymbols; i++) {
        free(symbols[i]);
    }
}


static void _generateEmptyPage() {
	_output(0, "%s",
		"\\vspace*{\\fill}\n"
		"\\begin{center}\n"
   		"\\textit{No automata was defined}\n"
		"\\end{center}\n"
		"\\vspace*{\\fill}\n"
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
		"\\usepackage{caption}\n"
		"\\usepackage{geometry}\n"
		"\\usepackage{dot2texi}\n"
		"\\usetikzlibrary{automata}\n"
		"\\geometry{a4paper, margin=1in}\n"
		"\\begin{document}\n"
	);
}

/**
 * Creates the epilogue of the generated output, that is, the final lines that
 * completes a valid Latex document.
 */
static void _generateEpilogue(const int value) {
	_output(0, "%s",
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

