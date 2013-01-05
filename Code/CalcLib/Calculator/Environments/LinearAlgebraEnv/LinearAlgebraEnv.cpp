// LinearAlgebraEnv.cpp

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
IMPLEMENT_CALCLIB_CLASS1( LinearAlgebraEnvironment, Environment );

//=========================================================================================
LinearAlgebraEnvironment::LinearAlgebraEnvironment( void )
{
}

//=========================================================================================
/*virtual*/ LinearAlgebraEnvironment::~LinearAlgebraEnvironment( void )
{
}

//=========================================================================================
/*virtual*/ void LinearAlgebraEnvironment::PrintEnvironmentInfo( void )
{
	Print( "You are in the linear algebra environment.\n" );
}

//=========================================================================================
/*virtual*/ Number* LinearAlgebraEnvironment::CreateNumber( void )
{
	Number* number = new MatrixNumber();
	return number;
}

//=========================================================================================
/*virtual*/ FunctionEvaluator* LinearAlgebraEnvironment::CreateFunction( const char* functionName )
{
	if( 0 == strcmp( functionName, "mtx_create" ) )
		return new MatrixFunctionEvaluator( MatrixFunctionEvaluator::CREATE_MATRIX );
	else if( 0 == strcmp( functionName, "mtx_set" ) )
		return new MatrixFunctionEvaluator( MatrixFunctionEvaluator::SET_ELEMENT );
	else if( 0 == strcmp( functionName, "mtx_set_row" ) )
		return new MatrixFunctionEvaluator( MatrixFunctionEvaluator::SET_ROW );
	else if( 0 == strcmp( functionName, "mtx_set_col" ) )
		return new MatrixFunctionEvaluator( MatrixFunctionEvaluator::SET_COLUMN );
	else if( 0 == strcmp( functionName, "mtx_get" ) )
		return new MatrixFunctionEvaluator( MatrixFunctionEvaluator::GET_ELEMENT );
	else if( 0 == strcmp( functionName, "mtx_get_row" ) )
		return new MatrixFunctionEvaluator( MatrixFunctionEvaluator::GET_ROW );
	else if( 0 == strcmp( functionName, "mtx_get_col" ) )
		return new MatrixFunctionEvaluator( MatrixFunctionEvaluator::GET_COLUMN );
	else if( 0 == strcmp( functionName, "mtx_inv" ) )
		return new MatrixFunctionEvaluator( MatrixFunctionEvaluator::CALC_INVERSE );
	else if( 0 == strcmp( functionName, "mtx_det" ) )
		return new MatrixFunctionEvaluator( MatrixFunctionEvaluator::CALC_DETERMINANT );

	return Environment::CreateFunction( functionName );
}

//=========================================================================================
/*virtual*/ Evaluator* LinearAlgebraEnvironment::CreateVariable( const char* variableName )
{
	return 0;
}

// LinearAlgebraEnv.cpp