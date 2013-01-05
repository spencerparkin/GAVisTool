// ReduceBivectorFunction.cpp

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "ReduceBivectorFunction.h"
#include "../Environment.h"
#include "../Application.h"

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( GAVisToolReduceBivectorFunctionEvaluator, FunctionEvaluator );

//=========================================================================================
GAVisToolReduceBivectorFunctionEvaluator::GAVisToolReduceBivectorFunctionEvaluator( void )
{
	CalcLib::Calculator calculator( "geoalg" );

	char reductionCode[ 2*1024 ];
	strcpy_s( reductionCode, sizeof( reductionCode ),
		"do("

		"c0 = scalar_part( bivector, no^nob ),"
		"c1 = scalar_part( bivector, no^e1b ),"
		"c2 = scalar_part( bivector, no^e2b ),"
		"c3 = scalar_part( bivector, no^e3b ),"
		"c4 = scalar_part( bivector, no^nib ),"

		"c5 = scalar_part( bivector, e1^nob ),"
		"c6 = scalar_part( bivector, e1^e1b ),"
		"c7 = scalar_part( bivector, e1^e2b ),"
		"c8 = scalar_part( bivector, e1^e3b ),"
		"c9 = scalar_part( bivector, e1^nib ),"
		
		"c10 = scalar_part( bivector, e2^nob ),"
		"c11 = scalar_part( bivector, e2^e1b ),"
		"c12 = scalar_part( bivector, e2^e2b ),"
		"c13 = scalar_part( bivector, e2^e3b ),"
		"c14 = scalar_part( bivector, e2^nib ),"
		
		"c15 = scalar_part( bivector, e3^nob ),"
		"c16 = scalar_part( bivector, e3^e1b ),"
		"c17 = scalar_part( bivector, e3^e2b ),"
		"c18 = scalar_part( bivector, e3^e3b ),"
		"c19 = scalar_part( bivector, e3^nib ),"
		
		"c20 = scalar_part( bivector, ni^nob ),"
		"c21 = scalar_part( bivector, ni^e1b ),"
		"c22 = scalar_part( bivector, ni^e2b ),"
		"c23 = scalar_part( bivector, ni^e3b ),"
		"c24 = scalar_part( bivector, ni^nib ),"

		// The result of the do() function should be the following expression.
		"	c0*no^nob + "
		"	(c1 + c5)*no^e1b + "
		"	(c2 + c10)*no^e2b + "
		"	(c3 + c15)*no^e2b + "
		"	(c4 + c20)*no^nib + "

		"	c6*e1^e1b + "
		"	(c7 + c11)*e1^e2b + "
		"	(c8 + c16)*e1^e3b + "
		"	(c9 + c23)*e1^nib + "

		"	c12*e2^e2b + "
		"	(c13 + c17)*e2^e3b + "
		"	(c14 + c22)*e2^nib + "

		"	c18*e3^e3b + "
		"	(c19 + c23)*e3^nib + "

		"	c24*ni^nib,"
		
		")"
	);

	reductionEvaluator = calculator.CompileEvaluator( reductionCode );
}

//=========================================================================================
/*virtual*/ GAVisToolReduceBivectorFunctionEvaluator::~GAVisToolReduceBivectorFunctionEvaluator( void )
{
	delete reductionEvaluator;
}

//=========================================================================================
/*virtual*/ bool GAVisToolReduceBivectorFunctionEvaluator::EvaluateResult( CalcLib::Number& result, CalcLib::Environment& environment )
{
	bool success = false;
	
	do
	{
		if( GetArgumentCount() != 1 )
		{
			environment.AddError( "The reduce-bivector function expects one and only one argument." );
			break;
		}

		CalcLib::FunctionArgumentEvaluator* argument = GetArgument(0);
		if( !argument->EvaluateResult( result, environment ) )
			break;
		
		CalcLib::GeometricAlgebraEnvironment gaEnv;
		if( !gaEnv.StoreVariable( "bivector", result ) )
			break;

		if( !reductionEvaluator->EvaluateResult( result, gaEnv ) )
			break;

		success = true;
	}
	while( false );

	return success;
}

// ReduceBivectorFunction.cpp