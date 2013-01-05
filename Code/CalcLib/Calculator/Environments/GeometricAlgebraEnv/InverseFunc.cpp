// InverseFunc.cpp

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "../../CalcLib.h"

using namespace CalcLib;

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( InverseFunctionEvaluator, FunctionEvaluator );

//=========================================================================================
InverseFunctionEvaluator::InverseFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ InverseFunctionEvaluator::~InverseFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ bool InverseFunctionEvaluator::EvaluateResult( Number& result, Environment& environment )
{
	bool success = false;
	Number* argumentResult = 0;

	do
	{
		FunctionArgumentEvaluator* argument = GetArgument(0);
		if( !argument )
		{
			environment.AddError( "The inverse function expects an argument." );
			break;
		}

		argumentResult = environment.CreateNumber( argument );
		if( !argument->EvaluateResult( *argumentResult, environment ) )
			break;

		if( !result.IsTypeOf( MultivectorNumber::ClassName() ) )
			break;
		
		if( !result.AssignMultilicativeInverse( argumentResult, environment ) )
		{
			environment.AddError( "Failed to take the inverse!" );
			break;
		}

		success = true;
	}
	while( false );

	delete argumentResult;

	return success;
}

// InverseFunc.cpp