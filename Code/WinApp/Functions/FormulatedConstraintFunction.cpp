// FormulatedConstraintFunction.cpp

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "FormulatedConstraintFunction.h"
#include "../Constraints/FormulatedConstraint.h"

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( GAVisToolFormulatedConstraintFunctionEvaluator, FunctionEvaluator );

//=========================================================================================
GAVisToolFormulatedConstraintFunctionEvaluator::GAVisToolFormulatedConstraintFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ GAVisToolFormulatedConstraintFunctionEvaluator::~GAVisToolFormulatedConstraintFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ bool GAVisToolFormulatedConstraintFunctionEvaluator::EvaluateResult( CalcLib::Number& result, CalcLib::Environment& environment )
{
	bool success = false;
	FormulatedConstraint* formulatedConstraint = 0;

	do
	{
		if( !result.AssignAdditiveIdentity( environment ) )
			break;

		if( !environment.IsTypeOf( GAVisToolEnvironment::ClassName() ) )
		{
			environment.AddError( "The formulated constraint function can only operate within the GAVisTool environment." );
			break;
		}

		GAVisToolEnvironment* visToolEnv = ( GAVisToolEnvironment* )&environment;

		if( GetArgumentCount() > 1 || GetArgumentCount() <= 0 )
		{
			environment.AddError( "The formulated constraint function expects one and only one argument." );
			break;
		}

		CalcLib::FunctionArgumentEvaluator* argumentEvaluator = GetArgument(0);
		CalcLib::Evaluator* evaluator = argumentEvaluator->Argument();
		if( !evaluator->IsTypeOf( CalcLib::StringEvaluator::ClassName() ) )
		{
			environment.AddError( "The formulated constraint function expects its argument to be a string." );
			break;
		}

		CalcLib::StringEvaluator* stringEvaluator = ( CalcLib::StringEvaluator* )evaluator;
		const char* formula = stringEvaluator->String();

		formulatedConstraint = new FormulatedConstraint();
		
		if( !formulatedConstraint->SetFormula( formula, visToolEnv ) )
		{
			environment.AddError( "Failed to use \"%s\" as the formula for a formulated constraint.", formula );
			break;
		}

		if( !visToolEnv->AddConstraint( formulatedConstraint ) )
		{
			environment.AddError( "The environment rejected the formulated constraint." );
			break;
		}

		// Do an initial execution of the newly added constraint.
		formulatedConstraint->MarkAllInputsAsChanged( visToolEnv );
		visToolEnv->SatisfyConstraints();

		success = true;
	}
	while( false );

	if( !success )
	{
		if( formulatedConstraint )
			delete formulatedConstraint;
	}

	return success;
}

// FormulatedConstraintFunction.cpp