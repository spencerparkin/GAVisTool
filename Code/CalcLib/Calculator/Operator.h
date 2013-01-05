// Operator.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

//=========================================================================================
// A string may get executed in a number context if the number/environment
// is string-aware.  If it is not string-aware, then the user can always
// pass strings to this unary operator which is string-aware.  It simply
// executes the given string and returns its result.  No number/environment
// really needs to be string-aware, though, if we provide this operator.
class CalcLib::UnaryStringExecutionEvaluator : public UnaryOperationEvaluator
{
	DECLARE_CALCLIB_CLASS( UnaryStringExecutionEvaluator );

public:

	UnaryStringExecutionEvaluator( void );
	virtual ~UnaryStringExecutionEvaluator( void );

	virtual bool EvaluateResult( Number& result, Environment& environment ) override;
};

//=========================================================================================
class CalcLib::UnaryArithmeticOperationEvaluator : public UnaryOperationEvaluator
{
	DECLARE_CALCLIB_CLASS( UnaryArithmeticOperationEvaluator );

public:

	enum ArithmeticOperation
	{
		ARITHMETIC_OPERATOR_UNKNOWN,
		ARITHMETIC_OPERATOR_NEGATE,
		ARITHMETIC_OPERATOR_INVERT,
	};

	UnaryArithmeticOperationEvaluator( ArithmeticOperation arithmeticOperation );
	virtual ~UnaryArithmeticOperationEvaluator( void );
	
	virtual bool EvaluateResult( Number& result, Environment& environment ) override;

protected:

	bool PerformOperation( const Number* number, Number& result, Environment& environment );

	ArithmeticOperation arithmeticOperation;
};

//=========================================================================================
class CalcLib::BinaryArithmeticOperationEvaluator : public BinaryOperationEvaluator
{
	DECLARE_CALCLIB_CLASS( BinaryArithmeticOperationEvaluator );

public:

	enum ArithmeticOperation
	{
		ARITHMETIC_OPERATION_UNKNOWN,
		ARITHMETIC_OPERATION_ADD,
		ARITHMETIC_OPERATION_SUBTRACT,
		ARITHMETIC_OPERATION_MULTIPLY,
		ARITHMETIC_OPERATION_DIVIDE,

		// Not sure if these are really arithmetic operations, but why not.
		ARITHMETIC_OPERATION_INNER_PRODUCT,
		ARITHMETIC_OPERATION_OUTER_PRODUCT,
	};

	BinaryArithmeticOperationEvaluator( ArithmeticOperation arithmeticOperation );
	virtual ~BinaryArithmeticOperationEvaluator( void );
	
	virtual bool EvaluateResult( Number& result, Environment& environment ) override;

protected:

	bool PerformOperation( const Number* leftNumber, const Number* rightNumber, Number& result, Environment& environment );

	ArithmeticOperation arithmeticOperation;
};

//=========================================================================================
class CalcLib::BinaryComparisonOperationEvaluator : public BinaryOperationEvaluator
{
	DECLARE_CALCLIB_CLASS( BinaryComparisonOperationEvaluator );

public:

	enum ComparisonOperation
	{
		COMPARE_OPERATION_UNKNOWN,
		COMPARE_OPERATION_LESS_THAN,
		COMPARE_OPERATION_GREATER_THAN,
		COMPARE_OPERATION_LESS_THAN_OR_EQUAL_TO,
		COMPARE_OPERATION_GREATER_THAN_OR_EQUAL_TO,
		COMPARE_OPERATION_EQUAL_TO,
	};

	BinaryComparisonOperationEvaluator( ComparisonOperation comparisonOperation );
	virtual ~BinaryComparisonOperationEvaluator( void );
	
	virtual bool EvaluateResult( Number& result, Environment& environment ) override;

private:
	ComparisonOperation comparisonOperation;
};

//=========================================================================================
class CalcLib::BinaryLogicOperationEvaluator : public BinaryOperationEvaluator
{
	DECLARE_CALCLIB_CLASS( BinaryLogicOperationEvaluator );

public:

	enum LogicOperation
	{
		LOGIC_OPERATION_UNKOWN,
		LOGIC_OPERATION_AND,
		LOGIC_OPERATION_INCLUSIVE_OR,
		LOGIC_OPERATION_EXCLUSIVE_OR,
	};

	BinaryLogicOperationEvaluator( LogicOperation logicOperation );
	virtual ~BinaryLogicOperationEvaluator( void );
	
	virtual bool EvaluateResult( Number& result, Environment& environment ) override;

private:
	LogicOperation logicOperation;
};

//=========================================================================================
class CalcLib::BinaryAssignmentOperationEvaluator : public BinaryOperationEvaluator
{
	DECLARE_CALCLIB_CLASS( BinaryAssignmentOperationEvaluator );

public:

	BinaryAssignmentOperationEvaluator( void );
	virtual ~BinaryAssignmentOperationEvaluator( void );
	
	virtual bool EvaluateResult( Number& result, Environment& environment ) override;
};

//=========================================================================================
class CalcLib::BinaryIndexingOperationEvaluator : public BinaryOperationEvaluator
{
	DECLARE_CALCLIB_CLASS( BinaryIndexingOperationEvaluator );

public:

	BinaryIndexingOperationEvaluator( void );
	virtual ~BinaryIndexingOperationEvaluator( void );
	
	virtual bool EvaluateResult( Number& result, Environment& environment ) override;
	virtual bool StoreResult( const Number& result, Environment& environment ) override;

private:

	bool FormulateVariableName( char* variableName, int variableNameSize, Environment& environment );
	bool FormulateLeftVariableName( char* variableName, int variableNameSize, Environment& environment );
	bool FormulateRightVariableName( char* variableName, int variableNameSize, Environment& environment );
	static bool FormulateLeftOrRightVariableName( char* variableName, int variableNameSize, Evaluator* operandEvaluator, Environment& environment );
};

// Operator.h