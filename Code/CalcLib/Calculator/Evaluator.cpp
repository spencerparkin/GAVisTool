// Evaluator.cpp

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
IMPLEMENT_CALCLIB_CLASS0( Evaluator );

//=========================================================================================
Evaluator::Evaluator( void )
{
}

//=========================================================================================
/*virtual*/ Evaluator::~Evaluator( void )
{
}

//=========================================================================================
/*virtual*/ Number* Evaluator::CreateNumberForResult( Environment& environment )
{
	return 0;
}

//=========================================================================================
/*virtual*/ bool Evaluator::StoreResult( const Number& result, Environment& environment )
{
	environment.AddError( "Result cannot be stored." );
	return false;
}

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( UnaryOperationEvaluator, Evaluator );

//=========================================================================================
UnaryOperationEvaluator::UnaryOperationEvaluator( void )
{
	operand = 0;
}

//=========================================================================================
/*virtual*/ UnaryOperationEvaluator::~UnaryOperationEvaluator( void )
{
	if( operand )
		delete operand;
}

//=========================================================================================
bool UnaryOperationEvaluator::EvaluateOperand( Number*& number, Environment& environment )
{
	bool success = false;

	do
	{
		if( !operand )
		{
			environment.AddError( "Unary operator has no argument." );
			break;
		}

		// Call upon the environment to create the type of number we'll be working with.
		number = environment.CreateNumber( operand );
		if( !number )
		{
			environment.AddError( "Failed to create number from environment for unary operator." );
			break;
		}

		// Evaluate our operand.
		if( !operand->EvaluateResult( *number, environment ) )
		{
			environment.AddError( "Unary operand operand evaluation failed." );
			break;
		}

		success = true;
	}
	while( false );

	if( !success )
	{
		delete number;
		number = 0;
	}

	return success;
}

//=========================================================================================
void UnaryOperationEvaluator::Operand( Evaluator* operand )
{
	this->operand = operand;
}

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( BinaryOperationEvaluator, Evaluator );

//=========================================================================================
BinaryOperationEvaluator::BinaryOperationEvaluator( void )
{
	leftOperand = 0;
	rightOperand = 0;
}

//=========================================================================================
/*virtual*/ BinaryOperationEvaluator::~BinaryOperationEvaluator( void )
{
	if( leftOperand )
		delete leftOperand;
	if( rightOperand )
		delete rightOperand;
}

//=========================================================================================
bool BinaryOperationEvaluator::EvaluateOperands( Number*& leftNumber, Number*& rightNumber, Environment& environment )
{
	bool success = false;

	leftNumber = 0;
	rightNumber = 0;

	do
	{
		if( !leftOperand )
		{
			environment.AddError( "Binary operator has no left operand." );
			break;
		}

		if( !rightOperand )
		{
			environment.AddError( "Binary operator has no right operand." );
			break;
		}

		// Call upon the environment to create the types of numbers we'll be working with.
		leftNumber = environment.CreateNumber( this );
		rightNumber = environment.CreateNumber( this );
		if( !( leftNumber && rightNumber ) )
		{
			environment.AddError( "Failed to create numbers from environment for binary operation." );
			break;
		}
		
		// Evaluate our left and right operands.
		if( !leftOperand->EvaluateResult( *leftNumber, environment ) )
		{
			environment.AddError( "Failed to evaluate left operand of binary operator." );
			break;
		}
		if( !rightOperand->EvaluateResult( *rightNumber, environment ) )
		{
			environment.AddError( "Failed to evaluate right operand of binary operator." );
			break;
		}

		success = true;
	}
	while( false );

	if( !success )
	{
		delete leftNumber;
		delete rightNumber;
		leftNumber = 0;
		rightNumber = 0;
	}

	return success;
}

//=========================================================================================
void BinaryOperationEvaluator::LeftOperand( Evaluator* leftOperand )
{
	this->leftOperand = leftOperand;
}

//=========================================================================================
void BinaryOperationEvaluator::RightOperand( Evaluator* rightOperand )
{
	this->rightOperand = rightOperand;
}

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( FunctionArgumentEvaluator, Evaluator );

//=========================================================================================
FunctionArgumentEvaluator::FunctionArgumentEvaluator( void )
{
	argument = 0;
}

//=========================================================================================
/*virtual*/ FunctionArgumentEvaluator::~FunctionArgumentEvaluator( void )
{
	if( argument )
		delete argument;
}

