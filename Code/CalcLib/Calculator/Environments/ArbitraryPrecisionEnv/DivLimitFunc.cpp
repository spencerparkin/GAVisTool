// DivLimitFunc.cpp

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
IMPLEMENT_CALCLIB_CLASS1( DivisionLimitFunctionEvaluator, FunctionEvaluator );

//=========================================================================================
DivisionLimitFunctionEvaluator::DivisionLimitFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ DivisionLimitFunctionEvaluator::~DivisionLimitFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ bool DivisionLimitFunctionEvaluator::EvaluateResult( Number& result, Environment& environment )
{
	bool success = false;
	Number** numberArray = 0;
	int numberArraySize = 0;
	
	do
	{
		// Make our result zero now in case anything goes wrong later.
		if( !result.AssignAdditiveIdentity( environment ) )
			break;

		// We should do some RTTI here to make sure we're in the right environment.
		ArbitraryPrecisionEnvironment* arbitraryPrecisionEnvironment = ( ArbitraryPrecisionEnvironment* )&environment;

		// Evaluate our arguments.
		if( !EvaluateArguments( numberArray, numberArraySize, environment ) )
			break;

		// Expect just one or no arguments.
		if( !( numberArraySize == 0 || numberArraySize == 1 ) )
		{
			environment.AddError( "The \"div_limit\" function expects one or zero arguments only." );
			break;
		}

		// If given an argument, use it to set the division limit.
		if( numberArraySize == 1 )
		{
			// Try to pull a double out of the number.
			double argument;
			if( !numberArray[0]->AssignTo( argument, environment ) )
				break;

			// See if the integer case of the double makes sense.
			int divisionLimit = int( argument );
			if( divisionLimit > 0 )		// Maybe there is a case where having a positive division limit is wanted?
			{
				environment.AddError( "A division limit of %d (cast from %f) is not good.", divisionLimit, argument );
				break;
			}

			// Set the new limit.
			arbitraryPrecisionEnvironment->DivisionLimit( divisionLimit );
		}
		
		// If given no argument or if we set a new limit, return the currently set division limit.
		int divisionLimit = arbitraryPrecisionEnvironment->DivisionLimit();
		if( !result.AssignFrom( divisionLimit, environment ) )
			break;

		success = true;
	}
	while( false );

	DeleteArguments( numberArray, numberArraySize );

	return success;
}

// DivLimitFunc.cpp