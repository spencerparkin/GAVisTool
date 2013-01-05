// GeometricAlgebraEnv.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

//=========================================================================================
class CalcLib::GeometricAlgebraEnvironment : public Environment
{
	DECLARE_CALCLIB_CLASS( GeometricAlgebraEnvironment );

public:
	
	GeometricAlgebraEnvironment( void );
	virtual ~GeometricAlgebraEnvironment( void );
	
	virtual void PrintEnvironmentInfo( void ) override;
	virtual Number* CreateNumber( void ) override;
	virtual FunctionEvaluator* CreateFunction( const char* functionName ) override;
	virtual Evaluator* CreateVariable( const char* variableName ) override;
	virtual Evaluator* CreateBinaryOperator( const char* operatorName, Evaluator* leftOperand, Evaluator* rightOperand, bool& isBinaryOperationEvaluator ) override;
	virtual Evaluator* CreateUnaryOperator( const char* operatorName, Evaluator* operand, bool& isUnaryOperationEvaluator ) override;

	enum DisplayMode
	{
		DISPLAY_AS_SUM_OF_BLADES,
		DISPLAY_AS_SUM_OF_VERSORS,
	};

	DisplayMode displayMode;
};

// GeometricAlgebraEnv.h