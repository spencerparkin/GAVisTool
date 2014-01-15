// BindFunction.cpp

/*
 * Copyright (C) 2013-2014 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "BindFunction.h"
#include "../Environment.h"
#include "../Application.h"

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( GAVisToolBindFunctionEvaluator, FunctionEvaluator );

//=========================================================================================
GAVisToolBindFunctionEvaluator::GAVisToolBindFunctionEvaluator( GAVisToolGeometry::CreationFunction creationFunction, GAVisToolGeometry::BindType bindType )
{
	this->creationFunction = creationFunction;
	this->bindType = bindType;
}

//=========================================================================================
/*virtual*/ GAVisToolBindFunctionEvaluator::~GAVisToolBindFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ bool GAVisToolBindFunctionEvaluator::EvaluateResult( CalcLib::Number& result, CalcLib::Environment& environment )
{
	if( !result.AssignAdditiveIdentity( environment ) )
		return false;

	if( !result.IsTypeOf( CalcLib::MultivectorNumber::ClassName() ) )
	{
		environment.AddError( "The bind function can only produce multivectors." );
		return false;
	}

	CalcLib::MultivectorNumber* multivectorNumber = ( CalcLib::MultivectorNumber* )&result;

	if( !environment.IsTypeOf( GAVisToolEnvironment::ClassName() ) )
	{
		environment.AddError( "The bind function can only operate within the GAVisTool environment." );
		return false;
	}

	GAVisToolEnvironment* visToolEnv = ( GAVisToolEnvironment* )&environment;

	if( GetArgumentCount() == 0 )
	{
		environment.AddError( "No arguments were given to the bind function." );
		return false;
	}

	for( int index = 0; index < GetArgumentCount(); index++ )
	{
		CalcLib::FunctionArgumentEvaluator* argumentEvaluator = GetArgument( index );
		CalcLib::Evaluator* evaluator = argumentEvaluator->Argument();

		if( !evaluator->IsTypeOf( CalcLib::VariableEvaluator::ClassName() ) )
		{
			environment.AddError( "The bind functions expects its arguments to be variables.  Argument %d is not a variable.", index + 1 );
			return false;
		}

		CalcLib::VariableEvaluator* variableEvaluator = ( CalcLib::VariableEvaluator* )evaluator;

		if( !Unbind( visToolEnv, variableEvaluator ) )
		{
			environment.AddError( "Failed to unbind variable \"%s\".", variableEvaluator->Name() );
			return false;
		}

		if( creationFunction != 0 && !Bind( visToolEnv, variableEvaluator, multivectorNumber ) )
		{
			environment.AddError( "Failed to bind variable \"%s\".", variableEvaluator->Name() );
			return false;
		}
	}

	return true;
}

//=========================================================================================
bool GAVisToolBindFunctionEvaluator::Unbind(
							GAVisToolEnvironment* visToolEnv,
							CalcLib::VariableEvaluator* variableEvaluator )
{
	// If the given variable is bound to a target, then unbind it.
	GAVisToolBindTarget* bindTarget = visToolEnv->LookupBindTargetByName( variableEvaluator->Name() );
	if( bindTarget )
	{
		// Before nuking the target, make sure that we sync the variable
		// in the environment with what value the target held.
		GeometricAlgebra::SumOfBlades element;
		bindTarget->ComposeTo( element );
		
		// Okay, now blow it away.
		visToolEnv->RemoveBindTarget( bindTarget );
		delete bindTarget;
		visToolEnv->Print( "The old bind-target bound to variable \"%s\" has been removed.\n", variableEvaluator->Name() );

		// Now sync the variable value.
		CalcLib::MultivectorNumber multivectorNumber( element );
		CalcLib::Number* variableValue = &multivectorNumber;
		visToolEnv->StoreVariable( variableEvaluator->Name(), *variableValue );
	}

	return true;
}

//=========================================================================================
bool GAVisToolBindFunctionEvaluator::Bind(
							GAVisToolEnvironment* visToolEnv,
							CalcLib::VariableEvaluator* variableEvaluator,
							CalcLib::MultivectorNumber* multivectorNumber )
{
	GAVisToolBindTarget* bindTarget = creationFunction( bindType );
	bindTarget->SetName( variableEvaluator->Name() );

	if( visToolEnv->VariableExists( variableEvaluator->Name() ) )
	{
		if( !variableEvaluator->EvaluateResult( *multivectorNumber, *visToolEnv ) )
			return false;
		GeometricAlgebra::SumOfBlades element;
		if( !multivectorNumber->AssignTo( element, *visToolEnv ) )
			return false;
		bindTarget->DecomposeFrom( element );
	}
	else
	{
		GeometricAlgebra::SumOfBlades element;
		bindTarget->ComposeTo( element );
		if( !multivectorNumber->AssignFrom( element, *visToolEnv ) )
			return false;
	}

	if( bindTarget->IsTypeOf( GAVisToolGeometry::ClassName() ) )
	{
		if( wxGetApp().canvasFrame->canvas->drawGeometryNames )
		{
			GAVisToolGeometry* geometry = ( GAVisToolGeometry* )bindTarget;
			geometry->GenerateNameTexture();
		}

		wxGetApp().canvasFrame->canvas->RedrawNeeded( true );
	}

	visToolEnv->AddBindTarget( bindTarget );
	visToolEnv->Print( "A new bind-target has been bound to variable \"%s\".\n", variableEvaluator->Name() );
	return true;
}

// BindFunction.cpp