// ArbitraryPrecisionEnv.cpp

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "../../CalcLib.h"

using namespace CalcLib;

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( ArbitraryPrecisionEnvironment, Environment );

//=========================================================================================
ArbitraryPrecisionEnvironment::ArbitraryPrecisionEnvironment( void )
{
	divisionLimit = -10;
	printFormat = PRINT_COMPACT_NOTATION;
}

//=========================================================================================
/*virtual*/ ArbitraryPrecisionEnvironment::~ArbitraryPrecisionEnvironment( void )
{
}

//=========================================================================================
/*virtual*/ void ArbitraryPrecisionEnvironment::PrintEnvironmentInfo( void )
{
	Print
	(
		"\n"
		"You are in the arbitrary precision arithmetic environment.\n"
		"\n"
		"Arguments in square brackets are optional.\n"
		"Arguments not in square brackets are not optional.\n"
		"\n"
		"Available functions are:\n"
		"\n"
		"\tdiv_limit([x]) -- Set/get the division limit.\n"
		"\tround(x,y) -- Round x to the nearest digit of power y.\n"
		"\tconvert_base(x,y) -- Convert x to base y.\n"
		"\n"
	);

	Environment::PrintEnvironmentInfo();
}

//=========================================================================================
/*virtual*/ Number* ArbitraryPrecisionEnvironment::CreateNumber( void )
{
	RationalNumber* rationalNumber = new RationalNumber( 10 );
	rationalNumber->AssignAdditiveIdentity( *this );
	Number* number = rationalNumber;
	return number;
}

//=========================================================================================
/*virtual*/ FunctionEvaluator* ArbitraryPrecisionEnvironment::CreateFunction( const char* functionName )
{
	FunctionEvaluator* functionEvaluator = 0;
	
	if( 0 == strcmp( functionName, "div_limit" ) )
		functionEvaluator = new DivisionLimitFunctionEvaluator();
	else if( 0 == strcmp( functionName, "round" ) )
		functionEvaluator = new RoundNumberFunctionEvaluator();
	else if( 0 == strcmp( functionName, "convert_base" ) )
		functionEvaluator = new BaseConverterFunctionEvaluator();
	else
		functionEvaluator = Environment::CreateFunction( functionName );

	return functionEvaluator;
}

//=========================================================================================
/*virtual*/ Evaluator* ArbitraryPrecisionEnvironment::CreateVariable( const char* variableName )
{
	return 0;
}

//=========================================================================================
void ArbitraryPrecisionEnvironment::DivisionLimit( int divisionLimit )
{
	this->divisionLimit = divisionLimit;
}

//=========================================================================================
int ArbitraryPrecisionEnvironment::DivisionLimit( void )
{
	return divisionLimit;
}

//=========================================================================================
void ArbitraryPrecisionEnvironment::PrintFormat( PrintFormatType printFormatType )
{
	this->printFormat = printFormat;
}

//=========================================================================================
ArbitraryPrecisionEnvironment::PrintFormatType ArbitraryPrecisionEnvironment::PrintFormat( void )
{
	return printFormat;
}

// ArbitraryPrecisionEnv.cpp