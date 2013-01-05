// BaseConverterFunc.cpp

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
IMPLEMENT_CALCLIB_CLASS1( BaseConverterFunctionEvaluator, FunctionEvaluator );

//=========================================================================================
BaseConverterFunctionEvaluator::BaseConverterFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ BaseConverterFunctionEvaluator::~BaseConverterFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ bool BaseConverterFunctionEvaluator::EvaluateResult( Number& result, Environment& environment )
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
			environment.AddError( "The \"convert_base\" function expects two and only two arguments." );
			break;
		}

		// Our result will be the first argument, hopefully converted to the desired base.
		RationalNumber numberToConvert( 2 );
		if( !numberToConvert.AssignFrom( numberArray[0], environment ) )
			break;

		// The second argument is the desired base.
		double baseArgument;
		if( !numberArray[1]->AssignTo( baseArgument, environment ) )
			break;
		
		// Make sure it's actually an integer and that it is greater than 1.
		int base = int( baseArgument );
		if( base <= 1 || double( base ) != baseArgument )
		{
			environment.AddError( "Cannot convert a number to base %f.", baseArgument );
			break;
		}

		// Do the conversion to the desired base.
		if( !result.IsTypeOf( RationalNumber::ClassName() ) )
			break;
		RationalNumber* rationalNumber = ( RationalNumber* )&result;
		if( !numberToConvert.ConvertToBase( base, *rationalNumber, environment ) )
			break;

		success = true;
	}
	while( false );

	DeleteArguments( numberArray, numberArraySize );

	return success;
}

// BaseConverterFunc.cpp