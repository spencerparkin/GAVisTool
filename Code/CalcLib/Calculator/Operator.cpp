// Operator.cpp

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
IMPLEMENT_CALCLIB_CLASS1( UnaryStringExecutionEvaluator, UnaryOperationEvaluator );

//=========================================================================================
UnaryStringExecutionEvaluator::UnaryStringExecutionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ UnaryStringExecutionEvaluator::~UnaryStringExecutionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ bool UnaryStringExecutionEvaluator::EvaluateResult( Number& result, Environment& environment )
{
	bool success = false;

	Number* number = 0;

	do
	{
		if( !EvaluateOperand( number, environment ) )
		{
			environment.AddError( "Failed to evaluate unary operator operand." );
			break;
		}

		if( number->IsTypeOf( StringNumber::ClassName() ) )
		{
			StringNumber* stringNumber = ( StringNumber* )number;
			if( !stringNumber->Execute( result, environment ) )
			{
				environment.AddError( "Failed to execute string argument of unary '#' operator." );
				break;
			}
		}
		else
		{
			if( !result.AssignFrom( number, environment ) )
				break;
		}

		success = true;
	}
	while( false );

	delete number;
	
	return success;
}

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( UnaryArithmeticOperationEvaluator, UnaryOperationEvaluator );

//=========================================================================================
UnaryArithmeticOperationEvaluator::UnaryArithmeticOperationEvaluator( ArithmeticOperation arithmeticOperation )
{
	this->arithmeticOperation = arithmeticOperation;
}

//=========================================================================================
/*virtual*/ UnaryArithmeticOperationEvaluator::~UnaryArithmeticOperationEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ bool UnaryArithmeticOperationEvaluator::EvaluateResult( Number& result, Environment& environment )
{
	bool success = false;

	Number* number = 0;

	do
	{
		if( !EvaluateOperand( number, environment ) )
		{
			environment.AddError( "Failed to evaluate unary operator operand." );
			break;
		}

		// Now perform the operation we are called upon to perform.
		if( !PerformOperation( number, result, environment ) )
			break;

		// We made it through the gauntlet!
		success = true;
	}
	while( false );

	delete number;

	return success;
}

//=========================================================================================
bool UnaryArithmeticOperationEvaluator::PerformOperation( const Number* number, Number& result, Environment& environment )
{
	bool operationPerformed = false;

	switch( arithmeticOperation )
	{
		case ARITHMETIC_OPERATOR_NEGATE:
		{
			operationPerformed = result.AssignAdditiveInverse( number, environment );
			break;
		}
		case ARITHMETIC_OPERATOR_INVERT:
		{
			operationPerformed = result.AssignMultilicativeInverse( number, environment );
			break;
		}
		default:
		{
			operationPerformed = false;
			environment.AddError( "Unknown unary arithmetic operation: %d.", arithmeticOperation );
			break;
		}
	}

	return operationPerformed;
}

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( BinaryArithmeticOperationEvaluator, BinaryOperationEvaluator );

//=========================================================================================
BinaryArithmeticOperationEvaluator::BinaryArithmeticOperationEvaluator( ArithmeticOperation arithmeticOperation )
{
	this->arithmeticOperation = arithmeticOperation;
}

//=========================================================================================
/*virtual*/ BinaryArithmeticOperationEvaluator::~BinaryArithmeticOperationEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ bool BinaryArithmeticOperationEvaluator::EvaluateResult( Number& result, Environment& environment )
{
	bool success = false;

	Number* leftNumber = 0;
	Number* rightNumber = 0;

	do
	{
		if( !EvaluateOperands( leftNumber, rightNumber, environment ) )
		{
			environment.AddError( "Failed to evaluate binary operator operands." );
			break;
		}

		// Now combine our operands in the operation we're called upon to perform.
		if( !PerformOperation( leftNumber, rightNumber, result, environment ) )
			break;

		// We made it through the gauntlet!
		success = true;
	}
	while( false );

	delete leftNumber;
	delete rightNumber;
	
	return success;
}

