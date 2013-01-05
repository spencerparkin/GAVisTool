// ScalarPartFunc.cpp

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
IMPLEMENT_CALCLIB_CLASS1( ScalarPartFunctionEvaluator, FunctionEvaluator );

//=========================================================================================
ScalarPartFunctionEvaluator::ScalarPartFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ ScalarPartFunctionEvaluator::~ScalarPartFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ bool ScalarPartFunctionEvaluator::EvaluateResult( Number& result, Environment& environment )
{
	bool success = false;
	Number* multivectorNumber = 0;
	Number* bladeNumber = 0;

	do
	{
		// Make sure we got our first argument.
		FunctionArgumentEvaluator* multivectorArgument = GetArgument(0);
		if( !multivectorArgument )
		{
			environment.AddError( "The scalar_part() function expects a multivector as its first argument." );
			break;
		}

		// Make sure we got our second argument.
		FunctionArgumentEvaluator* bladeArgument = GetArgument(1);
		if( !bladeArgument )
		{
			environment.AddError( "The scalar_part() function expects a blade as its second argument." );
			break;
		}

		// Evaluate our first argument.
		multivectorNumber = environment.CreateNumber( multivectorArgument );
		if( !multivectorArgument->EvaluateResult( *multivectorNumber, environment ) )
			break;

		// Evaluate our second argument.
		bladeNumber = environment.CreateNumber( bladeArgument );
		if( !bladeArgument->EvaluateResult( *bladeNumber, environment ) )
			break;

		// Make sure our first argument is a multivector.
		if( !multivectorNumber->IsTypeOf( MultivectorNumber::ClassName() ) )
		{
			environment.AddError( "The scalar_part() function expects a multivector as its first argument." );
			break;
		}

		// Make sure our second argument is a multivector.
		if( !bladeNumber->IsTypeOf( MultivectorNumber::ClassName() ) )
		{
			environment.AddError( "The scalar_part() function expects a blade as its second argument." );
			break;
		}

		// Pull the multivector out of the first argument.
		GeometricAlgebra::SumOfBlades multivector;
		if( !( ( MultivectorNumber* )multivectorNumber )->AssignTo( multivector, environment ) )
			break;

		// Pull what should be just a blade out of the second argument.
		GeometricAlgebra::Blade blade;
		if( !( ( MultivectorNumber* )bladeNumber )->AssignTo( blade, environment ) )
			break;

		// Finally, do the work.
		GeometricAlgebra::Scalar scalar;
		if( !multivector.ScalarPart( blade, scalar ) )
		{
			environment.AddError( "Failed to calculate a scalar part of the given multivector." );
			break;
		}

		// Make sure our result is of the correct number type.
		if( !result.IsTypeOf( MultivectorNumber::ClassName() ) )
		{
			environment.AddError( "The scalar_part() function result can only be put into a multivector number." );
			break;
		}

		// And now store the result.
		MultivectorNumber* resultNumber = ( MultivectorNumber* )&result;
		if( !resultNumber->AssignFrom( scalar, environment ) )
			break;

		success = true;
	}
	while( false );

	delete multivectorNumber;
	delete bladeNumber;

	return success;
}

// ScalarPartFunc.cpp