// Variable.cpp

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
IMPLEMENT_CALCLIB_CLASS1( VariableEvaluator, Evaluator );

//=========================================================================================
VariableEvaluator::VariableEvaluator( const char* variableName )
{
	int nameLength = ( int )strlen( variableName ) + 1;
	this->variableName = new char[ nameLength ];
	strcpy_s( this->variableName, nameLength, variableName );
}

//=========================================================================================
/*virtual*/ VariableEvaluator::~VariableEvaluator( void )
{
	delete[] variableName;
}

//=========================================================================================
/*virtual*/ Number* VariableEvaluator::CreateNumberForResult( Environment& environment )
{
	Number* number = environment.AccessStoredVariable( variableName );
	if( !number )
		return 0;

	if( number->IsTypeOf( StringNumber::ClassName() ) )
		return new StringNumber();

	return 0;
}

//=========================================================================================
/*virtual*/ bool VariableEvaluator::EvaluateResult( Number& result, Environment& environment )
{
	// We don't auto-declare variables.
	if( !environment.LookupVariable( variableName, result ) )
	{
		environment.AddError( "Failed to lookup variable \"%s\".", variableName );
		return false;
	}
	return true;
}

//=========================================================================================
/*virtual*/ bool VariableEvaluator::StoreResult( const Number& result, Environment& environment )
{
	if( !environment.StoreVariable( variableName, result ) )
	{
		environment.AddError( "Failed to store variable \"%s\".", variableName );
		return false;
	}
	return true;
}

//=========================================================================================
const char* VariableEvaluator::Name( void ) const
{
	return variableName;
}

// Variable.cpp