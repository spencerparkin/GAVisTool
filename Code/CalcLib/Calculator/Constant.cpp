// Constant.cpp

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
IMPLEMENT_CALCLIB_CLASS1( ConstantEvaluator, Evaluator );

//=========================================================================================
ConstantEvaluator::ConstantEvaluator( Number* constant )
{
	this->constant = constant;
}

//=========================================================================================
/*virtual*/ ConstantEvaluator::~ConstantEvaluator( void )
{
	delete constant;
}

//=========================================================================================
/*virtual*/ bool ConstantEvaluator::EvaluateResult( Number& result, Environment& environment )
{
	if( !result.AssignFrom( constant, environment ) )
	{
		environment.AddError( "Assignment operation failed." );
		return false;
	}
	return true;
}

//=========================================================================================
/*virtual*/ bool ConstantEvaluator::StoreResult( const Number& result, Environment& environment )
{
	environment.AddError( "A number cannot be stored into a contant." );
	return false;
}

// Constant.cpp