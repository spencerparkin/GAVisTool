// MultivectorMathFunc.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

//=========================================================================================
class CalcLib::MultivectorMathFunctionEvaluator : public FunctionEvaluator
{
	DECLARE_CALCLIB_CLASS( MultivectorMathFunctionEvaluator );

public:

	enum FunctionType
	{
		FUNC_EXP,
		FUNC_LOG,
		FUNC_POW,
		FUNC_SQRT,
		FUNC_COS,
		FUNC_SIN,
		FUNC_TAN,
		FUNC_ABS,
		FUNC_ACOS,
		FUNC_ASIN,
	};

	MultivectorMathFunctionEvaluator( FunctionType functionType );
	virtual ~MultivectorMathFunctionEvaluator( void );
	virtual bool EvaluateResult( Number& result, Environment& environment );

private:

	FunctionType functionType;

	bool EvaluateScalarArguments( double**& scalarNumberArray, Number** numberArray, int numberArraySize, Environment& environment );
	void DeleteScalarArguments( double**& scalarNumberArray, int numberArraySize );
};

// MultivectorMathFunc.h