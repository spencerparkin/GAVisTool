// Calculus.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

//=========================================================================================
class CalcLib::CalculusFunctionEvaluator : public FunctionEvaluator
{
	DECLARE_CALCLIB_CLASS( CalculusFunctionEvaluator );

public:

	enum CalculusFunc
	{
		TAKE_DERIVATIVE,
		TAKE_ANTI_DERIVATIVE,
	};

	CalculusFunctionEvaluator( CalculusFunc calculusFunc );
	virtual ~CalculusFunctionEvaluator( void );
	virtual bool EvaluateResult( Number& result, Environment& environment );

private:

	CalculusFunc calculusFunc;
};

// Calculus.h