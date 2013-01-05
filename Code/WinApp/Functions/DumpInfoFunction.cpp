// DumpInfoFunction.cpp

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "DumpInfoFunction.h"
#include "../Environment.h"

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( GAVisToolDumpInfoFunctionEvaluator, FunctionEvaluator );

//=========================================================================================
GAVisToolDumpInfoFunctionEvaluator::GAVisToolDumpInfoFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ GAVisToolDumpInfoFunctionEvaluator::~GAVisToolDumpInfoFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ bool GAVisToolDumpInfoFunctionEvaluator::EvaluateResult( CalcLib::Number& result, CalcLib::Environment& environment )
{
	GAVisToolEnvironment* visToolEnv = environment.Cast< GAVisToolEnvironment >();
	if( !visToolEnv )
	{
		environment.AddError( "The dump_info() function only operates within the GAVisTool environment." );
		return false;
	}

	// If given no arguments, dump out info on all bind targets.
	if( GetArgumentCount() == 0 )
	{
		const Utilities::List& bindTargetList = visToolEnv->BindTargetList();
		const GAVisToolBindTarget* bindTarget = ( const GAVisToolBindTarget* )bindTargetList.LeftMost();
		while( bindTarget )
		{
			char printBuffer[ 1024 ];
			bindTarget->DumpInfo( printBuffer, sizeof( printBuffer ) );
			environment.Print( "%s\n", printBuffer );
			bindTarget = ( const GAVisToolBindTarget* )bindTarget->Right();
		}
	}
	else
	{
		for( int index = 0; index < GetArgumentCount(); index++ )
		{
			CalcLib::FunctionArgumentEvaluator* argumentEvaluator = GetArgument( index );
			CalcLib::Evaluator* evaluator = argumentEvaluator->Argument();
			CalcLib::VariableEvaluator* variableEvaluator = evaluator->Cast< CalcLib::VariableEvaluator >();
			if( !variableEvaluator )
				environment.Print( "Skipping argument %d, because dump_info() doesn't handle the argument type.\n", index + 1 );
			else
			{
				const char* variableName = variableEvaluator->Name();
				GAVisToolBindTarget* bindTarget = visToolEnv->LookupBindTargetByName( variableName );
				if( !bindTarget )
					environment.Print( "The variable \"%s\" is not bound to a bind target.\n", variableName );
				else
				{
					char printBuffer[ 1024 ];
					bindTarget->DumpInfo( printBuffer, sizeof( printBuffer ) );
					environment.Print( "%s\n", printBuffer );
				}
			}
		}
	}

	return true;
}

// DumpInfoFunction.cpp