//=========================================================================================
bool BinaryArithmeticOperationEvaluator::PerformOperation( const Number* leftNumber, const Number* rightNumber, Number& result, Environment& environment )
{
	bool operationPerformed = false;

	switch( arithmeticOperation )
	{
		case ARITHMETIC_OPERATION_ADD:
		{
			operationPerformed = result.AssignSum( leftNumber, rightNumber, environment );
			break;
		}
		case ARITHMETIC_OPERATION_SUBTRACT:
		{
			operationPerformed = result.AssignDifference( leftNumber, rightNumber, environment );
			break;
		}
		case ARITHMETIC_OPERATION_MULTIPLY:
		{
			operationPerformed = result.AssignProduct( leftNumber, rightNumber, environment );
			break;
		}
		case ARITHMETIC_OPERATION_DIVIDE:
		{
			operationPerformed = result.AssignQuotient( leftNumber, rightNumber, environment );
			break;
		}
		case ARITHMETIC_OPERATION_INNER_PRODUCT:
		{
			operationPerformed = result.AssignInnerProduct( leftNumber, rightNumber, environment );
			break;
		}
		case ARITHMETIC_OPERATION_OUTER_PRODUCT:
		{
			operationPerformed = result.AssignOuterProduct( leftNumber, rightNumber, environment );
			break;
		}
		default:
		{
			operationPerformed = false;
			environment.AddError( "Unknown binary arithmetic operation: %d.", arithmeticOperation );
			break;
		}
	}

	return operationPerformed;
}

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( BinaryComparisonOperationEvaluator, BinaryOperationEvaluator );

//=========================================================================================
BinaryComparisonOperationEvaluator::BinaryComparisonOperationEvaluator( ComparisonOperation comparisonOperation )
{
	this->comparisonOperation = comparisonOperation;
}

//=========================================================================================
/*virtual*/ BinaryComparisonOperationEvaluator::~BinaryComparisonOperationEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ bool BinaryComparisonOperationEvaluator::EvaluateResult( Number& result, Environment& environment )
{
	bool success = false;

	Number* leftNumber = 0;
	Number* rightNumber = 0;

	do
	{
		if( !EvaluateOperands( leftNumber, rightNumber, environment ) )
		{
			environment.AddError( "Failed to evaluate binary operator operands." );
			break;
		}

		Number::Comparison comparison;
		if( !leftNumber->CompareWith( rightNumber, comparison, environment ) )
		{
			environment.AddError( "Failed to compare left argument with right argument." );
			break;
		}

		bool operationPerformed = false;
		switch( comparisonOperation )
		{
			case COMPARE_OPERATION_EQUAL_TO:
			{
				if( comparison == Number::IS_EQUAL_TO )
					operationPerformed = result.AssignMultiplicativeIdentity( environment );
				else
					operationPerformed = result.AssignAdditiveIdentity( environment );
				break;
			}
			case COMPARE_OPERATION_LESS_THAN:
			{
				if( comparison == Number::IS_LESS_THAN )
					operationPerformed = result.AssignMultiplicativeIdentity( environment );
				else
					operationPerformed = result.AssignAdditiveIdentity( environment );
				break;
			}
			case COMPARE_OPERATION_LESS_THAN_OR_EQUAL_TO:
			{
				if( comparison == Number::IS_LESS_THAN || comparison == Number::IS_EQUAL_TO )
					operationPerformed = result.AssignMultiplicativeIdentity( environment );
				else
					operationPerformed = result.AssignAdditiveIdentity( environment );
				break;
			}
			case COMPARE_OPERATION_GREATER_THAN:
			{
				if( comparison == Number::IS_GREATER_THAN )
					operationPerformed = result.AssignMultiplicativeIdentity( environment );
				else
					operationPerformed = result.AssignAdditiveIdentity( environment );
				break;
			}
			case COMPARE_OPERATION_GREATER_THAN_OR_EQUAL_TO:
			{
				if( comparison == Number::IS_GREATER_THAN || comparison == Number::IS_EQUAL_TO )
					operationPerformed = result.AssignMultiplicativeIdentity( environment );
				else
					operationPerformed = result.AssignAdditiveIdentity( environment );
				break;
			}
			default:
			{
				operationPerformed = false;
				environment.AddError( "Unknown binary comparison operation: %d.", comparisonOperation );
				break;
			}
		}

		if( !operationPerformed )
			break;

		// We made it through the gauntlet!
		success = true;
	}
	while( false );

	delete leftNumber;
	delete rightNumber;
	
	return success;
}

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( BinaryLogicOperationEvaluator, BinaryOperationEvaluator );

