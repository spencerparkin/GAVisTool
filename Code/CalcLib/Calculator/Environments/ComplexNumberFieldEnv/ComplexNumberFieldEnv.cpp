// ComplexNumberFieldEnv.cpp

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
IMPLEMENT_CALCLIB_CLASS1( ComplexNumberFieldEnvironment, Environment );

//=========================================================================================
ComplexNumberFieldEnvironment::ComplexNumberFieldEnvironment( void )
{
}

//=========================================================================================
/*virtual*/ ComplexNumberFieldEnvironment::~ComplexNumberFieldEnvironment( void )
{
}

//=========================================================================================
/*virtual*/ void ComplexNumberFieldEnvironment::PrintEnvironmentInfo( void )
{
	Print( "\nYou are in the complex number field environment.\n\n" );
}

//=========================================================================================
/*virtual*/ Number* ComplexNumberFieldEnvironment::CreateNumber( void )
{
	Number* number = new ComplexNumber( 0.0, 0.0 );
	return number;
}

//=========================================================================================
/*virtual*/ FunctionEvaluator* ComplexNumberFieldEnvironment::CreateFunction( const char* functionName )
{
	// We should be able to use the Tayler Series functions, because they do not depend on the type of number.
	// That is, they work with any type of number.

	return Environment::CreateFunction( functionName );
}

//=========================================================================================
/*virtual*/ Evaluator* ComplexNumberFieldEnvironment::CreateVariable( const char* variableName )
{
	if( 0 == strcmp( variableName, "i" ) )
	{
		ComplexNumber* imaginaryUnit = new ComplexNumber( 0.0, 1.0 );
		ConstantEvaluator* evaluator = new ConstantEvaluator( imaginaryUnit );
		return evaluator;
	}

	return 0;
}

// ComplexNumberFieldEnv.cpp