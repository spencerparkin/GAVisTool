// ArbitraryPrecisionEnv.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

//=========================================================================================
class CalcLib::ArbitraryPrecisionEnvironment : public Environment
{
	DECLARE_CALCLIB_CLASS( ArbitraryPrecisionEnvironment );

public:

	ArbitraryPrecisionEnvironment( void );
	virtual ~ArbitraryPrecisionEnvironment( void );

	virtual void PrintEnvironmentInfo( void ) override;
	virtual Number* CreateNumber( void ) override;
	virtual FunctionEvaluator* CreateFunction( const char* functionName );
	virtual Evaluator* CreateVariable( const char* variableName );

	void DivisionLimit( int divisionLimit );
	int DivisionLimit( void );

	enum PrintFormatType
	{
		PRINT_COMPACT_NOTATION,
		PRINT_POLYNOMIAL_NOTATION,
	};

	void PrintFormat( PrintFormatType printFormatType );
	PrintFormatType PrintFormat( void );

private:

	// This is the minimum monomial degree to which we'll carry out the division algorithm before rounding the answer.
	int divisionLimit;
	PrintFormatType printFormat;
};

// ArbitraryPrecisionEnv.h