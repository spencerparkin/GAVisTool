// Environment.cpp

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
IMPLEMENT_CALCLIB_CLASS0( Environment );

//=========================================================================================
Environment::Environment( void )
{
	printBuffer = 0;
	printBufferSize = 0;
}

//=========================================================================================
/*virtual*/ Environment::~Environment( void )
{
	WipeAllErrors();
	DeleteAllVariables();
}

//=========================================================================================
void Environment::PrintBuffer( char* printBuffer, int printBufferSize )
{
	this->printBuffer = printBuffer;
	this->printBufferSize = printBufferSize;

	printBuffer[0] = '\0';
}

//=========================================================================================
bool Environment::Print( const char* formatString, ... )
{
	if( !printBuffer )
		return false;

	char printMessage[ 5*1024 ];

	va_list args;
	va_start( args, formatString );
	vsprintf_s( printMessage, sizeof( printMessage ), formatString, args );
	va_end( args );

	strcat_s( printBuffer, printBufferSize, printMessage );
	return true;
}

//=========================================================================================
const char* Environment::PrintBuffer( void )
{
	return printBuffer;
}

//=========================================================================================
/*virtual*/ void Environment::PrintEnvironmentInfo( void )
{
	Print( "Functions available in every environment:\n\n" );
	Print( "\tenv_info() -- Show environment information.\n" );
	Print( "\tdump_vars() -- Display all currently stored variables.\n" );
	Print( "\n" );
}

//=========================================================================================
Number* Environment::CreateNumber( Evaluator* evaluator )
{
	// We always give the evaluator a chance to create the number
	// into which its result will go first before letting the environment
	// create the types of numbers used in that environment.  This is
	// so that we can interleave other data-types into the language;
	// strings to be specific.  Further, since we're doing this, environments
	// need to always be careful about casting to derived types.  They should
	// do run-time type-checking before any such casts.
	Number* number = 0;
	if( evaluator )
		number = evaluator->CreateNumberForResult( *this );
	if( !number )
		number = CreateNumber();	// Call the environment to create the number.
	return number;
}

//=========================================================================================
// The default function creator should create functions that will not try to
// cast their number inputs to any specific derived type.  Instead, they must use
// the number interface to accomplish their function.
/*virtual*/ FunctionEvaluator* Environment::CreateFunction( const char* functionName )
{
	if( 0 == strcmp( functionName, "env_info" ) )
		return new EnvironmentInformationFunctionEvaluator();
	if( 0 == strcmp( functionName, "dump_vars" ) )
		return new DumpVariablesFunctionEvaluator();
	if( 0 == strcmp( functionName, "del_vars" ) )
		return new DeleteAllVariablesFunctionEvaluator();
	if( 0 == strcmp( functionName, "do" ) )
		return new DoFunctionEvaluator();
	if( 0 == strcmp( functionName, "if" ) )
		return new IfFunctionEvaluator();
	if( 0 == strcmp( functionName, "while" ) )
		return new LoopFunctionEvaluator( LoopFunctionEvaluator::LOOP_WHILE );
	if( 0 == strcmp( functionName, "until" ) )
		return new LoopFunctionEvaluator( LoopFunctionEvaluator::LOOP_UNTIL );
	if( 0 == strcmp( functionName, "deriv" ) )
		return new CalculusFunctionEvaluator( CalculusFunctionEvaluator::TAKE_DERIVATIVE );
	if( 0 == strcmp( functionName, "antideriv" ) )
		return new CalculusFunctionEvaluator( CalculusFunctionEvaluator::TAKE_ANTI_DERIVATIVE );

	return 0;
}

