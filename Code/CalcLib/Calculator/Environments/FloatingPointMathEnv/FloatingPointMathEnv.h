// FloatingPointMathEnv.h

//=========================================================================================
class CalcLib::FloatingPointMathEnvironment : public Environment
{
	DECLARE_CALCLIB_CLASS( FloatingPointMathEnvironment );

public:

	FloatingPointMathEnvironment( void );
	virtual ~FloatingPointMathEnvironment( void );
	
	virtual void PrintEnvironmentInfo( void ) override;
	virtual Number* CreateNumber( void ) override;
	virtual FunctionEvaluator* CreateFunction( const char* functionName ) override;
	virtual Evaluator* CreateVariable( const char* variableName ) override;
	virtual Evaluator* CreateBinaryOperator( const char* operatorName, Evaluator* leftOperand, Evaluator* rightOperand, bool& isBinaryOperationEvaluator ) override;
};

// FloatingPointMathEnv.h