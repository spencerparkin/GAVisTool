// MatrixFunctions.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

//=========================================================================================
class CalcLib::MatrixFunctionEvaluator : public FunctionEvaluator
{
	DECLARE_CALCLIB_CLASS( MatrixFunctionEvaluator );

public:

	enum Function
	{
		CREATE_MATRIX,
		SET_ELEMENT,
		SET_ROW,
		SET_COLUMN,
		GET_ELEMENT,
		GET_ROW,
		GET_COLUMN,
		CALC_INVERSE,
		CALC_DETERMINANT,
	};

	MatrixFunctionEvaluator( Function function );
	virtual ~MatrixFunctionEvaluator( void );
	virtual bool EvaluateResult( Number& result, Environment& environment ) override;
	virtual Number* CreateNumberForResult( Environment& environment ) override;

private:

	bool PerformFunction( Number& result, Number** numberArray, int numberArraySize, Environment& environment );

	Function function;
};

// MatrixFunctions.h