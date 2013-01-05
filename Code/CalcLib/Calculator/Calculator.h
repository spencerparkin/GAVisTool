// Calculator.h

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
class CalcLib::Calculator : public CalcLibClass
{
	DECLARE_CALCLIB_CLASS( Calculator );

public:

	Calculator( const char* envString );
	Calculator( Environment* environment );
	virtual ~Calculator( void );

	bool IsPartialMathExpression( const char* mathExpression, bool& isPartialMathExpression );

	bool Calculate( const char* mathExpression, char* resultBuffer, int resultBufferSize, bool printLatex = false );

	void PrintErrorReport( void );

	Environment* GetEnvironment( void );
	void SetEnvironment( Environment* environment );

	Evaluator* CompileEvaluator( const char* mathExpression );

private:

	static Environment* NewEnvironment( const Number* result );
	static Environment* NewEnvironment( const char* envString );

	Environment* environment;
	Tokenizer tokenizer;
	Parser parser;
};

// Calculator.h