// LinearAlgebraEnv.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

//=========================================================================================
class CalcLib::LinearAlgebraEnvironment : public Environment
{
	DECLARE_CALCLIB_CLASS( LinearAlgebraEnvironment );

public:
	LinearAlgebraEnvironment( void );
	virtual ~LinearAlgebraEnvironment( void );
	
	virtual void PrintEnvironmentInfo( void ) override;
	virtual Number* CreateNumber( void ) override;
	virtual FunctionEvaluator* CreateFunction( const char* functionName ) override;
	virtual Evaluator* CreateVariable( const char* variableName ) override;
};

// LinearAlgebraEnv.h