//=========================================================================================
/*virtual*/ Evaluator* Environment::CreateBinaryOperator( const char* operatorName, Evaluator* leftOperand, Evaluator* rightOperand, bool& isBinaryOperationEvaluator )
{
	// Here we implement exponentiation in terms of Tayler series.
	// This works for any type of number, because the Tyler series
	// function only uses the arithmetic operations of the number interface.
	// We re-write x^y as exp(y*ln x).  The Tayler series are centered at zero.
	// If the user does not want it centered at zero, they must call the
	// Tyler series functions themselves.  Environment derivatives, of course,
	// can override this function to give the "^" operator a different meaning.
	if( 0 == strcmp( operatorName, "^" ) )
	{
		Number* zero = CreateNumber();
		zero->AssignAdditiveIdentity( *this );

		FunctionArgumentEvaluator* argumentEvaluator = new FunctionArgumentEvaluator();
		argumentEvaluator->Argument( leftOperand );

		TaylorSeriesFunctionEvaluator* lnEvaluator = new TaylorSeriesFunctionEvaluator();
		lnEvaluator->Center( zero, *this );
		lnEvaluator->TaylorSeries( TaylorSeriesFunctionEvaluator::TAYLOR_SERIES_LN );
		lnEvaluator->AddArgument( argumentEvaluator, FunctionEvaluator::APPEND_TO_LIST_OF_ARGS );

		BinaryArithmeticOperationEvaluator* mulEvaluator = new BinaryArithmeticOperationEvaluator( BinaryArithmeticOperationEvaluator::ARITHMETIC_OPERATION_MULTIPLY );
		mulEvaluator->LeftOperand( rightOperand );
		mulEvaluator->RightOperand( lnEvaluator );

		argumentEvaluator = new FunctionArgumentEvaluator();
		argumentEvaluator->Argument( mulEvaluator );

		TaylorSeriesFunctionEvaluator* expEvaluator = new TaylorSeriesFunctionEvaluator();
		expEvaluator->Center( zero, *this );
		expEvaluator->TaylorSeries( TaylorSeriesFunctionEvaluator::TAYLOR_SERIES_EXP );
		expEvaluator->AddArgument( argumentEvaluator, FunctionEvaluator::APPEND_TO_LIST_OF_ARGS );

		// We're not actually returning a binary operation evaluator.
		isBinaryOperationEvaluator = false;
		delete zero;
		return expEvaluator;
	}

	return 0;
}

//=========================================================================================
/*virtual*/ Evaluator* Environment::CreateUnaryOperator( const char* operatorName, Evaluator* operand, bool& isUnaryOperationEvaluator )
{
	return 0;
}

//=========================================================================================
bool Environment::VariableExists( const char* variableName )
{
	if( variableMap.Lookup( variableName ) )
		return true;
	return false;
}

//=========================================================================================
Number* Environment::AccessStoredVariable( const char* variableName )
{
	Number* foundNumber = 0;
	if( !variableMap.Lookup( variableName, &foundNumber ) )
		return 0;
	return foundNumber;
}

//=========================================================================================
bool Environment::LookupVariable( const char* variableName, Number& variableValue )
{
	Number* foundNumber = 0;
	if( !variableMap.Lookup( variableName, &foundNumber ) )
	{
		AddError( "Failed to find variable \"%s\" in the environment.", variableName );
		return false;
	}

	if( !variableValue.AssignFrom( foundNumber, *this ) )
		return false;

	return true;
}

//=========================================================================================
void Environment::DumpVariables( void )
{
	int varCount = variableMap.Count();
	if( varCount == 0 )
		Print( "\nThere are currently no variables stored.\n\n" );
	else
	{
		if( varCount > 1 )
			Print( "\nThere are %d variables:\n\n", varCount );
		else
			Print( "There is 1 variable:\n\n" );
		Utilities::Map< Number* >::Iterator iterator( &variableMap );
		while( !iterator.Finished() )
		{
			const char* entryName = 0;
			Number* number = iterator.CurrentEntry( &entryName );
			Print( "%s = ", entryName );
			number->Print( *this );
			Print( "\n" );
			iterator.Next();
		}
		Print( "\n" );
	}
}

//=========================================================================================
void Environment::DeleteAllVariables( void )
{
	variableMap.DeleteAndRemoveAll();
}

//=========================================================================================
bool Environment::StoreVariable( const char* variableName, const Number& variableValue )
{
	Number* foundNumber = 0;
	if( variableMap.Lookup( variableName, &foundNumber ) )
	{
		variableMap.Remove( variableName );
		delete foundNumber;
	}

	Number* numberCopy = variableValue.CreateCopy( *this );
	if( !variableMap.Insert( variableName, numberCopy ) )
		return false;

	return true;
}

//=========================================================================================
void Environment::WipeAllErrors( void )
{
	listOfErrors.RemoveAll( true );
}

//=========================================================================================
void Environment::PrintAllErrors( void )
{
	for( Error* error = ( Error* )listOfErrors.LeftMost(); error; error = ( Error* )error->Right() )
		Print( "Error: %s\n", error->errorString );
}

//=========================================================================================
void Environment::AddError( const char* formatString, ... )
{
	char errorMessageString[ 1024 ];

	va_list args;
	va_start( args, formatString );
	vsprintf_s( errorMessageString, sizeof( errorMessageString ), formatString, args );
	va_end( args );

	Error* error = new Error( errorMessageString );
	listOfErrors.InsertRightOf( listOfErrors.RightMost(), error );
}

//=========================================================================================
int Environment::ErrorCount( void )
{
	return listOfErrors.Count();
}

//=========================================================================================
Environment::Error::Error( const char* errorString )
{
	int errorStringLength = ( int )strlen( errorString ) + 1;
	this->errorString = new char[ errorStringLength ];
	strcpy_s( this->errorString, errorStringLength, errorString );
}

//=========================================================================================
/*virtual*/ Environment::Error::~Error( void )
{
	delete[] errorString;
}

// Environment.cpp