//=========================================================================================
BinaryLogicOperationEvaluator::BinaryLogicOperationEvaluator( LogicOperation logicOperation )
{
	this->logicOperation = logicOperation;
}

//=========================================================================================
/*virtual*/ BinaryLogicOperationEvaluator::~BinaryLogicOperationEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ bool BinaryLogicOperationEvaluator::EvaluateResult( Number& result, Environment& environment )
{
	bool success = false;

	Number* leftNumber = 0;
	Number* rightNumber = 0;

	do
	{
		if( !EvaluateOperands( leftNumber, rightNumber, environment ) )
		{
			environment.AddError( "Failed to evaluate binary operator operands." );
			break;
		}

		bool leftIsAdditiveIdentity;
		if( !leftNumber->IsAdditiveIdentity( leftIsAdditiveIdentity, environment ) )
			return false;

		bool rightIsAdditiveIdentity;
		if( !rightNumber->IsAdditiveIdentity( rightIsAdditiveIdentity, environment ) )
			return false;

		bool booleanResult = false;
		bool operationPerformed = false;
		switch( logicOperation )
		{
			case LOGIC_OPERATION_AND:
			{
				booleanResult = !( leftIsAdditiveIdentity || rightIsAdditiveIdentity );
				operationPerformed = true;
				break;
			}
			case LOGIC_OPERATION_INCLUSIVE_OR:
			{
				booleanResult = !( leftIsAdditiveIdentity && rightIsAdditiveIdentity );
				operationPerformed = true;
				break;
			}
			case LOGIC_OPERATION_EXCLUSIVE_OR:
			{
				booleanResult = !( leftIsAdditiveIdentity && rightIsAdditiveIdentity ) && ( leftIsAdditiveIdentity != rightIsAdditiveIdentity );
				operationPerformed = true;
				break;
			}
			default:
			{
				operationPerformed = false;
				environment.AddError( "Unknown binary logic operation: %d.", logicOperation );
				break;
			}
		}

		if( !operationPerformed )
			break;

		if( booleanResult )
		{
			if( !result.AssignMultiplicativeIdentity( environment ) )
				break;
		}
		else
		{
			if( !result.AssignAdditiveIdentity( environment ) )
				break;
		}

		// We made it through the gauntlet!
		success = true;
	}
	while( false );

	delete leftNumber;
	delete rightNumber;
	
	return success;
}

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( BinaryAssignmentOperationEvaluator, BinaryOperationEvaluator );

//=========================================================================================
BinaryAssignmentOperationEvaluator::BinaryAssignmentOperationEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ BinaryAssignmentOperationEvaluator::~BinaryAssignmentOperationEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ bool BinaryAssignmentOperationEvaluator::EvaluateResult( Number& result, Environment& environment )
{
	bool success = false;
	Number* assignmentNumber = 0;

	do
	{
		if( !result.AssignAdditiveIdentity( environment ) )
			break;

		if( !rightOperand )
		{
			environment.AddError( "Assignment operator has nothing to assign." );
			break;
		}

		if( !leftOperand )
		{
			environment.AddError( "Assignment operator has nothing to be assigned." );
			break;
		}

		// Notice that the right operand evaluator has a chance to dictate what type of number we create.
		assignmentNumber = environment.CreateNumber( rightOperand );
		if( !assignmentNumber )
		{
			environment.AddError( "Assignment operator failed to a create number from the environment." );
			break;
		}

		if( !rightOperand->EvaluateResult( *assignmentNumber, environment ) )
		{
			environment.AddError( "Assignment operator failed to evaluate its right operand." );
			break;
		}

		if( !leftOperand->StoreResult( *assignmentNumber, environment ) )
		{
			environment.AddError( "Assignment operator failed to assign its right operand to its left operand." );
			break;
		}

		if( !assignmentNumber->IsTypeOf( StringNumber::ClassName() ) )
		{
			if( !result.AssignFrom( assignmentNumber, environment ) )
				break;
		}

		success = true;
	}
	while( false );

	delete assignmentNumber;

	return success;
}

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( BinaryIndexingOperationEvaluator, BinaryOperationEvaluator );

