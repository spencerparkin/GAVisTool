// VectorToFromBivectorFunction.cpp

/*
 * Copyright (C) 2013-2014 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "VectorToFromBivectorFunction.h"
#include "../Environment.h"
#include "../Application.h"

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( GAVisToolVectorToFromBivectorFunctionEvaluator, FunctionEvaluator );

//=========================================================================================
GAVisToolVectorToFromBivectorFunctionEvaluator::GAVisToolVectorToFromBivectorFunctionEvaluator( FuncType funcType )
{
	this->funcType = funcType;

	CalcLib::Calculator calculator( "geoalg" );

	char conversionCode[ 5*1024 ];
	conversionCode[0] = '\0';

	if( funcType == CONVERT_VECTOR_TO_BIVECTOR )
	{
		strcpy_s( conversionCode, sizeof( conversionCode ),

			"scalar_part( element, no_nob )*no^nob + "
			"scalar_part( element, no_e1b )*no^e1b + "
			"scalar_part( element, no_e2b )*no^e2b + "
			"scalar_part( element, no_e3b )*no^e3b + "
			"scalar_part( element, no_nib )*no^nib + "

			"scalar_part( element, e1_nob )*e1^nob + "
			"scalar_part( element, e1_e1b )*e1^e1b + "
			"scalar_part( element, e1_e2b )*e1^e2b + "
			"scalar_part( element, e1_e3b )*e1^e3b + "
			"scalar_part( element, e1_nib )*e1^nib + "

			"scalar_part( element, e2_nob )*e2^nob + "
			"scalar_part( element, e2_e1b )*e2^e1b + "
			"scalar_part( element, e2_e2b )*e2^e2b + "
			"scalar_part( element, e2_e3b )*e2^e3b + "
			"scalar_part( element, e2_nib )*e2^nib + "

			"scalar_part( element, e3_nob )*e3^nob + "
			"scalar_part( element, e3_e1b )*e3^e1b + "
			"scalar_part( element, e3_e2b )*e3^e2b + "
			"scalar_part( element, e3_e3b )*e3^e3b + "
			"scalar_part( element, e3_nib )*e3^nib + "

			"scalar_part( element, ni_nob )*ni^nob + "
			"scalar_part( element, ni_e1b )*ni^e1b + "
			"scalar_part( element, ni_e2b )*ni^e2b + "
			"scalar_part( element, ni_e3b )*ni^e3b + "
			"scalar_part( element, ni_nib )*ni^nib"
		);
	}
	else if( funcType == CONVERT_VECTOR_FROM_BIVECTOR )
	{
		strcpy_s( conversionCode, sizeof( conversionCode ),

			"scalar_part( element, no^nob )*no_nob + "
			"scalar_part( element, no^e1b )*no_e1b + "
			"scalar_part( element, no^e2b )*no_e2b + "
			"scalar_part( element, no^e3b )*no_e3b + "
			"scalar_part( element, no^nib )*no_nib + "

			"scalar_part( element, e1^nob )*e1_nob + "
			"scalar_part( element, e1^e1b )*e1_e1b + "
			"scalar_part( element, e1^e2b )*e1_e2b + "
			"scalar_part( element, e1^e3b )*e1_e3b + "
			"scalar_part( element, e1^nib )*e1_nib + "

			"scalar_part( element, e2^nob )*e2_nob + "
			"scalar_part( element, e2^e1b )*e2_e1b + "
			"scalar_part( element, e2^e2b )*e2_e2b + "
			"scalar_part( element, e2^e3b )*e2_e3b + "
			"scalar_part( element, e2^nib )*e2_nib + "

			"scalar_part( element, e3^nob )*e3_nob + "
			"scalar_part( element, e3^e1b )*e3_e1b + "
			"scalar_part( element, e3^e2b )*e3_e2b + "
			"scalar_part( element, e3^e3b )*e3_e3b + "
			"scalar_part( element, e3^nib )*e3_nib + "

			"scalar_part( element, ni^nob )*ni_nob + "
			"scalar_part( element, ni^e1b )*ni_e1b + "
			"scalar_part( element, ni^e2b )*ni_e2b + "
			"scalar_part( element, ni^e3b )*ni_e3b + "
			"scalar_part( element, ni^nib )*ni_nib"
		);
	}

	conversionEvaluator = calculator.CompileEvaluator( conversionCode );
}

//=========================================================================================
/*virtual*/ GAVisToolVectorToFromBivectorFunctionEvaluator::~GAVisToolVectorToFromBivectorFunctionEvaluator( void )
{
	delete conversionEvaluator;
}

//=========================================================================================
/*virtual*/ bool GAVisToolVectorToFromBivectorFunctionEvaluator::EvaluateResult( CalcLib::Number& result, CalcLib::Environment& environment )
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
		if( !gaEnv.StoreVariable( "element", result ) )
			break;

		if( !conversionEvaluator->EvaluateResult( result, gaEnv ) )
			break;

		success = true;
	}
	while( false );

	return success;
}

// VectorToFromBivectorFunction.cpp