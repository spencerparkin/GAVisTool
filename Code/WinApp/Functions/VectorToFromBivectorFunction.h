// VectorToFromBivectorFunction.h

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

//=========================================================================================
class GAVisToolVectorToFromBivectorFunctionEvaluator : public CalcLib::FunctionEvaluator
{
	DECLARE_CALCLIB_CLASS( GAVisToolVectorToFromBivectorFunctionEvaluator );
public:
	enum FuncType
	{
		CONVERT_VECTOR_TO_BIVECTOR,
		CONVERT_VECTOR_FROM_BIVECTOR,
	};
	GAVisToolVectorToFromBivectorFunctionEvaluator( FuncType funcType );
	virtual ~GAVisToolVectorToFromBivectorFunctionEvaluator( void );
	virtual bool EvaluateResult( CalcLib::Number& result, CalcLib::Environment& environment );
private:
	FuncType funcType;
	CalcLib::Evaluator* conversionEvaluator;
};

// VectorToFromBivectorFunction.h