// FloatingPointMathFunc.cpp

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
IMPLEMENT_CALCLIB_CLASS1( FloatingPointMathFunctionEvaluator, FunctionEvaluator );

//=========================================================================================
FloatingPointMathFunctionEvaluator::FloatingPointMathFunctionEvaluator( void )
{
	function = FUNC_UNKNOWN;
}

//=========================================================================================
/*virtual*/ FloatingPointMathFunctionEvaluator::~FloatingPointMathFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ bool FloatingPointMathFunctionEvaluator::EvaluateResult( Number& result, Environment& environment )
{
	bool success = false;
	Number** numberArray = 0;
	int numberArraySize = 0;
	FloatingPointNumber* floatResult = ( FloatingPointNumber* )&result;

	do
	{
		if( !EvaluateArguments( numberArray, numberArraySize, environment ) )
			break;

		bool functionEvaluated = false;
		switch( function )
		{
			case FUNC_SQRT:
			{
				if( numberArraySize != 1 )
				{
					environment.AddError( "The \"sqrt\" function expects one and only one argument." );
					break;
				}
				double argument = ( ( FloatingPointNumber* )numberArray[0] )->value;
				floatResult->value = sqrt( argument );
				functionEvaluated = true;
				break;
			}
			case FUNC_EXP:
			{
				if( numberArraySize != 1 )
				{
					environment.AddError( "The \"exp\" function expects one and only one argument." );
					break;
				}
				double argument = ( ( FloatingPointNumber* )numberArray[0] )->value;
				floatResult->value = exp( argument );
				functionEvaluated = true;
				break;
			}
			case FUNC_LOG:
			{
				if( numberArraySize != 2 )
				{
					environment.AddError( "The \"log\" function expects two and only two arguments." );
					break;
				}
				double argument = ( ( FloatingPointNumber* )numberArray[0] )->value;
				double base = ( ( FloatingPointNumber* )numberArray[1] )->value;
				floatResult->value = log( argument ) / log( base );
				functionEvaluated = true;
				break;
			}
			case FUNC_LN:
			{
				if( numberArraySize != 1 )
				{
					environment.AddError( "The \"ln\" function expects one and only one argument." );
					break;
				}
				double argument = ( ( FloatingPointNumber* )numberArray[0] )->value;
				floatResult->value = log( argument );
				functionEvaluated = true;
				break;
			}
			case FUNC_POW:
			{
				if( numberArraySize != 2 )
				{
					environment.AddError( "The \"pow\" function expects two and only two arguments." );
					break;
				}
				double argument = ( ( FloatingPointNumber* )numberArray[0] )->value;
				double exponent = ( ( FloatingPointNumber* )numberArray[1] )->value;
				floatResult->value = pow( argument, exponent );
				functionEvaluated = true;
				break;
			}
			case FUNC_SIN:
			{
				if( numberArraySize != 1 )
				{
					environment.AddError( "The \"sin\" function expects one and only one argument." );
					break;
				}
				double argument = ( ( FloatingPointNumber* )numberArray[0] )->value;
				floatResult->value = sin( argument );
				functionEvaluated = true;
				break;
			}
			case FUNC_COS:
			{
				if( numberArraySize != 1 )
				{
					environment.AddError( "The \"cos\" function expects one and only one argument." );
					break;
				}
				double argument = ( ( FloatingPointNumber* )numberArray[0] )->value;
				floatResult->value = cos( argument );
				functionEvaluated = true;
				break;
			}
			case FUNC_TAN:
			{
				if( numberArraySize != 1 )
				{
					environment.AddError( "The \"tan\" function expects one and only one argument." );
					break;
				}
				double argument = ( ( FloatingPointNumber* )numberArray[0] )->value;
				floatResult->value = tan( argument );
				functionEvaluated = true;
				break;
			}
			case FUNC_ASIN:
			{
				if( numberArraySize != 1 )
				{
					environment.AddError( "The \"asin\" function expects one and only one argument." );
					break;
				}
				double argument = ( ( FloatingPointNumber* )numberArray[0] )->value;
				floatResult->value = asin( argument );
				functionEvaluated = true;
				break;
			}
			case FUNC_ACOS:
			{
				if( numberArraySize != 1 )
				{
					environment.AddError( "The \"acos\" function expects one and only one argument." );
					break;
				}
				double argument = ( ( FloatingPointNumber* )numberArray[0] )->value;
				floatResult->value = acos( argument );
				functionEvaluated = true;
				break;
			}
			case FUNC_ATAN:
			{
				if( numberArraySize != 1 )
				{
					environment.AddError( "The \"atan\" function expects one and only one argument." );
					break;
				}
				double argument = ( ( FloatingPointNumber* )numberArray[0] )->value;
				floatResult->value = atan( argument );
				functionEvaluated = true;
				break;
			}
			case FUNC_ATAN2:
			{
				if( numberArraySize != 1 )
				{
					environment.AddError( "The \"atan2\" function expects two and only two arguments." );
					break;
				}
				double argumentY = ( ( FloatingPointNumber* )numberArray[0] )->value;
				double argumentX = ( ( FloatingPointNumber* )numberArray[1] )->value;
				floatResult->value = atan2( argumentY, argumentX );
				functionEvaluated = true;
				break;
			}
			case FUNC_CSC:
			{
				if( numberArraySize != 1 )
				{
					environment.AddError( "The \"csc\" function expects one and only one argument." );
					break;
				}
				double argument = ( ( FloatingPointNumber* )numberArray[0] )->value;
				floatResult->value = 1.0 / sin( argument );
				functionEvaluated = true;
				break;
			}
			case FUNC_SEC:
			{
				if( numberArraySize != 1 )
				{
					environment.AddError( "The \"sec\" function expects one and only one argument." );
					break;
				}
				double argument = ( ( FloatingPointNumber* )numberArray[0] )->value;
				floatResult->value = 1.0 / cos( argument );
				functionEvaluated = true;
				break;
			}
			case FUNC_COT:
			{
				if( numberArraySize != 1 )
				{
					environment.AddError( "The \"cot\" function expects one and only one argument." );
					break;
				}
				double argument = ( ( FloatingPointNumber* )numberArray[0] )->value;
				floatResult->value = 1.0 / tan( argument );
				functionEvaluated = true;
				break;
			}
			case FUNC_ACSC:
			case FUNC_ASEC:
			case FUNC_ACOT:
			{
				break;
			}
		}
		if( !functionEvaluated )
			break;

		success = true;
	}
	while( false );

	DeleteArguments( numberArray, numberArraySize );

	return success;
}

//=========================================================================================
void FloatingPointMathFunctionEvaluator::Function( FunctionType function )
{
	this->function = function;
}

// FloatingPointMathFunc.cpp