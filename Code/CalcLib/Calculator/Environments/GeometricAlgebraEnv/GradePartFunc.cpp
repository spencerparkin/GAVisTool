// GradePartFunc.cpp

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
IMPLEMENT_CALCLIB_CLASS1( GradePartFunctionEvaluator, FunctionEvaluator );

//=========================================================================================
GradePartFunctionEvaluator::GradePartFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ GradePartFunctionEvaluator::~GradePartFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ bool GradePartFunctionEvaluator::EvaluateResult( Number& result, Environment& environment )
{
	bool success = false;
	Number* gradeNumber = 0;
	Number* multivectorNumber = 0;

	do
	{
		// Make sure we got our first argument.
		FunctionArgumentEvaluator* gradeArgument = GetArgument(0);
		if( !gradeArgument )
		{
			environment.AddError( "The grade_part() function expects a scalar as its first argument." );
			break;
		}

		// Make sure we got our second argument.
		FunctionArgumentEvaluator* multivectorArgument = GetArgument(1);
		if( !multivectorArgument )
		{
			environment.AddError( "The grade_part() function expects a multivector as its second argument." );
			break;
		}

		// Evaluate our first argument.
		gradeNumber = environment.CreateNumber( gradeArgument );
		if( !gradeArgument->EvaluateResult( *gradeNumber, environment ) )
			break;

		// Evaluate our second argument.
		multivectorNumber = environment.CreateNumber( multivectorArgument );
		if( !multivectorArgument->EvaluateResult( *multivectorNumber, environment ) )
			break;

		// Try to suck a scalar out of the first argument.
		double gradeFloat;
		if( !gradeNumber->AssignTo( gradeFloat, environment ) )
		{
			environment.AddError( "The grade_part() function failed to get a scalar out of its first given argument." );
			break;
		}

		// Make sure it's an integer scalar.
		int grade = int( gradeFloat );
		if( double( grade ) != gradeFloat )
		{
			environment.AddError( "The grade_part() function expects an integer as its first argument, not %f.", gradeFloat );
			break;
		}

		// Make sure our second argument is a multivector.
		if( !multivectorNumber->IsTypeOf( MultivectorNumber::ClassName() ) )
		{
			environment.AddError( "The grade_part() function expects a multivector as its second argument." );
			break;
		}

		// Pull the multivector out of the second argument.
		GeometricAlgebra::SumOfBlades multivector;
		if( !( ( MultivectorNumber* )multivectorNumber )->AssignTo( multivector, environment ) )
			break;

		// Finally, do the work.
		GeometricAlgebra::SumOfBlades homogeneousPart;
		if( !multivector.GradePart( grade, homogeneousPart ) )
		{
			environment.AddError( "Failed to get the grade %d part out of the given multivectors.", grade );
			break;
		}

		// Make sure our result is of the correct number type.
		if( !result.IsTypeOf( MultivectorNumber::ClassName() ) )
		{
			environment.AddError( "The grade_part() function result can only be put into a multivector number." );
			break;
		}

		// And now store the result.
		MultivectorNumber* resultNumber = ( MultivectorNumber* )&result;
		if( !resultNumber->AssignFrom( homogeneousPart, environment ) )
			break;

		success = true;
	}
	while( false );

	delete gradeNumber;
	delete multivectorNumber;

	return success;
}

// GradePartFunc.cpp