// Function.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#include "Evaluator.h"

//=========================================================================================
class CalcLib::EnvironmentInformationFunctionEvaluator : public FunctionEvaluator
{
public:
	EnvironmentInformationFunctionEvaluator( void );
	virtual ~EnvironmentInformationFunctionEvaluator( void );
	virtual bool EvaluateResult( Number& result, Environment& environment );
};

//=========================================================================================
class CalcLib::DumpVariablesFunctionEvaluator : public FunctionEvaluator
{
public:
	DumpVariablesFunctionEvaluator( void );
	virtual ~DumpVariablesFunctionEvaluator( void );
	virtual bool EvaluateResult( Number& result, Environment& environment );
};

//=========================================================================================
class CalcLib::DeleteAllVariablesFunctionEvaluator : public FunctionEvaluator
{
public:
	DeleteAllVariablesFunctionEvaluator( void );
	virtual ~DeleteAllVariablesFunctionEvaluator( void );
	virtual bool EvaluateResult( Number& result, Environment& environment );
};

//=========================================================================================
// This function evaluator should never depend on the type of number being
// processed so that it can work with any type of number in any environment.
class CalcLib::TaylorSeriesFunctionEvaluator : public FunctionEvaluator
{
public:

	enum TaylorSeriesType
	{
		TAYLOR_SERIES_UNKNOWN,
		TAYLOR_SERIES_EXP,
		TAYLOR_SERIES_LN,
	};

	TaylorSeriesFunctionEvaluator( void );
	virtual ~TaylorSeriesFunctionEvaluator( void );
	virtual bool EvaluateResult( Number& result, Environment& environment );
	void TaylorSeries( TaylorSeriesType taylorSeries );
	void Center( const Number* number, Environment& environment );

private:

	Number* center;
	TaylorSeriesType taylorSeries;
};

// Function.h