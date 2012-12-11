// PointFunction.cpp

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "PointFunction.h"
#include "../Environment.h"
#include "../Application.h"

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( GAVisToolPointFunctionEvaluator, FunctionEvaluator );

//=========================================================================================
GAVisToolPointFunctionEvaluator::GAVisToolPointFunctionEvaluator( void )
{
	CalcLib::Calculator calculator( "geoalg" );

	char evaluatorCode[ 256 ];
	strcpy_s( evaluatorCode, sizeof( evaluatorCode ), "no + point + 0.5*(point.point)*ni" );

	evaluator = calculator.CompileEvaluator( evaluatorCode );
}

//=========================================================================================
/*virtual*/ GAVisToolPointFunctionEvaluator::~GAVisToolPointFunctionEvaluator( void )
{
	delete evaluator;
}

//=========================================================================================
/*virtual*/ bool GAVisToolPointFunctionEvaluator::EvaluateResult( CalcLib::Number& result, CalcLib::Environment& environment )
{
	bool success = false;
	
	do
	{
		if( GetArgumentCount() != 1 )
			break;

		CalcLib::FunctionArgumentEvaluator* argument = GetArgument(0);
		if( !argument->EvaluateResult( result, environment ) )
			break;
		
		CalcLib::GeometricAlgebraEnvironment gaEnv;
		if( !gaEnv.StoreVariable( "point", result ) )
			break;

		if( !evaluator->EvaluateResult( result, gaEnv ) )
			break;

		success = true;
	}
	while( false );

	return success;
}

// PointFunction.cpp