// DumpInfoFunction.h

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
#include "../BindTarget.h"

//=========================================================================================
class GAVisToolDumpInfoFunctionEvaluator : public CalcLib::FunctionEvaluator
{
	DECLARE_CALCLIB_CLASS( GAVisToolDumpInfoFunctionEvaluator );

public:

	GAVisToolDumpInfoFunctionEvaluator( void );
	virtual ~GAVisToolDumpInfoFunctionEvaluator( void );
	virtual bool EvaluateResult( CalcLib::Number& result, CalcLib::Environment& environment );
};

// DumpInfoFunction.h