// PointFunction.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#include "Calculator/CalcLib.h"

//=========================================================================================
class GAVisToolPointFunctionEvaluator : public CalcLib::FunctionEvaluator
{
	DECLARE_CALCLIB_CLASS( GAVisToolPointFunctionEvaluator );
public:
	GAVisToolPointFunctionEvaluator( void );
	virtual ~GAVisToolPointFunctionEvaluator( void );
	virtual bool EvaluateResult( CalcLib::Number& result, CalcLib::Environment& environment );
private:
	CalcLib::Evaluator* evaluator;
};

// PointFunction.h