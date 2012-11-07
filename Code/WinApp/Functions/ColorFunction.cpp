// ColorFunction.cpp

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "ColorFunction.h"
#include "../Environment.h"
#include "../Application.h"

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( GAVisToolColorFunctionEvaluator, FunctionEvaluator );

//=========================================================================================
GAVisToolColorFunctionEvaluator::GAVisToolColorFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ GAVisToolColorFunctionEvaluator::~GAVisToolColorFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ bool GAVisToolColorFunctionEvaluator::EvaluateResult( CalcLib::Number& result, CalcLib::Environment& environment )
{
	bool success = false;

	CalcLib::Number* redResult = 0;
	CalcLib::Number* greenResult = 0;
	CalcLib::Number* blueResult = 0;
	CalcLib::Number* alphaResult = 0;

	do
	{
		if( !result.AssignAdditiveIdentity( environment ) )
			break;

		if( !environment.IsTypeOf( GAVisToolEnvironment::ClassName() ) )
		{
			environment.AddError( "The color function can only operate within the GAVisTool environment." );
			break;
		}

		GAVisToolEnvironment* visToolEnv = ( GAVisToolEnvironment* )&environment;

		CalcLib::FunctionArgumentEvaluator* argumentEvaluator = GetArgument(0);
		if( !argumentEvaluator )
		{
			environment.AddError( "No arguments were given to the color function." );
			break;
		}

		CalcLib::Evaluator* evaluator = argumentEvaluator->Argument();
		if( !evaluator->IsTypeOf( CalcLib::VariableEvaluator::ClassName() ) )
		{
			environment.AddError( "The color function expects its first argument to be a variable." );
			break;
		}

		CalcLib::VariableEvaluator* variableEvaluator = ( CalcLib::VariableEvaluator* )evaluator;

		GAVisToolGeometry* geometry = visToolEnv->LookupGeometryByName( variableEvaluator->Name() );
		if( !geometry )
		{
			environment.AddError( "The color function expects its variable argument to be bound to a geometry in the environment." );
			break;
		}

		CalcLib::FunctionArgumentEvaluator* redArgumentEvaluator = GetArgument(1);
		CalcLib::FunctionArgumentEvaluator* greenArgumentEvaluator = GetArgument(2);
		CalcLib::FunctionArgumentEvaluator* blueArgumentEvaluator = GetArgument(3);
		if( !( redArgumentEvaluator && greenArgumentEvaluator && blueArgumentEvaluator ) )
		{
			environment.AddError( "The color function expects the 2nd, 3rd and 4th arguments to be red, green and blue, respectively.  The 5th is optionally alpha." );
			break;
		}

		redResult = environment.CreateNumber( redArgumentEvaluator );
		greenResult = environment.CreateNumber( greenArgumentEvaluator );
		blueResult = environment.CreateNumber( blueArgumentEvaluator );
		
		bool redSuccess = redArgumentEvaluator->EvaluateResult( *redResult, environment );
		bool greenSuccess = greenArgumentEvaluator->EvaluateResult( *greenResult, environment );
		bool blueSuccess = blueArgumentEvaluator->EvaluateResult( *blueResult, environment );
		if( !( redSuccess && greenSuccess && blueSuccess ) )
			break;

		CalcLib::MultivectorNumber* redMultivectorNumber =( CalcLib::MultivectorNumber* )redResult;
		CalcLib::MultivectorNumber* greenMultivectorNumber =( CalcLib::MultivectorNumber* )greenResult;
		CalcLib::MultivectorNumber* blueMultivectorNumber =( CalcLib::MultivectorNumber* )blueResult;

		double redMagnitude, greenMagnitude, blueMagnitude;
		
		redSuccess = CalculateMagnitudeAndClamp( *redMultivectorNumber, redMagnitude, environment );
		greenSuccess = CalculateMagnitudeAndClamp( *greenMultivectorNumber, greenMagnitude, environment );
		blueSuccess = CalculateMagnitudeAndClamp( *blueMultivectorNumber, blueMagnitude, environment );
		if( !( redSuccess && greenSuccess && blueSuccess ) )
		{
			environment.AddError( "Failed to calculate color from color arguments." );
			break;
		}

		VectorMath::Vector color;
		VectorMath::Set( color, redMagnitude, greenMagnitude, blueMagnitude );
		geometry->SetColor( color );

		CalcLib::FunctionArgumentEvaluator* alphaArgumentEvaluator = GetArgument(4);
		if( alphaArgumentEvaluator )
		{
			alphaResult = environment.CreateNumber( alphaArgumentEvaluator );
			bool alphaSuccess = alphaArgumentEvaluator->EvaluateResult( *alphaResult, environment );
			if( !alphaSuccess )
				break;

			CalcLib::MultivectorNumber* alphaMultivectorNumber =( CalcLib::MultivectorNumber* )alphaResult;
			double alphaMagnitude;
			alphaSuccess = CalculateMagnitudeAndClamp( *alphaMultivectorNumber, alphaMagnitude, environment );
			if( !alphaSuccess )
			{
				environment.AddError( "Failed to calculate alpha from color argument." );
				break;
			}

			geometry->SetAlpha( alphaMagnitude );
		}

		success = true;
	}
	while( false );

	delete redResult;
	delete greenResult;
	delete blueResult;
	delete alphaResult;

	wxGetApp().canvasFrame->canvas->RedrawNeeded( true );

	return success;
}

//=========================================================================================
bool GAVisToolColorFunctionEvaluator::CalculateMagnitudeAndClamp( const CalcLib::MultivectorNumber& multivectorNumber, double& magnitude, CalcLib::Environment& environment )
{
	GeometricAlgebra::SumOfBlades multivector;

	if( !multivectorNumber.AssignTo( multivector, environment ) )
		return false;

	GeometricAlgebra::Scalar scalar;
	if( !multivector.AssignSquareMagnitudeTo( scalar ) )
		return false;

	double squareMagnitude = scalar;
	magnitude = sqrt( squareMagnitude );
	if( magnitude < 0.0 )
		magnitude = 0.0;
	if( magnitude > 1.0 )
		magnitude = 1.0;

	return true;
}

// ColorFunction.cpp