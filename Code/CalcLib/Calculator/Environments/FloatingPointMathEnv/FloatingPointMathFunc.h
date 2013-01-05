// FloatingPointMathFunc.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

//=========================================================================================
class CalcLib::FloatingPointMathFunctionEvaluator : public FunctionEvaluator
{
	DECLARE_CALCLIB_CLASS( FloatingPointMathFunctionEvaluator );

public:

	enum FunctionType
	{
		FUNC_UNKNOWN,
		FUNC_SQRT,
		FUNC_EXP,
		FUNC_LOG,
		FUNC_LN,
		FUNC_POW,
		FUNC_SIN,
		FUNC_COS,
		FUNC_TAN,
		FUNC_ASIN,
		FUNC_ACOS,
		FUNC_ATAN,
		FUNC_ATAN2,
		FUNC_CSC,
		FUNC_SEC,
		FUNC_COT,
		FUNC_ACSC,
		FUNC_ASEC,
		FUNC_ACOT,
	};

	FloatingPointMathFunctionEvaluator( void );
	virtual ~FloatingPointMathFunctionEvaluator( void );
	virtual bool EvaluateResult( Number& result, Environment& environment );
	void Function( FunctionType function );

private:

	FunctionType function;
};

// FloatingPointMathFunc.h