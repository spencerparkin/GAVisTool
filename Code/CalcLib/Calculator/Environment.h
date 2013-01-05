// Environment.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

// TODO: All printing should go through an environment function
//       so that output can be redirected to the appropriate device.
//       We might also want to get input through the environment.

// TODO: It might be nice to provide a way to save and restore
//       environments from file.  For example, to save out all
//       variables to a file for later re-load.

//=========================================================================================
class CalcLib::Environment : public CalcLibClass
{
	DECLARE_CALCLIB_CLASS( Environment );

public:

	Environment( void );
	virtual ~Environment( void );

	virtual void PrintEnvironmentInfo( void );

	// Creation functions:
	Number* CreateNumber( Evaluator* evaluator );		// Always call this to create a number from the environment.
	virtual FunctionEvaluator* CreateFunction( const char* functionName );
	virtual Evaluator* CreateVariable( const char* variableName ) = 0;
	virtual Evaluator* CreateBinaryOperator( const char* operatorName, Evaluator* leftOperand, Evaluator* rightOperand, bool& isBinaryOperationEvaluator );
	virtual Evaluator* CreateUnaryOperator( const char* operatorName, Evaluator* operand, bool& isUnaryOperationEvaluator );

	// Variable maintenance:
	void DumpVariables( void );
	void DeleteAllVariables( void );
	Number* AccessStoredVariable( const char* variableName );
	virtual bool LookupVariable( const char* variableName, Number& variableValue );
	virtual bool StoreVariable( const char* variableName, const Number& variableValue );
	bool VariableExists( const char* variableName );

	// Error reporting:
	void WipeAllErrors( void );
	void PrintAllErrors( void );
	void AddError( const char* formatString, ... );
	int ErrorCount( void );

	// Additional user feedback:
	void PrintBuffer( char* printBuffer, int printBufferSize );
	const char* PrintBuffer( void );
	bool Print( const char* formatString, ... );

	//void PushScope( void );
	//void PopScope( void );

protected:

	// This should never be called by an evaluator to create a number.
	// It will be called by the non-virtual overload above.
	virtual Number* CreateNumber( void ) = 0;

private:

	Utilities::Map< Number* > variableMap;

	class Error : public Utilities::List::Item
	{
	public:
		Error( const char* errorString );
		virtual ~Error( void );
		char* errorString;
	};

	Utilities::List listOfErrors;

	char* printBuffer;
	int printBufferSize;
};

// Environment.h