// Calculus.cpp

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
IMPLEMENT_CALCLIB_CLASS1( CalculusFunctionEvaluator, FunctionEvaluator );

//=========================================================================================
CalculusFunctionEvaluator::CalculusFunctionEvaluator( CalculusFunc calculusFunc )
{
	this->calculusFunc = calculusFunc;
}

//=========================================================================================
/*virtual*/ CalculusFunctionEvaluator::~CalculusFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ bool CalculusFunctionEvaluator::EvaluateResult( Number& result, Environment& environment )
{
	bool success = false;
	Number* number = 0;

	do
	{
		if( calculusFunc == TAKE_DERIVATIVE || calculusFunc == TAKE_ANTI_DERIVATIVE )
		{
			// Make sure we were given the right number of arguments.
			if( GetArgumentCount() != 2 )
			{
				environment.AddError( "The \"%s\" function requires 2 arguments, not %d.",
					( calculusFunc == TAKE_DERIVATIVE ? "deriv" : "antideriv" ), GetArgumentCount() );
				break;
			}

			// What variable are we taking the anti/derivative with respect to?
			FunctionArgumentEvaluator* argument = GetArgument(1);
			Evaluator* evaluator = argument->Argument();
			StringEvaluator* stringEvaluator = evaluator->Cast< StringEvaluator >();
			if( !stringEvaluator )
			{
				environment.AddError( "The \"%s\" function requires the 2nd argument be a variable name.",
					( calculusFunc == TAKE_DERIVATIVE ? "deriv" : "antideriv" ) );
				break;
			}

			// Evaluate the expression we'll be taking the derivative of.
			argument = GetArgument(0);
			number = environment.CreateNumber( argument );
			if( !argument->EvaluateResult( *number, environment ) )
				break;
			
			// Perform the function.
			const char* variableName = stringEvaluator->String();
			bool derivativeSuccess = false;
			if( calculusFunc == TAKE_DERIVATIVE )
				derivativeSuccess = result.AssignDerivative( number, variableName, environment );
			else
				derivativeSuccess = result.AssignAntiDerivative( number, variableName, environment );
			if( !derivativeSuccess )
			{
				environment.AddError( "The number type of the calculator environment failed to perform the anti/derivative function." );
				break;
			}
		}

		success = true;
	}
	while( false );

	if( number )
		delete number;
	
	return success;
}

// Calculus.cpp