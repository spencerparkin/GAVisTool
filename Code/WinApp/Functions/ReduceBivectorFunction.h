// ReduceBivectorFunction.h

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#include "Calculator/CalcLib.h"

//=========================================================================================
class GAVisToolReduceBivectorFunctionEvaluator : public CalcLib::FunctionEvaluator
{
	DECLARE_CALCLIB_CLASS( GAVisToolReduceBivectorFunctionEvaluator );
public:
	GAVisToolReduceBivectorFunctionEvaluator( void );
	virtual ~GAVisToolReduceBivectorFunctionEvaluator( void );
	virtual bool EvaluateResult( CalcLib::Number& result, CalcLib::Environment& environment );
private:
	CalcLib::Evaluator* reductionEvaluator;
};

// ReduceBivectorFunction.h