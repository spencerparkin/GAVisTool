// Language.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

//=========================================================================================
// This function does nothing more than evaluate its arguments.
// All functions evaluate their arguments left to right, so this
// simply executes a list of statements from top to bottom.  The
// result is always that of the last argument.
class CalcLib::DoFunctionEvaluator : public FunctionEvaluator
{
	DECLARE_CALCLIB_CLASS( DoFunctionEvaluator );

public:
	DoFunctionEvaluator( void );
	virtual ~DoFunctionEvaluator( void );
	virtual bool EvaluateResult( Number& result, Environment& environment );
};

//=========================================================================================
class CalcLib::IfFunctionEvaluator : public FunctionEvaluator
{
	DECLARE_CALCLIB_CLASS( IfFunctionEvaluator );

public:
	IfFunctionEvaluator( void );
	virtual ~IfFunctionEvaluator( void );
	virtual bool EvaluateResult( Number& result, Environment& environment );
};

//=========================================================================================
class CalcLib::LoopFunctionEvaluator : public FunctionEvaluator
{
	DECLARE_CALCLIB_CLASS( LoopFunctionEvaluator );

public:

	enum LoopType
	{
		LOOP_WHILE,
		LOOP_UNTIL,
	};

	LoopFunctionEvaluator( LoopType loopType );
	virtual ~LoopFunctionEvaluator( void );
	virtual bool EvaluateResult( Number& result, Environment& environment );

private:

	LoopType loopType;
};

// Language.h