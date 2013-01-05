// MatrixFunctions.cpp

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
IMPLEMENT_CALCLIB_CLASS1( MatrixFunctionEvaluator, FunctionEvaluator );

//=========================================================================================
MatrixFunctionEvaluator::MatrixFunctionEvaluator( Function function )
{
	this->function = function;
}

//=========================================================================================
/*virtual*/ MatrixFunctionEvaluator::~MatrixFunctionEvaluator( void )
{
}

//=========================================================================================
/*virtual*/ bool MatrixFunctionEvaluator::EvaluateResult( Number& result, Environment& environment )
{
	bool success = false;
	Number** numberArray = 0;
	int numberArraySize = 0;

	do
	{
		if( !result.IsTypeOf( MatrixNumber::ClassName() ) )
		{
			environment.AddError( "The matrix function evaluator works only with matrix numbers." );
			break;
		}

		if( !EvaluateArguments( numberArray, numberArraySize, environment ) )
			break;

		int index;
		for( index = 0; index < numberArraySize; index++ )
		{
			if( !numberArray[ index ]->IsTypeOf( MatrixNumber::ClassName() ) )
			{
				environment.AddError( "The matrix function evaluator works only with matrix numbers." );
				break;
			}
		}
		if( index < numberArraySize )
			break;

		if( !PerformFunction( result, numberArray, numberArraySize, environment ) )
			return false;

		success = true;
	}
	while( false );

	DeleteArguments( numberArray, numberArraySize );

	return success;
}

//=========================================================================================
bool MatrixFunctionEvaluator::PerformFunction( Number& result, Number** numberArray, int numberArraySize, Environment& environment )
{
	switch( function )
	{
		case CREATE_MATRIX:
		{
			if( numberArraySize != 2 )
				return false;
			
			MatrixNumber* rowsNumber = ( MatrixNumber* )numberArray[0];
			MatrixNumber* colsNumber = ( MatrixNumber* )numberArray[1];

			double rowsFloat, colsFloat;
			if( !rowsNumber->AssignTo( rowsFloat, environment ) )
				return false;
			if( !colsNumber->AssignTo( colsFloat, environment ) )
				return false;
			int rows = int( rowsFloat );
			int cols = int( colsFloat );

			LinearAlgebra::Matrix matrix;
			if( !matrix.AssignDimensions( rows, cols ) )
				return false;

			MatrixNumber* matrixNumber = ( MatrixNumber* )&result;
			matrixNumber->element.AssignMatrixFrom( matrix );

			break;
		}
		case SET_ELEMENT:
		{
			if( numberArraySize != 4 )
				return false;

			MatrixNumber* matrixNumber = ( MatrixNumber* )numberArray[0];
			MatrixNumber* rowNumber = ( MatrixNumber* )numberArray[1];
			MatrixNumber* colNumber = ( MatrixNumber* )numberArray[2];
			MatrixNumber* elementNumber = ( MatrixNumber* )numberArray[3];

			double rowFloat, colFloat;
			if( !rowNumber->AssignTo( rowFloat, environment ) )
				return false;
			if( !colNumber->AssignTo( colFloat, environment ) )
				return false;
			int row = int( rowFloat );
			int col = int( colFloat );

			LinearAlgebra::Matrix* matrix = matrixNumber->element.AccessMatrix();
			if( !matrix )
				return false;

			if( !matrix->AssignElementFrom( row, col, elementNumber->element ) )
				return false;

			matrixNumber = ( MatrixNumber* )&result;
			matrixNumber->element.AssignMatrixFrom( *matrix );

			break;
		}
		case SET_ROW:
		{
			break;
		}
		case SET_COLUMN:
		{
			break;
		}
		case GET_ELEMENT:
		{
			break;
		}
		case GET_ROW:
		{
			break;
		}
		case GET_COLUMN:
		{
			break;
		}
		case CALC_INVERSE:
		{
			if( numberArraySize != 1 )
				return false;

			MatrixNumber* matrixNumber = ( MatrixNumber* )numberArray[0];
			LinearAlgebra::Matrix* matrix = matrixNumber->element.AccessMatrix();
			if( !matrix )
				return false;

			LinearAlgebra::Matrix::InverseResult inverseResult;
			LinearAlgebra::Matrix inverseMatrix;
			if( !inverseMatrix.AssignInverse( *matrix, LinearAlgebra::Matrix::LEFT_INVERSE, inverseResult ) )
				return false;

			matrixNumber = ( MatrixNumber* )&result;
			if( !matrixNumber->element.AssignMatrixFrom( inverseMatrix ) )
				return false;

			break;
		}
		case CALC_DETERMINANT:
		{
			if( numberArraySize != 1 )
				return false;

			MatrixNumber* matrixNumber = ( MatrixNumber* )numberArray[0];
			LinearAlgebra::Matrix* matrix = matrixNumber->element.AccessMatrix();
			if( !matrix )
				return false;

			LinearAlgebra::Scalar scalar;
			if( !matrix->AssignDeterminantTo( scalar ) )
				return false;

			matrixNumber = ( MatrixNumber* )&result;
			if( !matrixNumber->element.AssignScalarFrom( scalar ) )
				return false;

			break;
		}
	}

	return true;
}

//=========================================================================================
/*virtual*/ Number* MatrixFunctionEvaluator::CreateNumberForResult( Environment& environment )
{
	return new MatrixNumber();
}

// MatrixFunctions.cpp