//=========================================================================================
/*virtual*/ bool FunctionArgumentEvaluator::EvaluateResult( Number& result, Environment& environment )
{
	if( !argument )
	{
		environment.AddError( "Function argument evaluator has no argument to evaluate." );
		return false;
	}

	if( !argument->EvaluateResult( result, environment ) )
		return false;

	return true;
}

//=========================================================================================
/*virtual*/ Number* FunctionArgumentEvaluator::CreateNumberForResult( Environment& environment )
{
	if( argument )
		return argument->CreateNumberForResult( environment );
	return 0;
}

//=========================================================================================
void FunctionArgumentEvaluator::Argument( Evaluator* argument )
{
	this->argument = argument;
}

//=========================================================================================
Evaluator* FunctionArgumentEvaluator::Argument( void )
{
	return argument;
}

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( FunctionEvaluator, Evaluator );

//=========================================================================================
FunctionEvaluator::FunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ FunctionEvaluator::~FunctionEvaluator( void )
{
	listOfArguments.RemoveAll( true );
}

//=========================================================================================
// It is very important that we evaluate the arguments from left to right, because
// this is the expected behavior of the language.
bool FunctionEvaluator::EvaluateArguments( Number**& numberArray, int& numberArraySize, Environment& environment )
{
	bool success = false;

	do
	{
		// Some functions don't take any arguments.
		// If this one does, then go evaluate those arguments.
		numberArray = 0;
		numberArraySize = listOfArguments.Count();
		if( numberArraySize > 0 )
		{
			numberArray = new Number*[ numberArraySize ];
			memset( numberArray, 0, numberArraySize * sizeof( Number* ) );

			// Go populate the number array with the argument list evaluation results.
			FunctionArgumentEvaluator* evaluator = ( FunctionArgumentEvaluator* )listOfArguments.LeftMost();
			int index;
			for( index = 0; index < numberArraySize; index++ )
			{
				if( !evaluator )
					break;
				Number* number = environment.CreateNumber( this );
				numberArray[ index ] = number;
				if( !evaluator->EvaluateResult( *number, environment ) )
					break;
				evaluator = ( FunctionArgumentEvaluator* )evaluator->Right();
			}
			if( index < numberArraySize )
				break;
		}

		success = true;
	}
	while( false );

	// It's all or nothing.  If we failed, don't return anything.
	if( !success )
		DeleteArguments( numberArray, numberArraySize );

	return success;
}

//=========================================================================================
void FunctionEvaluator::DeleteArguments( Number**& numberArray, int& numberArraySize )
{
	if( numberArray )
	{
		for( int index = 0; index < numberArraySize; index++ )
			delete numberArray[ index ];
		delete[] numberArray;
		numberArray = 0;
	}
}

//=========================================================================================
void FunctionEvaluator::AddArgument( FunctionArgumentEvaluator* argument, InsertionType insertionType )
{
	if( insertionType == APPEND_TO_LIST_OF_ARGS )
		listOfArguments.InsertRightOf( listOfArguments.RightMost(), argument );
	else if( insertionType == PREPEND_TO_LIST_OF_ARGS )
		listOfArguments.InsertLeftOf( listOfArguments.LeftMost(), argument );
}

//=========================================================================================
FunctionArgumentEvaluator* FunctionEvaluator::GetArgument( int argumentIndex )
{
	FunctionArgumentEvaluator* argumentEvaluator = ( FunctionArgumentEvaluator* )listOfArguments[ argumentIndex ];
	return argumentEvaluator;
}

//=========================================================================================
int FunctionEvaluator::GetArgumentCount( void )
{
	return listOfArguments.Count();
}

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( StringEvaluator, Evaluator );

//=========================================================================================
StringEvaluator::StringEvaluator( void )
{
	string = 0;
}

//=========================================================================================
/*virtual*/ StringEvaluator::~StringEvaluator( void )
{
	if( string )
		delete[] string;
}

//=========================================================================================
/*virtual*/ Number* StringEvaluator::CreateNumberForResult( Environment& environment )
{
	StringNumber* stringNumber = new StringNumber();
	return stringNumber;
}

//=========================================================================================
/*virtual*/ bool StringEvaluator::EvaluateResult( Number& result, Environment& environment )
{
	if( !result.AssignFrom( string, environment ) )
		return false;
	return true;
}

//=========================================================================================
void StringEvaluator::String( const char* string )
{
	if( this->string )
		delete[] this->string;
	int stringLength = strlen( string ) + 1;
	this->string = new char[ stringLength ];
	strcpy_s( this->string, stringLength, string );
}

//=========================================================================================
const char* StringEvaluator::String( void ) const
{
	return string;
}

// Evaluator.cpp