//=========================================================================================
BinaryIndexingOperationEvaluator::BinaryIndexingOperationEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ BinaryIndexingOperationEvaluator::~BinaryIndexingOperationEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ bool BinaryIndexingOperationEvaluator::EvaluateResult( Number& result, Environment& environment )
{
	if( !result.AssignAdditiveIdentity( environment ) )
		return false;

	char variableName[ 512 ];
	if( !FormulateVariableName( variableName, sizeof( variableName ), environment ) )
		return false;

	VariableEvaluator variableEvaluator( variableName );
	if( !variableEvaluator.EvaluateResult( result, environment ) )
		return false;

	return true;
}

//=========================================================================================
/*virtual*/ bool BinaryIndexingOperationEvaluator::StoreResult( const Number& result, Environment& environment )
{
	char variableName[ 512 ];
	if( !FormulateVariableName( variableName, sizeof( variableName ), environment ) )
		return false;

	VariableEvaluator variableEvaluator( variableName );
	if( !variableEvaluator.StoreResult( result, environment ) )
		return false;

	return true;
}

//=========================================================================================
bool BinaryIndexingOperationEvaluator::FormulateVariableName( char* variableName, int variableNameSize, Environment& environment )
{
	char leftVariableName[ 512 ];
	char rightVariableName[ 512 ];

	// It's important to realize that we treat the left argument differently than the right.
	if( !FormulateLeftVariableName( leftVariableName, sizeof( leftVariableName ), environment ) )
		return false;
	if( !FormulateRightVariableName( rightVariableName, sizeof( rightVariableName ), environment ) )
		return false;

	strcpy_s( variableName, variableNameSize, leftVariableName );
	strcat_s( variableName, variableNameSize, rightVariableName );
	return true;
}

//=========================================================================================
bool BinaryIndexingOperationEvaluator::FormulateLeftVariableName( char* variableName, int variableNameSize, Environment& environment )
{
	if( leftOperand->IsTypeOf( VariableEvaluator::ClassName() ) )
	{
		VariableEvaluator* variableEvaluator = ( VariableEvaluator* )leftOperand;
		strcpy_s( variableName, variableNameSize, variableEvaluator->Name() );
	}
	else
	{
		if( !FormulateLeftOrRightVariableName( variableName, variableNameSize, leftOperand, environment ) )
			return false;
	}

	return true;
}

//=========================================================================================
bool BinaryIndexingOperationEvaluator::FormulateRightVariableName( char* variableName, int variableNameSize, Environment& environment )
{
	if( !FormulateLeftOrRightVariableName( variableName, variableNameSize, rightOperand, environment ) )
		return false;

	return true;
}

//=========================================================================================
/*static*/ bool BinaryIndexingOperationEvaluator::FormulateLeftOrRightVariableName( char* variableName, int variableNameSize, Evaluator* operandEvaluator, Environment& environment )
{
	bool success = false;

	if( operandEvaluator->IsTypeOf( StringEvaluator::ClassName() ) )
	{
		StringEvaluator* stringEvaluator = ( StringEvaluator* )operandEvaluator;
		strcpy_s( variableName, variableNameSize, stringEvaluator->String() );
		success = true;
	}
	else if( operandEvaluator->IsTypeOf( BinaryIndexingOperationEvaluator::ClassName() ) )
	{
		BinaryIndexingOperationEvaluator* indexingEvaluator = ( BinaryIndexingOperationEvaluator* )operandEvaluator;
		if( indexingEvaluator->FormulateVariableName( variableName, variableNameSize, environment ) )
			success = true;
	}
	else
	{
		Number* number = 0;
		
		do
		{
			number = environment.CreateNumber( operandEvaluator );
			if( !number )
				break;

			if( !operandEvaluator->EvaluateResult( *number, environment ) )
				break;

			if( !number->Print( variableName, variableNameSize, false, environment ) )
				break;

			success = true;
		}
		while( false );

		if( number )
			delete number;
	}

	return success;
}

// Operator.cpp