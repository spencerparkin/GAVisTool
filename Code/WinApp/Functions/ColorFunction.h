// ColorFunction.h

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
#include "../Geometry.h"

//=========================================================================================
class GAVisToolColorFunctionEvaluator : public CalcLib::FunctionEvaluator
{
	DECLARE_CALCLIB_CLASS( GAVisToolColorFunctionEvaluator );

public:

	GAVisToolColorFunctionEvaluator( void );
	virtual ~GAVisToolColorFunctionEvaluator( void );
	virtual bool EvaluateResult( CalcLib::Number& result, CalcLib::Environment& environment );

private:

	bool CalculateMagnitudeAndClamp( const CalcLib::MultivectorNumber& multivectorNumber, double& magnitude, CalcLib::Environment& environment );
};

// ColorFunction.h