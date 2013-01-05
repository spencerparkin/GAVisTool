// Parser.h

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
class CalcLib::Parser
{
public:

	Parser( void );
	virtual ~Parser( void );

	bool IsPartialStatement( Utilities::List& tokenizedStatement, bool& isPartialStatement, Environment& environment );
	bool Parse( Utilities::List& tokenizedStatement, Evaluator*& evaluator, Environment& environment );

private:

	enum Syntax
	{
		SYNTAX_UNKNOWN,
		SYNTAX_FUNCTION_CALL,
		SYNTAX_NUMBER,
		SYNTAX_BINARY_OPERATOR,
		SYNTAX_LEFT_UNARY_OPERATOR,
		SYNTAX_RIGHT_UNARY_OPERATOR,
	};

	enum Associativity
	{
		ASSOC_UNKNOWN,
		ASSOC_LEFT_TO_RIGHT,
		ASSOC_RIGHT_TO_LEFT,
		ASSOC_DOESNT_MATTER,
	};

	Evaluator* CreateEvaluationTree( Utilities::List& tokenizedStatement, Environment& environment );
	Evaluator* CreateString( Token* token, Environment& environment );
	Evaluator* CreateConstant( Token* token, Environment& environment );
	Evaluator* CreateVariable( Token* token, Environment& environment );
	Evaluator* CreateBinaryOperator( Token* token, Utilities::List& leftSubTokenizedStatement, Utilities::List& rightSubTokenizedStatement, Environment& environment );
	Evaluator* CreateUnaryOperator( Token* token, Utilities::List& subTokenizedStatement, Environment& environment );
	Evaluator* CreateFunction( Utilities::List& functionCallTokenizedStatement, Environment& environment );
	bool CreateFunctionArguments( FunctionEvaluator* evaluator, Utilities::List& tokenizedListOfArguments, Environment& environment );

	Token* FindFirstTokenOfType( Utilities::List& listOfTokens, Token::Type tokenType, bool skipSubExpressions );
	Token* FindEvaluatingToken( Utilities::List& tokenizedStatement, Environment& environment, Syntax& syntax );
	void RemoveOuterParentheses( Utilities::List& tokenizedStatement );
	Token* FindMatchingParenthesis( Token* token );
	int BinaryOperatorPrecedence( Token* token );
	int UnaryOperatorPrecedence( Token* token );
	Associativity OperatorAssociativity( Token* token );
	Syntax OperatorType( Token* token );
};

// Parser.h