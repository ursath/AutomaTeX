#include "backend/code-generation/Generator.h"
#include "backend/domain-specific/Automatex.h"
#include "frontend/lexical-analysis/FlexActions.h"
#include "frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "frontend/syntactic-analysis/BisonActions.h"
#include "frontend/syntactic-analysis/SyntacticAnalyzer.h"
#include "shared/CompilerState.h"
#include "shared/Environment.h"
#include "shared/Logger.h"
#include "shared/String.h"

/**
 * The main entry-point of the entire application.
 */
const int main(const int count, const char ** arguments) {
	Logger * logger = createLogger("EntryPoint");
	initializeFlexActionsModule();
	initializeBisonActionsModule();
	initializeSyntacticAnalyzerModule();
	initializeAbstractSyntaxTreeModule();
	initializeAutomatexModule();
	initializeGeneratorModule();
	
	// Logs the arguments of the application.
	for (int k = 0; k < count; ++k) {
	logDebugging(logger, "Argument %d: \"%s\"", k, arguments[k]);
	}
	
	// Begin compilation process.
	CompilerState compilerState = {
	.abstractSyntaxtTree = NULL,
	.succeed = false,
	.value = 0
	};
	const SyntacticAnalysisStatus syntacticAnalysisStatus = parse(&compilerState);
	CompilationStatus compilationStatus = SUCCEED;
	if (syntacticAnalysisStatus == ACCEPT) {
	logDebugging(logger, "Computing expression value...");
	Program * program = compilerState.abstractSyntaxtTree;
	ComputationResult computationResult = computeDefinitionSet(program->definitionSet);
	if (computationResult.succeed) {
	//compilerState.value = computationResult.value;
	generate(&compilerState);
	}
	else {
	logError(logger, "The computation phase rejects the input program.");
	compilationStatus = FAILED;
	}
	logDebugging(logger, "Releasing AST resources...");
	// todo
	//releaseProgram(program);
	}
	else {
	logError(logger, "The syntactic-analysis phase rejects the input program.");
	compilationStatus = FAILED;
	}
	
	logDebugging(logger, "Releasing modules resources...");
	shutdownGeneratorModule();
	shutdownAutomatexModule();
	shutdownAbstractSyntaxTreeModule();
	shutdownSyntacticAnalyzerModule();
	shutdownBisonActionsModule();
	shutdownFlexActionsModule();
	logDebugging(logger, "Compilation is done.");
	destroyLogger(logger);
	return compilationStatus;
}

//#include "backend/code-generation/Generator.h"
//#include "backend/domain-specific/Calculator.h"
//#include "frontend/lexical-analysis/FlexActions.h"
//#include "frontend/syntactic-analysis/AbstractSyntaxTree.h"
//#include "frontend/syntactic-analysis/BisonActions.h"
//#include "frontend/syntactic-analysis/SyntacticAnalyzer.h"
//#include "shared/CompilerState.h"
//#include "shared/Environment.h"
//#include "shared/Logger.h"
//#include "shared/String.h"
//
///**
// * The main entry-point of the entire application.
// */
//const int main(const int count, const char ** arguments) {
//	Logger * logger = createLogger("EntryPoint");
//	initializeFlexActionsModule();
//	initializeBisonActionsModule();
//	initializeSyntacticAnalyzerModule();
//	initializeAbstractSyntaxTreeModule();
//	initializeCalculatorModule();
//	initializeGeneratorModule();
//
//	// Logs the arguments of the application.
//	for (int k = 0; k < count; ++k) {
//		logDebugging(logger, "Argument %d: \"%s\"", k, arguments[k]);
//	}
//
//	// Begin compilation process.
//	CompilerState compilerState = {
//		.abstractSyntaxtTree = NULL,
//		.succeed = false,
//		.value = 0
//	};
//	const SyntacticAnalysisStatus syntacticAnalysisStatus = parse(&compilerState);
//	CompilationStatus compilationStatus = SUCCEED;
//	if (syntacticAnalysisStatus == ACCEPT) {
//		logDebugging(logger, "Computing expression value...");
//		Program * program = compilerState.abstractSyntaxtTree;
//		ComputationResult computationResult = computeExpression(program->definitionSet);
//		if (computationResult.succeed) {
//			compilerState.value = computationResult.value;
//			generate(&compilerState);
//		}
//		else {
//			logError(logger, "The computation phase rejects the input program.");
//			compilationStatus = FAILED;
//		}
//		logDebugging(logger, "Releasing AST resources...");
//		releaseProgram(program);
//	}
//	else {
//		logError(logger, "The syntactic-analysis phase rejects the input program.");
//		compilationStatus = FAILED;
//	}
//
//	logDebugging(logger, "Releasing modules resources...");
//	shutdownGeneratorModule();
//	shutdownCalculatorModule();
//	shutdownAbstractSyntaxTreeModule();
//	shutdownSyntacticAnalyzerModule();
//	shutdownBisonActionsModule();
//	shutdownFlexActionsModule();
//	logDebugging(logger, "Compilation is done.");
//	destroyLogger(logger);
//	return compilationStatus;
//}
//