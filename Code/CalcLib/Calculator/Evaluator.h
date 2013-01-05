// Evaluator.h

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
// An evalutor is simply something that evaluates to a numerical result.
// Some evaluators, namely variables, can store a numerical result.
// Evaluators evaluate in the context of the given environment.
class CalcLib::Evaluator : public CalcLibClass
{
	DECLARE_CALCLIB_CLASS( Evaluator );
public:
	Evaluator( void );
	virtual ~Evaluator( void );
	virtual bool EvaluateResult( Number& result, Environment& environment ) = 0;
	virtual bool StoreResult( const Number& result, Environment& environment );
	virtual Number* CreateNumberForResult( Environment& environment );
};

//=========================================================================================
class CalcLib::UnaryOperationEvaluator : public Evaluator
{
	DECLARE_CALCLIB_CLASS( UnaryOperationEvaluator );
public:
	UnaryOperationEvaluator( void );
	virtual ~UnaryOperationEvaluator( void );
	virtual bool EvaluateResult( Number& result, Environment& environment ) = 0;
	bool EvaluateOperand( Number*& number, Environment& environment );
	void Operand( Evaluator* operand );
protected:
	Evaluator* operand;
};

//=========================================================================================
class CalcLib::BinaryOperationEvaluator : public Evaluator
{
	DECLARE_CALCLIB_CLASS( BinaryOperationEvaluator );
public:
	BinaryOperationEvaluator( void );
	virtual ~BinaryOperationEvaluator( void );
	virtual bool EvaluateResult( Number& result, Environment& environment ) = 0;
	bool EvaluateOperands( Number*& leftNumber, Number*& rightNumber, Environment& environment );
	void LeftOperand( Evaluator* leftOperand );
	void RightOperand( Evaluator* rightOperand );
protected:
	Evaluator* leftOperand;
	Evaluator* rightOperand;
};

//=========================================================================================
class CalcLib::FunctionArgumentEvaluator : public Evaluator, public Utilities::List::Item
{
	DECLARE_CALCLIB_CLASS( FunctionArgumentEvaluator );
public:
	FunctionArgumentEvaluator( void );
	virtual ~FunctionArgumentEvaluator( void );
	virtual bool EvaluateResult( Number& result, Environment& environment );
	virtual Number* CreateNumberForResult( Environment& environment );
	void Argument( Evaluator* argument );
	Evaluator* Argument( void );
protected:
	Evaluator* argument;
};

//=========================================================================================
class CalcLib::FunctionEvaluator : public Evaluator
{
	DECLARE_CALCLIB_CLASS( FunctionEvaluator );
public:

	enum InsertionType
	{
		APPEND_TO_LIST_OF_ARGS,
		PREPEND_TO_LIST_OF_ARGS,
	};

	FunctionEvaluator( void );
	virtual ~FunctionEvaluator( void );
	virtual bool EvaluateResult( Number& result, Environment& environment ) = 0;
	bool EvaluateArguments( Number**& numberArray, int& numberArraySize, Environment& environment );
	void DeleteArguments( Number**& numberArray, int& numberArraySize );
	void AddArgument( FunctionArgumentEvaluator* argument, InsertionType insertionType );
	FunctionArgumentEvaluator* GetArgument( int argumentIndex );
	int GetArgumentCount( void );
protected:
	Utilities::List listOfArguments;
};

//=========================================================================================
class CalcLib::StringEvaluator : public Evaluator
{
	DECLARE_CALCLIB_CLASS( StringEvaluator );
public:
	StringEvaluator( void );
	virtual ~StringEvaluator( void );
	virtual bool EvaluateResult( Number& result, Environment& environment );
	virtual Number* CreateNumberForResult( Environment& environment );
	void String( const char* string );
	const char* String( void ) const;
private:
	char* string;
};

// Evaluator.h