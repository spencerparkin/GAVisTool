// MultivectorMathFunc.cpp

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
IMPLEMENT_CALCLIB_CLASS1( MultivectorMathFunctionEvaluator, FunctionEvaluator );

//=========================================================================================
MultivectorMathFunctionEvaluator::MultivectorMathFunctionEvaluator( FunctionType functionType )
{
	this->functionType = functionType;
}

//=========================================================================================
/*virtual*/ MultivectorMathFunctionEvaluator::~MultivectorMathFunctionEvaluator( void )
{
}

//=========================================================================================
bool MultivectorMathFunctionEvaluator::EvaluateScalarArguments( double**& scalarNumberArray, Number** numberArray, int numberArraySize, Environment& environment )
{
	if( numberArraySize <= 0 )
	{
		scalarNumberArray = 0;
		return true;
	}

	scalarNumberArray = new double*[ numberArraySize ];
	for( int index = 0; index < numberArraySize; index++ )
		scalarNumberArray[ index ] = 0;

	for( int index = 0; index < numberArraySize; index++ )
	{
		MultivectorNumber* multivectorNumber = ( MultivectorNumber* )numberArray[ index ];

		GeometricAlgebra::SumOfBlades multivector;
		if( !multivectorNumber->AssignTo( multivector, environment ) )
			return false;

		if( !multivector.IsHomogeneousOfGrade(0) )
			continue;
		
		GeometricAlgebra::Scalar scalar = 0.0;
		if( multivector.BladeCount() == 1 )
		{
			GeometricAlgebra::Blade blade;
			if( !multivector.AssignBladeTo( blade, 0 ) )
				return false;
			if( !blade.AssignScalarPartTo( scalar ) )
				return false;
		}
	
		double* scalarFloat = new double;
		*scalarFloat = scalar;		// Oops!  Error in overloaded assignment operator?
		scalarNumberArray[ index ] = scalarFloat;
	}

	return true;
};

//=========================================================================================
void MultivectorMathFunctionEvaluator::DeleteScalarArguments( double**& scalarNumberArray, int numberArraySize )
{
	if( scalarNumberArray )
	{
		for( int index = 0; index < numberArraySize; index++ )
			if( scalarNumberArray[ index ] )
				delete scalarNumberArray[ index ];
		delete[] scalarNumberArray;
		scalarNumberArray = 0;
	}
}

