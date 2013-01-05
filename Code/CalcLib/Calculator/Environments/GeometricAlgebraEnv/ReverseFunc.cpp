// ReverseFunc.cpp

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
IMPLEMENT_CALCLIB_CLASS1( ReverseFunctionEvaluator, FunctionEvaluator );

//=========================================================================================
ReverseFunctionEvaluator::ReverseFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ ReverseFunctionEvaluator::~ReverseFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ bool ReverseFunctionEvaluator::EvaluateResult( Number& result, Environment& environment )
{
	bool success = false;
	Number* argumentResult = 0;

	do
	{
		FunctionArgumentEvaluator* argument = GetArgument(0);
		if( !argument )
		{
			environment.AddError( "The reverse function expects an argument." );
			break;
		}

		argumentResult = environment.CreateNumber( argument );
		if( !argument->EvaluateResult( *argumentResult, environment ) )
			break;

		if( !result.AssignFrom( argumentResult, environment ) )
			break;

		if( !result.IsTypeOf( MultivectorNumber::ClassName() ) )
		{
			environment.AddError( "The reverse function can only take the reverse of multivector number types." );
			break;
		}

		MultivectorNumber* multivector = ( MultivectorNumber* )&result;
		if( !multivector->Reverse( environment ) )
		{
			environment.AddError( "Failed to take the reverse!" );
			break;
		}

		success = true;
	}
	while( false );

	delete argumentResult;

	return success;
}

// ReverseFunc.cpp