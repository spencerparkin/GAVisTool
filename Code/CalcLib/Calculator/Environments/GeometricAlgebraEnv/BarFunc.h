// BarFunc.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

//=========================================================================================
class CalcLib::BarFunctionEvaluator : public FunctionEvaluator
{
	DECLARE_CALCLIB_CLASS( BarFunctionEvaluator );
public:
	BarFunctionEvaluator( void );
	virtual ~BarFunctionEvaluator( void );
	virtual bool EvaluateResult( Number& result, Environment& environment );
};

// BarFunc.h