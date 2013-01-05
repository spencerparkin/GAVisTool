// WipeEnvFunction.cpp

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "WipeEnvFunction.h"
#include "../Environment.h"
#include "../Application.h"

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( GAVisToolWipeEnvFunctionEvaluator, FunctionEvaluator );

//=========================================================================================
GAVisToolWipeEnvFunctionEvaluator::GAVisToolWipeEnvFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ GAVisToolWipeEnvFunctionEvaluator::~GAVisToolWipeEnvFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ bool GAVisToolWipeEnvFunctionEvaluator::EvaluateResult( CalcLib::Number& result, CalcLib::Environment& environment )
{
	GAVisToolEnvironment* visToolEnv = environment.Cast< GAVisToolEnvironment >();
	if( !visToolEnv )
	{
		environment.AddError( "The wipe_env() function only operates within the GAVisTool environment." );
		return false;
	}

	// KABOOM!!!
	visToolEnv->Wipe( true, true );

	wxGetApp().canvasFrame->canvas->RedrawNeeded( true );

	return true;
}

// WipeEnvFunction.cpp