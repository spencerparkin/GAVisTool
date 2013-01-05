// CalcLib.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

// The down-side to a header like this is that a single
// header change requires an entire library rebuild, but
// it does make header authoring a little easier.  I should
// probably learn how to use a pre-compiled header.

// Include standard headers first.
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

// Include the utilities stuff.
#include "../Utilities/Utilities.h"

// Include the geometric algebra stuff.
#include "../AlgebraLibraries/GeometricAlgebra/GeometricAlgebra.h"

// TODO: Implement ~ unary operator.
// TODO: Implement ! unary operator.
// TODO: Implement ^ binary operator.
// TODO: Implement log() and exp() functions in APA mode.
// TODO: Why does "2a" give "2"?
// TODO: $x - 5*$y doesn't work!

// TODO: A really cool idea for an environment would be one that uses
//       the scalar algebra in a way that generates optimized code for
//       evaluating expressions.  This would require a special algorithm
//       for evaluating the evaluation tree that caches results so
//       calculations that have already been done are not done again needlessly.
//       The caching mechanism would rely on the consistency with which an
//       expression would be printed, which in turn relies on the sorting algorithms
//       for sum of terms and term products.

namespace CalcLib
{
	// Main interface:
	class Calculator;

	// These are part of the RTTI mechanism.  I should really switch
	// over to the C++ RTTI mechanism once I learn how to use it.
	class CalcLibClass;
	class CalcLibClassInfo;

	// Calculator internals:
	class Environment;
	class Tokenizer;
	class Token;
	class Parser;

	// Environments:
	class Environment;

	// Core evaluators:
	class Evaluator;
	class FunctionArgumentEvaluator;
	class VariableEvaluator;
	class ConstantEvaluator;
	class StringEvaluator;

	// Unary operator evaluators:
	class UnaryOperationEvaluator;
	class UnaryStringExecutionEvaluator;
	class UnaryArithmeticOperationEvaluator;

	// Binary operator evaluators:
	class BinaryOperationEvaluator;
	class BinaryArithmeticOperationEvaluator;
	class BinaryAssignmentOperationEvaluator;
	class BinaryComparisonOperationEvaluator;
	class BinaryLogicOperationEvaluator;
	class BinaryIndexingOperationEvaluator;

	// Function evaluators:
	class FunctionEvaluator;
	class EnvironmentInformationFunctionEvaluator;
	class DumpVariablesFunctionEvaluator;
	class DeleteAllVariablesFunctionEvaluator;
	class TaylorSeriesFunctionEvaluator;
	class CalculusFunctionEvaluator;

	// Language evaluators:
	class DoFunctionEvaluator;
	class IfFunctionEvaluator;
	class LoopFunctionEvaluator;

	// Numbers:
	class Number;
	class StringNumber;
}

// Core library stuff:
#include "ClassInfo.h"
#include "Environment.h"
#include "Tokenizer.h"
#include "Parser.h"
#include "Calculator.h"
#include "Number.h"
#include "String.h"
#include "Evaluator.h"
#include "Constant.h"
#include "Variable.h"
#include "Function.h"
#include "Language.h"
#include "Operator.h"
#include "Calculus.h"

// Floating point math:
#include "Environments/FloatingPointMathEnv/FloatingPointMathEnvClasses.h"
#include "Environments/FloatingPointMathEnv/FloatNumber.h"
#include "Environments/FloatingPointMathEnv/FloatingPointMathEnv.h"
#include "Environments/FloatingPointMathEnv/FloatingPointMathFunc.h"

// Arbitrary precision arithmetic:
#include "Environments/ArbitraryPrecisionEnv/ArbitraryPrecisionEnvClasses.h"
#include "Environments/ArbitraryPrecisionEnv/RationalNumber.h"
#include "Environments/ArbitraryPrecisionEnv/ArbitraryPrecisionEnv.h"
#include "Environments/ArbitraryPrecisionEnv/BaseConverterFunc.h"
#include "Environments/ArbitraryPrecisionEnv/DivLimitFunc.h"
#include "Environments/ArbitraryPrecisionEnv/RoundFunc.h"

// The field of complex numbers:
#include "Environments/ComplexNumberFieldEnv/ComplexNumberFieldEnvClasses.h"
#include "Environments/ComplexNumberFieldEnv/ComplexNumber.h"
#include "Environments/ComplexNumberFieldEnv/ComplexNumberFieldEnv.h"

// Elementary school fraction:
#include "Environments/FractionEnv/FractionEnvClasses.h"
#include "Environments/FractionEnv/FractionNumber.h"
#include "Environments/FractionEnv/FractionEnv.h"

// Geometric algebra:
#include "Environments/GeometricAlgebraEnv/GeometricAlgebraEnvClasses.h"
#include "Environments/GeometricAlgebraEnv/MultivectorNumber.h"
#include "Environments/GeometricAlgebraEnv/GeometricAlgebraEnv.h"
#include "Environments/GeometricAlgebraEnv/InverseFunc.h"
#include "Environments/GeometricAlgebraEnv/ReverseFunc.h"
#include "Environments/GeometricAlgebraEnv/BarFunc.h"
#include "Environments/GeometricAlgebraEnv/ScalarPartFunc.h"
#include "Environments/GeometricAlgebraEnv/GradePartFunc.h"
#include "Environments/GeometricAlgebraEnv/MultivectorMathFunc.h"

// Linear algebra:
#include "Environments/LinearAlgebraEnv/LinearAlgebraEnvClasses.h"
#include "Environments/LinearAlgebraEnv/MatrixNumber.h"
#include "Environments/LinearAlgebraEnv/LinearAlgebraEnv.h"
#include "Environments/LinearAlgebraEnv/MatrixFunctions.h"

// CalcLib.h