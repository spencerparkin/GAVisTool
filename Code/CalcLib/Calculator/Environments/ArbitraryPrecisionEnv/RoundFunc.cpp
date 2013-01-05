// RoundFunc.cpp

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
IMPLEMENT_CALCLIB_CLASS1( RoundNumberFunctionEvaluator, FunctionEvaluator );

//=========================================================================================
RoundNumberFunctionEvaluator::RoundNumberFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ RoundNumberFunctionEvaluator::~RoundNumberFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ bool RoundNumberFunctionEvaluator::EvaluateResult( Number& result, Environment& environment )
{
	bool success = false;
	Number** numberArray = 0;
	int numberArraySize = 0;
	
	do
	{
		// Make our result zero now in case anything goes wrong later.
		if( !result.AssignAdditiveIdentity( environment ) )
			break;

		// Evaluate our arguments.
		if( !EvaluateArguments( numberArray, numberArraySize, environment ) )
			break;

		// Expect two and only two arguments.
		if( numberArraySize != 2 )
		{
			environment.AddError( "The \"round\" function expects two and only two arguments." );
			break;
		}

		if( !result.AssignFrom( numberArray[0], environment ) )
			break;

		double argument;
		if( !numberArray[1]->AssignTo( argument, environment ) )
			break;
		
		int degree = int( argument );
		if( double( degree ) != argument )
		{
			environment.AddError( "Cannot round to nearest degree of %f.", argument );
			break;
		}

		if( !result.IsTypeOf( RationalNumber::ClassName() ) )
			break;

		RationalNumber* rationalNumber = ( RationalNumber* )&result;
		rationalNumber->RoundToNearestDegree( degree );

		success = true;
	}
	while( false );

	DeleteArguments( numberArray, numberArraySize );

	return success;
}

// RoundFunc.cpp