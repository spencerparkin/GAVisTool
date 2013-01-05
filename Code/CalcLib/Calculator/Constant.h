// Constant.h

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
class CalcLib::ConstantEvaluator : public Evaluator
{
	DECLARE_CALCLIB_CLASS( ConstantEvaluator );
public:
	ConstantEvaluator( Number* constant );
	virtual ~ConstantEvaluator( void );
	virtual bool EvaluateResult( Number& result, Environment& environment ) override;
	virtual bool StoreResult( const Number& result, Environment& environment ) override;
private:
	Number* constant;
};

// Constant.h