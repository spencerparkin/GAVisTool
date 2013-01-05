// Variable.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

//=========================================================================================
class CalcLib::VariableEvaluator : public Evaluator
{
	DECLARE_CALCLIB_CLASS( VariableEvaluator );
public:
	VariableEvaluator( const char* variableName );
	virtual ~VariableEvaluator( void );
	virtual bool EvaluateResult( Number& result, Environment& environment ) override;
	virtual bool StoreResult( const Number& result, Environment& environment ) override;
	virtual Number* CreateNumberForResult( Environment& environment ) override;
	const char* Name( void ) const;
private:
	char* variableName;
};

// Variable.h