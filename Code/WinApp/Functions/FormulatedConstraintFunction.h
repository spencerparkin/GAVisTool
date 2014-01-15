// FormulatedConstraintFunction.h

/*
 * Copyright (C) 2013-2014 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#include "Calculator/CalcLib.h"
#include "../Environment.h"

//=========================================================================================
class GAVisToolFormulatedConstraintFunctionEvaluator : public CalcLib::FunctionEvaluator
{
	DECLARE_CALCLIB_CLASS( GAVisToolFormulatedConstraintFunctionEvaluator );

public:

	GAVisToolFormulatedConstraintFunctionEvaluator( void );
	virtual ~GAVisToolFormulatedConstraintFunctionEvaluator( void );
	virtual bool EvaluateResult( CalcLib::Number& result, CalcLib::Environment& environment );
};

// FormulatedConstraintFunction.h