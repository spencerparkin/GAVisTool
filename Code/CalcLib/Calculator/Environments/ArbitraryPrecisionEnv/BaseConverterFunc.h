// BaseConverterFunc.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

//=========================================================================================
class CalcLib::BaseConverterFunctionEvaluator : public FunctionEvaluator
{
	DECLARE_CALCLIB_CLASS( BaseConverterFunctionEvaluator );
public:
	BaseConverterFunctionEvaluator( void );
	virtual ~BaseConverterFunctionEvaluator( void );
	virtual bool EvaluateResult( Number& result, Environment& environment );
};

// BaseConverterFunc.h