//=========================================================================================
/*virtual*/ bool MultivectorMathFunctionEvaluator::EvaluateResult( Number& result, Environment& environment )
{
	bool success = false;
	Number** numberArray = 0;
	int numberArraySize = 0;
	double** scalarNumberArray = 0;

	do
	{
		// Verify the output type.
		if( !result.IsTypeOf( MultivectorNumber::ClassName() ) )
			break;
		MultivectorNumber* multivectorNumber = ( MultivectorNumber* )&result;

		// Get our inputs.
		if( !EvaluateArguments( numberArray, numberArraySize, environment ) )
			break;

		// Verify the input types.
		int index;
		for( index = 0; index < numberArraySize; index++ )
			if( !numberArray[ index ]->IsTypeOf( MultivectorNumber::ClassName() ) )
				break;
		if( index < numberArraySize )
			break;

		// Figure out which arguments are scalars, if any.
		if( !EvaluateScalarArguments( scalarNumberArray, numberArray, numberArraySize, environment ) )
			break;

		// Calculate our output.
		bool functionPerformed = false;
		switch( functionType )
		{
			case FUNC_EXP:
			{
				if( numberArraySize != 1 )
				{
					environment.AddError( "The exp() function takes one and only one argument." );
					break;
				}

				if( !scalarNumberArray[0] )
				{
					// TODO: We should be able to create rotors as exponentials of blades, etc.
					environment.AddError( "I'm not yet sure how to take the exp() of a multivector not homogenous of grade zero.  Use Tayler series?" );
					break;
				}
				else
				{
					double exponentialArgument = *scalarNumberArray[0];
					double exponentialResult = exp( exponentialArgument );
					if( !multivectorNumber->AssignFrom( exponentialResult, environment ) )
						break;

					functionPerformed = true;
				}

				break;
			}
			case FUNC_LOG:
			{
				// TODO: We should be able to take logarithms of multivectors generally.
				break;
			}
			case FUNC_POW:
			{
				// TODO: x^y = exp( log( x^y ) ) = exp( y*log( x ) ) ?
				break;
			}
			case FUNC_SQRT:
			{
				if( numberArraySize != 1 )
				{
					environment.AddError( "The sqrt() function takes one and only one argument." );
					break;
				}

				if( !scalarNumberArray[0] )
				{
					environment.AddError( "I'm not yet sure how to take the square root of a multivector not homogenous of grade zero." );
					break;
				}
				else
				{
					double radicalArgument = *scalarNumberArray[0];
					if( radicalArgument < 0.0 )
					{
						environment.AddError( "Imaginary number encounted by the sqrt() function!" );
						break;
					}

					double radicalResult = sqrt( radicalArgument );
					if( !multivectorNumber->AssignFrom( radicalResult, environment ) )
						break;

					functionPerformed = true;
				}

				break;
			}
			case FUNC_COS:
			{
				if( numberArraySize != 1 )
				{
					environment.AddError( "The cos() function takes one and only one argument." );
					break;
				}

				if( !scalarNumberArray[0] )
				{
					environment.AddError( "Not yet sure how to take the cosine of a multivector.  Use Tayler series?." );
					break;
				}
				else
				{
					double cosineArgument = *scalarNumberArray[0];
					double cosineResult = cos( cosineArgument );
					if( !multivectorNumber->AssignFrom( cosineResult, environment ) )
						break;

					functionPerformed = true;
				}

				break;
			}
			case FUNC_SIN:
			{
				if( numberArraySize != 1 )
				{
					environment.AddError( "The sin() function takes one and only one argument." );
					break;
				}

				if( !scalarNumberArray[0] )
				{
					environment.AddError( "Not yet sure how to take the sine of a multivector.  Use Tayler series?." );
					break;
				}
				else
				{
					double sineArgument = *scalarNumberArray[0];
					double sineResult = sin( sineArgument );
					if( !multivectorNumber->AssignFrom( sineResult, environment ) )
						break;

					functionPerformed = true;
				}

				break;
			}
			case FUNC_TAN:
			{
				break;
			}
			case FUNC_ABS:
			{
				if( numberArraySize != 1 )
				{
					environment.AddError( "The abs() function takes one and only one argument." );
					break;
				}

				if( !scalarNumberArray[0] )
				{
					// This code should work for multivectors homogeneous of grade zero
					// too, but it is over-kill in the case that we're dealing with a scalar.

					GeometricAlgebra::SumOfBlades multivector;
					if( !( ( MultivectorNumber* )numberArray[0] )->AssignTo( multivector, environment ) )
						break;

					GeometricAlgebra::Scalar squareMagnitude;
					if( !multivector.AssignSquareMagnitudeTo( squareMagnitude ) )
						break;

					double squareAbs = squareMagnitude;
					double magnitude = sqrt( squareAbs );
					if( !multivectorNumber->AssignFrom( magnitude, environment ) )
						break;

					functionPerformed = true;
				}
				else
				{
					double absArgument = *scalarNumberArray[0];
					double absResult = abs( absArgument );
					if( !multivectorNumber->AssignFrom( absResult, environment ) )
						break;

					functionPerformed = true;
				}

				break;
			}
			case FUNC_ACOS:
			{
				if( numberArraySize != 1 )
				{
					environment.AddError( "The acos() function takes one and only one argument." );
					break;
				}

				if( !scalarNumberArray[0] )
				{
					environment.AddError( "Not yet sure how to take the acos() of a multivector.  Use Tayler series?" );
					break;
				}
				else
				{
					double acosArgument = *scalarNumberArray[0];
					double acosResult = acos( acosArgument );
					if( !multivectorNumber->AssignFrom( acosResult, environment ) )
						break;

					functionPerformed = true;
				}

				break;
			}
			case FUNC_ASIN:
			{
				if( numberArraySize != 1 )
				{
					environment.AddError( "The asin() function takes one and only one argument." );
					break;
				}

				if( !scalarNumberArray[0] )
				{
					environment.AddError( "Not yet sure how to take the acos() of a multivector.  Use Tayler series?" );
					break;
				}
				else
				{
					double asinArgument = *scalarNumberArray[0];
					double asinResult = asin( asinArgument );
					if( !multivectorNumber->AssignFrom( asinResult, environment ) )
						break;

					functionPerformed = true;
				}

				break;
			}
		}

		if( !functionPerformed )
			break;

		success = true;
	}
	while( false );

	DeleteArguments( numberArray, numberArraySize );
	DeleteScalarArguments( scalarNumberArray, numberArraySize );

	return success;
}



// MultivectorMathFunc.cpp