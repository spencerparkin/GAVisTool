// ComplexNumberFieldEnv.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

//=========================================================================================
class CalcLib::ComplexNumberFieldEnvironment : public Environment
{
	DECLARE_CALCLIB_CLASS( ComplexNumberFieldEnvironment );

public:

	ComplexNumberFieldEnvironment( void );
	virtual ~ComplexNumberFieldEnvironment( void );

	virtual void PrintEnvironmentInfo( void ) override;
	virtual Number* CreateNumber( void ) override;
	virtual FunctionEvaluator* CreateFunction( const char* functionName ) override;
	virtual Evaluator* CreateVariable( const char* variableName ) override;
};

// ComplexNumberFieldEnv.h