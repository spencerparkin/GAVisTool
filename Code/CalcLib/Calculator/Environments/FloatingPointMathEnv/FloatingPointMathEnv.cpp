// FloatingPointMathEnv.cpp

#include "../../CalcLib.h"

using namespace CalcLib;

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( FloatingPointMathEnvironment, Environment );

//=========================================================================================
FloatingPointMathEnvironment::FloatingPointMathEnvironment( void )
{
}

//=========================================================================================
/*virtual*/ FloatingPointMathEnvironment::~FloatingPointMathEnvironment( void )
{
}

//=========================================================================================
/*virtual*/ void FloatingPointMathEnvironment::PrintEnvironmentInfo( void )
{
	Print
	(
		"\n"
		"You are in the 64-bit floating point math environment.\n"
		"\n"
		"Available functions are:\n"
		"\n"
		"\tsqrt(x)\n"
		"\texp(x)\n"
		"\tlog(x,y)\n"
		"\tln(x)\n"
		"\tpow(x,y)\n"
		"\tsin(x)\n"
		"\tcos(x)\n"
		"\ttan(x)\n"
		"\tasin(x)\n"
		"\tacos(x)\n"
		"\tatan(x)\n"
		"\tatan2(y,x)\n"
		"\tcsc(x)\n"
		"\tsec(x)\n"
		"\tcot(x)\n"
		"\tacsc(x)\n"
		"\tasec(x)\n"
		"\tacot(x)\n"
		"\n"
	);

	Environment::PrintEnvironmentInfo();
}

//=========================================================================================
/*virtual*/ Number* FloatingPointMathEnvironment::CreateNumber( void )
{
	FloatingPointNumber* floatingPointNumber = new FloatingPointNumber();
	floatingPointNumber->AssignAdditiveIdentity( *this );
	Number* number = floatingPointNumber;
	return number;
}

//=========================================================================================
/*virtual*/ FunctionEvaluator* FloatingPointMathEnvironment::CreateFunction( const char* functionName )
{
	FunctionEvaluator* functionEvaluator = Environment::CreateFunction( functionName );
	if( functionEvaluator )
		return functionEvaluator;

	FloatingPointMathFunctionEvaluator::FunctionType function = FloatingPointMathFunctionEvaluator::FUNC_UNKNOWN;

	if( 0 == strcmp( functionName, "sqrt" ) )
		function = FloatingPointMathFunctionEvaluator::FUNC_SQRT;
	else if( 0 == strcmp( functionName, "exp" ) )
		function = FloatingPointMathFunctionEvaluator::FUNC_EXP;
	else if( 0 == strcmp( functionName, "log" ) )
		function = FloatingPointMathFunctionEvaluator::FUNC_LOG;
	else if( 0 == strcmp( functionName, "ln" ) )
		function = FloatingPointMathFunctionEvaluator::FUNC_LN;
	else if( 0 == strcmp( functionName, "pow" ) )
		function = FloatingPointMathFunctionEvaluator::FUNC_POW;
	else if( 0 == strcmp( functionName, "sin" ) )
		function = FloatingPointMathFunctionEvaluator::FUNC_SIN;
	else if( 0 == strcmp( functionName, "cos" ) )
		function = FloatingPointMathFunctionEvaluator::FUNC_COS;
	else if( 0 == strcmp( functionName, "tan" ) )
		function = FloatingPointMathFunctionEvaluator::FUNC_TAN;
	else if( 0 == strcmp( functionName, "asin" ) )
		function = FloatingPointMathFunctionEvaluator::FUNC_ASIN;
	else if( 0 == strcmp( functionName, "acos" ) )
		function = FloatingPointMathFunctionEvaluator::FUNC_ACOS;
	else if( 0 == strcmp( functionName, "atan" ) )
		function = FloatingPointMathFunctionEvaluator::FUNC_ATAN;
	else if( 0 == strcmp( functionName, "atan2" ) )
		function = FloatingPointMathFunctionEvaluator::FUNC_ATAN2;
	else if( 0 == strcmp( functionName, "csc" ) )
		function = FloatingPointMathFunctionEvaluator::FUNC_CSC;
	else if( 0 == strcmp( functionName, "sec" ) )
		function = FloatingPointMathFunctionEvaluator::FUNC_SEC;
	else if( 0 == strcmp( functionName, "cot" ) )
		function = FloatingPointMathFunctionEvaluator::FUNC_COT;
	else if( 0 == strcmp( functionName, "acsc" ) )
		function = FloatingPointMathFunctionEvaluator::FUNC_ACSC;
	else if( 0 == strcmp( functionName, "asec" ) )
		function = FloatingPointMathFunctionEvaluator::FUNC_ASEC;
	else if( 0 == strcmp( functionName, "acot" ) )
		function = FloatingPointMathFunctionEvaluator::FUNC_ACOT;

	if( function != FloatingPointMathFunctionEvaluator::FUNC_UNKNOWN )
	{	
		FloatingPointMathFunctionEvaluator* mathFunctionEvaluator = new FloatingPointMathFunctionEvaluator();
		mathFunctionEvaluator->Function( function );
		return mathFunctionEvaluator;
	}

	return 0;
}

//=========================================================================================
/*virtual*/ Evaluator* FloatingPointMathEnvironment::CreateBinaryOperator( const char* operatorName, Evaluator* leftOperand, Evaluator* rightOperand, bool& isBinaryOperationEvaluator )
{
	if( 0 == strcmp( operatorName, "^" ) )
	{
		FunctionArgumentEvaluator* leftArgument = new FunctionArgumentEvaluator();
		leftArgument->Argument( leftOperand );

		FunctionArgumentEvaluator* rightArgument = new FunctionArgumentEvaluator();
		rightArgument->Argument( rightOperand );

		FloatingPointMathFunctionEvaluator* mathFunctionEvaluator = new FloatingPointMathFunctionEvaluator();
		mathFunctionEvaluator->Function( FloatingPointMathFunctionEvaluator::FUNC_POW );
		mathFunctionEvaluator->AddArgument( leftArgument, FloatingPointMathFunctionEvaluator::APPEND_TO_LIST_OF_ARGS );
		mathFunctionEvaluator->AddArgument( rightArgument, FloatingPointMathFunctionEvaluator::APPEND_TO_LIST_OF_ARGS );

		isBinaryOperationEvaluator = false;
		return mathFunctionEvaluator;
	}

	return 0;
}

//=========================================================================================
/*virtual*/ Evaluator* FloatingPointMathEnvironment::CreateVariable( const char* variableName )
{
	double value = 0.0;

	if( 0 == strcmp( variableName, "pi" ) )
		value = 3.1415926536;

	if( value == 0.0 )
		return 0;

	FloatingPointNumber* constant = new FloatingPointNumber();
	constant->value = value;
	ConstantEvaluator* evaluator = new ConstantEvaluator( constant );
	return evaluator;
}

// FloatingPointMathEnv.cpp