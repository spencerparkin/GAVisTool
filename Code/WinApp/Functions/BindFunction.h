// BindFunction.h

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
#include "../Geometry.h"
#include "../Interface.h"
#include "../Environment.h"

//=========================================================================================
class GAVisToolBindFunctionEvaluator : public CalcLib::FunctionEvaluator
{
	DECLARE_CALCLIB_CLASS( GAVisToolBindFunctionEvaluator );

public:

	GAVisToolBindFunctionEvaluator( GAVisToolBindTarget::CreationFunction creationFunction, GAVisToolBindTarget::BindType bindType );
	virtual ~GAVisToolBindFunctionEvaluator( void );
	virtual bool EvaluateResult( CalcLib::Number& result, CalcLib::Environment& environment );

private:

	GAVisToolGeometry::CreationFunction creationFunction;
	GAVisToolGeometry::BindType bindType;

	bool Unbind( GAVisToolEnvironment* visToolEnv, CalcLib::VariableEvaluator* variableEvaluator );
	bool Bind( GAVisToolEnvironment* visToolEnv, CalcLib::VariableEvaluator* variableEvaluator, CalcLib::MultivectorNumber* multivectorNumber );
};

// BindFunction.h