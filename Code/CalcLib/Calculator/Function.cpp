// Function.cpp

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "CalcLib.h"

using namespace CalcLib;

//=========================================================================================
EnvironmentInformationFunctionEvaluator::EnvironmentInformationFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ EnvironmentInformationFunctionEvaluator::~EnvironmentInformationFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ bool EnvironmentInformationFunctionEvaluator::EvaluateResult( Number& result, Environment& environment )
{
	result.AssignAdditiveIdentity( environment );
	environment.PrintEnvironmentInfo();
	return true;
}

//=========================================================================================
DumpVariablesFunctionEvaluator::DumpVariablesFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ DumpVariablesFunctionEvaluator::~DumpVariablesFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ bool DumpVariablesFunctionEvaluator::EvaluateResult( Number& result, Environment& environment )
{
	environment.DumpVariables();
	return true;
}

//=========================================================================================
DeleteAllVariablesFunctionEvaluator::DeleteAllVariablesFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ DeleteAllVariablesFunctionEvaluator::~DeleteAllVariablesFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ bool DeleteAllVariablesFunctionEvaluator::EvaluateResult( Number& result, Environment& environment )
{
	environment.DeleteAllVariables();
	return true;
}

//=========================================================================================
TaylorSeriesFunctionEvaluator::TaylorSeriesFunctionEvaluator( void )
{
	center = 0;
	taylorSeries = TAYLOR_SERIES_UNKNOWN;
}

//=========================================================================================
/*virtual*/ TaylorSeriesFunctionEvaluator::~TaylorSeriesFunctionEvaluator( void )
{
	if( center )
		delete center;
}

//=========================================================================================
// Here we support the evaluation of a Taylor series in any environment,
// since all environments support addition, multiplication, subtraction, etc.
/*virtual*/ bool TaylorSeriesFunctionEvaluator::EvaluateResult( Number& result, Environment& environment )
{
	// TODO: Write this.

	// TODO: Maybe delete this function evaluator and create a utility that can do Tayler series with the Number class.

	return false;
}

//=========================================================================================
void TaylorSeriesFunctionEvaluator::Center( const Number* number, Environment& environment )
{
	if( center )
		delete center;
	center = environment.CreateNumber(0);
	center->AssignFrom( number, environment );
}

//=========================================================================================
void TaylorSeriesFunctionEvaluator::TaylorSeries( TaylorSeriesType taylorSeries )
{
	this->taylorSeries = taylorSeries;
}

// Function.cpp