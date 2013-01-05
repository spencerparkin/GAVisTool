// Language.cpp

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
IMPLEMENT_CALCLIB_CLASS1( DoFunctionEvaluator, FunctionEvaluator );

//=========================================================================================
DoFunctionEvaluator::DoFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ DoFunctionEvaluator::~DoFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ bool DoFunctionEvaluator::EvaluateResult( Number& result, Environment& environment )
{
	bool success = false;
	Number** numberArray = 0;
	int numberArraySize = 0;
	
	do
	{
		if( !EvaluateArguments( numberArray, numberArraySize, environment ) )
			break;

		if( numberArraySize > 0 && !result.AssignFrom( numberArray[ numberArraySize - 1 ], environment ) )
			break;

		success = true;
	}
	while( false );

	DeleteArguments( numberArray, numberArraySize );
	return success;
}

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( IfFunctionEvaluator, FunctionEvaluator );

//=========================================================================================
IfFunctionEvaluator::IfFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ IfFunctionEvaluator::~IfFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ bool IfFunctionEvaluator::EvaluateResult( Number& result, Environment& environment )
{
	Evaluator* conditionEvaluator = GetArgument(0);
	if( !conditionEvaluator )
	{
		environment.AddError( "The first argument to the if-function needs to be the conditional expression." );
		return false;
	}

	// Evaluate the conditional expression.
	Number* conditionResult = environment.CreateNumber( conditionEvaluator );
	if( !conditionEvaluator->EvaluateResult( *conditionResult, environment ) )
		return false;

	bool isAdditiveIdentity;
	if( !conditionResult->IsAdditiveIdentity( isAdditiveIdentity, environment ) )
		return false;

	bool conditionPassed = !isAdditiveIdentity;

	// It is not an error if either branch is not specified.
	if( conditionPassed )
	{
		Evaluator* conditionPassesEvaluator = GetArgument(1);
		if( conditionPassesEvaluator )
		{
			if( !conditionPassesEvaluator->EvaluateResult( result, environment ) )
				return false;
		}
		else
		{
			if( !result.AssignMultiplicativeIdentity( environment ) )
				return false;
		}
	}
	else
	{
		Evaluator* conditionFailsEvaluator = GetArgument(2);
		if( conditionFailsEvaluator )
		{
			if( !conditionFailsEvaluator->EvaluateResult( result, environment ) )
				return false;
		}
		else
		{
			if( !result.AssignAdditiveIdentity( environment ) )
				return false;
		}
	}

	return true;
}

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( LoopFunctionEvaluator, FunctionEvaluator );

//=========================================================================================
LoopFunctionEvaluator::LoopFunctionEvaluator( LoopType loopType )
{
	this->loopType = loopType;
}

//=========================================================================================
/*virtual*/ LoopFunctionEvaluator::~LoopFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ bool LoopFunctionEvaluator::EvaluateResult( Number& result, Environment& environment )
{
	bool success = false;
	Number* conditionResult = 0;
	Number* loopResult = 0;
	int maxIterations = 10000;		// Should this be set in the environment?
	int numIterations = 0;

	do
	{
		if( !result.AssignAdditiveIdentity( environment ) )
			break;

		FunctionArgumentEvaluator* conditionEvaluator = GetArgument(0);
		if( !conditionEvaluator )
		{
			environment.AddError( "The loop function requires a conditional statement as the first argument." );
			break;
		}

		FunctionArgumentEvaluator* loopEvaluator = GetArgument(1);
		if( !loopEvaluator )
		{
			environment.AddError( "The loop function requires a loop statement as the second argument." );
			break;
		}

		Number* conditionResult = environment.CreateNumber( conditionEvaluator );
		Number* loopResult = environment.CreateNumber( loopEvaluator );

		bool conditionPassed = true;
		if( !conditionEvaluator->EvaluateResult( *conditionResult, environment ) )
		{
			environment.AddError( "Failed to evaluate result of loop conditional statement." );
			break;
		}

		bool isAdditiveIdentity;
		if( !conditionResult->IsAdditiveIdentity( isAdditiveIdentity, environment ) )
			break;

		if( loopType == LOOP_WHILE )
			conditionPassed = !isAdditiveIdentity;
		else
			conditionPassed = isAdditiveIdentity;

		bool loopFinishedNormally = true;
		while( conditionPassed )
		{
			if( numIterations++ >= maxIterations )
			{
				environment.AddError( "Bailing out of potentially infinite loop.  The maximum number of allowed iterations is %d.", maxIterations );
				loopFinishedNormally = false;
				break;
			}

			if( !loopEvaluator->EvaluateResult( *loopResult, environment ) )
			{
				environment.AddError( "Failed to evaluate result of loop statement." );
				loopFinishedNormally = false;
				break;
			}

			if( !conditionEvaluator->EvaluateResult( *conditionResult, environment ) )
			{
				environment.AddError( "Failed to evaluate result of loop conditional statement." );
				loopFinishedNormally = false;
				break;
			}

			bool isAdditiveIdentity;
			if( !conditionResult->IsAdditiveIdentity( isAdditiveIdentity, environment ) )
			{
				loopFinishedNormally = false;
				break;
			}

			if( loopType == LOOP_WHILE )
				conditionPassed = !isAdditiveIdentity;
			else
				conditionPassed = isAdditiveIdentity;
		}

		if( !loopFinishedNormally )
			break;

		if( !result.AssignFrom( loopResult, environment ) )
			break;

		success = true;
	}
	while( false );

	delete conditionResult;
	delete loopResult;

	return success;
}

// Language.cpp