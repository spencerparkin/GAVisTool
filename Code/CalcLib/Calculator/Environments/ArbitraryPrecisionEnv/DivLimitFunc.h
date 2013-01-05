// DivLimitFunc.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

//=========================================================================================
class CalcLib::DivisionLimitFunctionEvaluator : public FunctionEvaluator
{
	DECLARE_CALCLIB_CLASS( DivisionLimitFunctionEvaluator );
public:
	DivisionLimitFunctionEvaluator( void );
	virtual ~DivisionLimitFunctionEvaluator( void );
	virtual bool EvaluateResult( Number& result, Environment& environment );
};

// DivLimitFunc.h