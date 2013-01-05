// Calculator.cpp

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "CalcLib.h"

using namespace CalcLib;

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS0( Calculator );

//=========================================================================================
Calculator::Calculator( const char* envString )
{
	environment = NewEnvironment( envString );
}

//=========================================================================================
Calculator::Calculator( Environment* environment )
{
	this->environment = environment;
}

//=========================================================================================
/*virtual*/ Calculator::~Calculator( void )
{
	delete environment;
}

//=========================================================================================
Environment* Calculator::GetEnvironment( void )
{
	return environment;
}

//=========================================================================================
void Calculator::SetEnvironment( Environment* environment )
{
	if( this->environment )
		delete this->environment;
	this->environment = environment;
}

//=========================================================================================
bool Calculator::IsPartialMathExpression( const char* mathExpression, bool& isPartialMathExpression )
{
	bool success = false;
	Utilities::List listOfTokens;

	do
	{
		if( !environment )
			break;

		environment->WipeAllErrors();

		if( !tokenizer.Tokenize( mathExpression, listOfTokens, *environment ) )
			break;

		if( !parser.IsPartialStatement( listOfTokens, isPartialMathExpression, *environment ) )
			break;

		success = true;
	}
	while( false );

	listOfTokens.RemoveAll( true );

	return success;
}

//=========================================================================================
Evaluator* Calculator::CompileEvaluator( const char* mathExpression )
{
	bool success = false;
	Evaluator* evaluator = 0;
	Utilities::List listOfTokens;

	do
	{
		// Tokenize the given math expression into a list of tokens.
		if( !tokenizer.Tokenize( mathExpression, listOfTokens, *environment ) )
			break;

		// Parse the token list into an evaluation tree.
		if( !parser.Parse( listOfTokens, evaluator, *environment ) )
			break;

		// We made it through the gauntlet!
		success = true;
	}
	while( false );

	// Clean up.
	listOfTokens.RemoveAll( true );
	if( !success && evaluator )
	{
		delete evaluator;
		evaluator = 0;
	}

	return evaluator;
}

//=========================================================================================
bool Calculator::Calculate( const char* mathExpression, char* resultBuffer, int resultBufferSize, bool printLatex /*= false*/ )
{
	bool success = false;
	Evaluator* evaluator = 0;
	Number* result = 0;

	do
	{
		// We can't do anything unless there's an environment to do it in.
		if( !environment )
			break;

		// Make sure we can at least poke a string terminator in there.
		if( resultBufferSize <= 0 )
			break;
		resultBuffer[0] = '\0';

		// Begin with a clean slate.
		environment->WipeAllErrors();

		// Compile the given math expression into an evaluation tree.
		evaluator = CompileEvaluator( mathExpression );
		if( !evaluator )
			break;

		// Call upon the environment to allocate the type of number that will store our result.
		result = environment->CreateNumber( evaluator );
		if( !result )
			break;

		// Evaluate the evaluation tree into the given result in the context of this calculator's environment.
		if( !evaluator->EvaluateResult( *result, *environment ) )
			break;

		// Print the result for the caller.
		if( !result->Print( resultBuffer, resultBufferSize, printLatex, *environment ) )
			break;
		
		// Special case: If the result is a string by the name of a desired environment, switch to that environment.
		Environment* switchEnvironment = NewEnvironment( result );
		if( switchEnvironment )
		{
			delete environment;
			environment = switchEnvironment;
			strcpy_s( resultBuffer, resultBufferSize, "The environment has been changed!  Use the \"env_info\" function for details." );
		}

		// We made it through the gauntlet!
		success = true;
	}
	while( false );

	// Clean up.
	if( evaluator )
		delete evaluator;
	if( result )
		delete result;

	// Indicate success or failure.
	return success;
}

//=========================================================================================
/*static*/ Environment* Calculator::NewEnvironment( const Number* result )
{
	if( result->IsTypeOf( StringNumber::ClassName() ) )
	{
		StringNumber* stringNumber = ( StringNumber* )result;
		return NewEnvironment( stringNumber->String() );
	}
	
	return 0;
}

//=========================================================================================
/*static*/ Environment* Calculator::NewEnvironment( const char* envString )
{
	Environment* environment = 0;
	if( 0 == strcmp( envString, "float64" ) )
		environment = new FloatingPointMathEnvironment();
	else if( 0 == strcmp( envString, "apa" ) )
		environment = new ArbitraryPrecisionEnvironment();
	else if( 0 == strcmp( envString, "cplx" ) )
		environment = new ComplexNumberFieldEnvironment();
	else if( 0 == strcmp( envString, "geoalg" ) )
		environment = new GeometricAlgebraEnvironment();
	else if( 0 == strcmp( envString, "fractions" ) )
		environment = new FractionEnvironment();
	else if( 0 == strcmp( envString, "linalg" ) )
		environment = new LinearAlgebraEnvironment();
	return environment;
}

//=========================================================================================
void Calculator::PrintErrorReport( void )
{
	if( environment )
	{
		environment->Print( "\n%d error(s) accumulated.\n\n", environment->ErrorCount() );
		environment->PrintAllErrors();
		environment->Print( "\n" );
	}
	else
	{
		environment->Print( "\nThere is no environment in which to calculate!\n\n" );
	}
}

// Calculator.cpp