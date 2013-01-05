// FractionEnv.cpp

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
IMPLEMENT_CALCLIB_CLASS1( FractionEnvironment, Environment );

//=========================================================================================
FractionEnvironment::FractionEnvironment( void )
{
}

//=========================================================================================
/*virtual*/ FractionEnvironment::~FractionEnvironment( void )
{
}

//=========================================================================================
/*virtual*/ void FractionEnvironment::PrintEnvironmentInfo( void )
{
	Print( "\nYou are in the fractions environment.\n\n" );
}

//=========================================================================================
/*virtual*/ Number* FractionEnvironment::CreateNumber( void )
{
	Number* number = new FractionNumber();
	return number;
}

//=========================================================================================
/*virtual*/ FunctionEvaluator* FractionEnvironment::CreateFunction( const char* functionName )
{
	return Environment::CreateFunction( functionName );
}

//=========================================================================================
/*virtual*/ Evaluator* FractionEnvironment::CreateVariable( const char* variableName )
{
	return 0;
}

// FractionEnv.cpp