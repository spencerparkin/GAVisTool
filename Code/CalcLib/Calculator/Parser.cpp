// Parser.cpp

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "CalcLib.h"

using namespace CalcLib;

//=========================================================================================
Parser::Parser( void )
{
}

//=========================================================================================
/*virtual*/ Parser::~Parser( void )
{
}

//=========================================================================================
// This algorithm needs improvement.
bool Parser::IsPartialStatement( Utilities::List& tokenizedStatement, bool& isPartialStatement, Environment& environment )
{
	Token* leftParanToken = FindFirstTokenOfType( tokenizedStatement, Token::TYPE_LEFT_PARAN, false );
	if( !leftParanToken )
	{
		isPartialStatement = false;
		return true;
	}
	
	Token* rightParanToken = FindMatchingParenthesis( leftParanToken );
	if( !rightParanToken )
	{
		isPartialStatement = true;
		return true;
	}

	isPartialStatement = false;
	return true;
}

//=========================================================================================
bool Parser::Parse( Utilities::List& tokenizedStatement, Evaluator*& evaluator, Environment& environment )
{
	evaluator = CreateEvaluationTree( tokenizedStatement, environment );
	if( !evaluator )
		return false;
	return true;
}

//=========================================================================================
Evaluator* Parser::CreateEvaluationTree( Utilities::List& tokenizedStatement, Environment& environment )
{
	Evaluator* evaluator = 0;

	RemoveOuterParentheses( tokenizedStatement );

	do
	{
		Syntax syntax = SYNTAX_UNKNOWN;
		Token* token = FindEvaluatingToken( tokenizedStatement, environment, syntax );
		if( !token )
		{
			environment.AddError( "Could not find the evaluating token." );
			break;
		}
		if( syntax == SYNTAX_UNKNOWN )
		{
			environment.AddError( "Unable to analyze syntax." );
			break;
		}

		// Create the appropriate construct based upon the token type and the context in which it is being used.
		switch( token->type )
		{
			case Token::TYPE_STRING:
			{
				evaluator = CreateString( token, environment );
				break;
			}
			case Token::TYPE_NUMBER:
			case Token::TYPE_ABSTRACT_NUMBER:
			{
				evaluator = CreateConstant( token, environment );
				break;
			}
			case Token::TYPE_NAME:
			{
				if( syntax == SYNTAX_FUNCTION_CALL )
					evaluator = CreateFunction( tokenizedStatement, environment );
				else if( syntax == SYNTAX_NUMBER )
					evaluator = CreateVariable( token, environment );
				break;
			}
			case Token::TYPE_OPERATOR:
			{
				if( syntax == SYNTAX_BINARY_OPERATOR )
				{
					Utilities::List leftSubTokenizedStatement, rightSubTokenizedStatement;
					tokenizedStatement.Split( token, &leftSubTokenizedStatement, &rightSubTokenizedStatement, Utilities::List::ITEM_PIVOT_TYPE_PART_OF_NEITHER_LIST );
					evaluator = CreateBinaryOperator( token, leftSubTokenizedStatement, rightSubTokenizedStatement, environment );
					leftSubTokenizedStatement.RemoveAll( true );
					rightSubTokenizedStatement.RemoveAll( true );
				}
				else if( syntax == SYNTAX_LEFT_UNARY_OPERATOR )
				{
					Utilities::List subTokenizedStatement;
					tokenizedStatement.Split( token, 0, &subTokenizedStatement, Utilities::List::ITEM_PIVOT_TYPE_PART_OF_NEITHER_LIST );
					evaluator = CreateUnaryOperator( token, subTokenizedStatement, environment );
					subTokenizedStatement.RemoveAll( true );
				}
				else if( syntax == SYNTAX_RIGHT_UNARY_OPERATOR )
				{
					Utilities::List subTokenizedStatement;
					tokenizedStatement.Split( token, &subTokenizedStatement, 0, Utilities::List::ITEM_PIVOT_TYPE_PART_OF_NEITHER_LIST );
					evaluator = CreateUnaryOperator( token, subTokenizedStatement, environment );
					subTokenizedStatement.RemoveAll( true );
				}
				break;
			}
		}
	}
	while( false );

	tokenizedStatement.RemoveAll( true );

	return evaluator;
}

//=========================================================================================
// Scanning from left to right, find the first token of the given type.
Token* Parser::FindFirstTokenOfType( Utilities::List& listOfTokens, Token::Type tokenType, bool skipSubExpressions )
{
	Token* foundToken = 0;
	for( Token* token = ( Token* )listOfTokens.LeftMost(); token && !foundToken; token = ( Token* )token->Right() )
	{
		if( skipSubExpressions && token->type == Token::TYPE_LEFT_PARAN )
		{
			token = FindMatchingParenthesis( token );
			if( !token )
				break;
		}
		if( token->type == tokenType )
			foundToken = token;
	}
	return foundToken;
}

//=========================================================================================
// The evaluating token is the token that takes the least precedence of all others in the given statement.
Token* Parser::FindEvaluatingToken( Utilities::List& tokenizedStatement, Environment& environment, Syntax& syntax )
{
	syntax = SYNTAX_UNKNOWN;

	// If there is only one token in the list, then it is the evaluating token.
	if( tokenizedStatement.Count() == 1 )
	{
		syntax = SYNTAX_NUMBER;
		return ( Token* )tokenizedStatement.LeftMost();
	}

	// If there is more than one token in the list, then we're looking for a token
	// representing something that acts on other tokens.

	// A function takes least precedence if it encompasses the entire statement.
	if( tokenizedStatement.Count() >= 3 )
	{
		Token* functionToken = ( Token* )tokenizedStatement.LeftMost();
		if( functionToken->type == Token::TYPE_NAME )
		{
			Token* leftParan = ( Token* )functionToken->Right();
			if( leftParan->type == Token::TYPE_LEFT_PARAN )
			{
				Token* rightParan = FindMatchingParenthesis( leftParan );
				if( rightParan && rightParan == ( Token* )tokenizedStatement.RightMost() )
				{
					syntax = SYNTAX_FUNCTION_CALL;
					return functionToken;
				}
			}
		}
	}

	// Look for a binary operator of least precedence.
	int leastPrecedenceLevel = -1;
	Token* leftMostOperatorToken = 0;
	Token* rightMostOperatorToken = 0;
	for( Token* token = ( Token* )tokenizedStatement.LeftMost(); token; token = ( Token* )token->Right() )
	{
		// Skip over all sub-expressions, because they take higher precedence.
		if( token->type == Token::TYPE_LEFT_PARAN )
		{
			token = FindMatchingParenthesis( token );
			if( !token )
			{
				environment.AddError( "Unbalanced parenthesis encountered!" );
				return 0;
			}
			continue;
		}

		// Filter for binary operators.
		syntax = OperatorType( token );
		if( syntax != SYNTAX_BINARY_OPERATOR )
			continue;

		int precedenceLevel = BinaryOperatorPrecedence( token );
		if( precedenceLevel == -1 )
			continue;

		if( leastPrecedenceLevel == -1 || precedenceLevel < leastPrecedenceLevel )
		{
			leastPrecedenceLevel = precedenceLevel;
			leftMostOperatorToken = token;
			rightMostOperatorToken = token;
		}
		else if( precedenceLevel == leastPrecedenceLevel )
			rightMostOperatorToken = token;
	}

	// If we found a binary operator of least precedence, then
	// we need to choose from among all operators having this
	// precedence level.
	if( leftMostOperatorToken )
	{
		Associativity assoc = OperatorAssociativity( leftMostOperatorToken );
		if( assoc == ASSOC_LEFT_TO_RIGHT || assoc == ASSOC_DOESNT_MATTER )
		{
			syntax = SYNTAX_BINARY_OPERATOR;
			return rightMostOperatorToken;
		}
		if( assoc == ASSOC_RIGHT_TO_LEFT )
		{
			syntax = SYNTAX_BINARY_OPERATOR;
			return leftMostOperatorToken;
		}
	}

	// Lastly, look for a unary operator.  We should only ever
	// find at most one of each of a left and right unary operator type.
	Token* leftUnaryOperator = 0;
	Token* rightUnaryOperator = 0;
	for( Token* token = ( Token* )tokenizedStatement.LeftMost(); token; token = ( Token* )token->Right() )
	{
		// Skip over all sub-expressions, because they take higher precedence.
		if( token->type == Token::TYPE_LEFT_PARAN )
		{
			token = FindMatchingParenthesis( token );
			if( !token )
			{
				environment.AddError( "Unbalanced parenthesis encountered!" );
				return 0;
			}
			continue;
		}

		syntax = OperatorType( token );
		if( syntax == SYNTAX_LEFT_UNARY_OPERATOR )
			leftUnaryOperator = token;
		else if( syntax == SYNTAX_RIGHT_UNARY_OPERATOR )
			rightUnaryOperator = token;
	}

	// If we found some unary operators, then choose one among them.
	if( leftUnaryOperator && rightUnaryOperator )
	{
		if( UnaryOperatorPrecedence( leftUnaryOperator ) < UnaryOperatorPrecedence( rightUnaryOperator ) )
		{
			syntax = SYNTAX_LEFT_UNARY_OPERATOR;
			return leftUnaryOperator;
		}
		else
		{
			syntax = SYNTAX_RIGHT_UNARY_OPERATOR;
			return rightUnaryOperator;
		}
	}
	else if( leftUnaryOperator )
	{
		syntax = SYNTAX_LEFT_UNARY_OPERATOR;
		return leftUnaryOperator;
	}
	else if( rightUnaryOperator )
	{
		syntax = SYNTAX_RIGHT_UNARY_OPERATOR;
		return rightUnaryOperator;
	}

	// If we got here, then the caller's statement was unintelligable to us.
	return 0;
}

//=========================================================================================
// We assume here that the given token is a binary operator.
int Parser::BinaryOperatorPrecedence( Token* token )
{
	if( token->type != Token::TYPE_OPERATOR )
		return -1;

	if( 0 == strcmp( token->string, "=" ) )
		return 0;
	if( 0 == strcmp( token->string, "|" ) )
		return 1;
	if( 0 == strcmp( token->string, "&" ) )
		return 2;
	if( 0 == strcmp( token->string, "<" ) || 0 == strcmp( token->string, "<=" ) )
		return 3;
	if( 0 == strcmp( token->string, ">" ) || 0 == strcmp( token->string, ">=" ) )
		return 3;
	if( 0 == strcmp( token->string, "==" ) )
		return 3;
	if( 0 == strcmp( token->string, "+" ) || 0 == strcmp( token->string, "-" ) )
		return 4;
	if( 0 == strcmp( token->string, "*" ) || 0 == strcmp( token->string, "/" ) )
		return 5;
	if( 0 == strcmp( token->string, "^" ) )		// This is exponentiation or the outer product.
		return 6;
	if( 0 == strcmp( token->string, "." ) )		// This is the inner product.
		return 7;
	if( 0 == strcmp( token->string, ":" ) )
		return 8;

	return -1;
}

//=========================================================================================
// We assume here that the given token is a unary operator.
int Parser::UnaryOperatorPrecedence( Token* token )
{
	if( token->type != Token::TYPE_OPERATOR )
		return -1;

	// Precedence levels here tell us which operators bind more tightly then others
	// in the case that we find both a left and right unary operator applied to something.
	// Parentheses must be used if two or more left/right operators are applied to something.
	if( 0 == strcmp( token->string, "-" ) || 0 == strcmp( token->string, "~" ) )
		return 0;
	if( 0 == strcmp( token->string, "!" ) )
		return 1;
	if( 0 == strcmp( token->string, "#" ) )
		return 2;

	return -1;
}

//=========================================================================================
// We assume here that the given token is a binary operator.
Parser::Associativity Parser::OperatorAssociativity( Token* token )
{
	if( token->type != Token::TYPE_OPERATOR )
		return ASSOC_UNKNOWN;

	if( 0 == strcmp( token->string, "+" ) )
		return ASSOC_DOESNT_MATTER;
	if( 0 == strcmp( token->string, "-" ) )
		return ASSOC_LEFT_TO_RIGHT;
	if( 0 == strcmp( token->string, "*" ) )
		return ASSOC_DOESNT_MATTER;
	if( 0 == strcmp( token->string, "/" ) )
		return ASSOC_LEFT_TO_RIGHT;
	if( 0 == strcmp( token->string, "^" ) )
		return ASSOC_RIGHT_TO_LEFT;
	if( 0 == strcmp( token->string, "." ) )
		return ASSOC_LEFT_TO_RIGHT;
	if( 0 == strcmp( token->string, "=" ) )
		return ASSOC_RIGHT_TO_LEFT;
	if( 0 == strcmp( token->string, "<" ) || 0 == strcmp( token->string, "<=" ) )
		return ASSOC_LEFT_TO_RIGHT;
	if( 0 == strcmp( token->string, ">" ) || 0 == strcmp( token->string, ">=" ) )
		return ASSOC_LEFT_TO_RIGHT;
	if( 0 == strcmp( token->string, "==" ) )
		return ASSOC_LEFT_TO_RIGHT;
	if( 0 == strcmp( token->string, "&" ) || 0 == strcmp( token->string, "|" ) )
		return ASSOC_LEFT_TO_RIGHT;
	if( 0 == strcmp( token->string, ":" ) )
		return ASSOC_LEFT_TO_RIGHT;

	return ASSOC_UNKNOWN;
}

//=========================================================================================
// The parser needs to know how to correctly distinguish
// between unary and binary operators, because the '-'
// character is overloaded with both types.
Parser::Syntax Parser::OperatorType( Token* token )
{
	if( token->type != Token::TYPE_OPERATOR )
		return SYNTAX_UNKNOWN;

	if( 0 == strcmp( token->string, "=" ) )
		return SYNTAX_BINARY_OPERATOR;
	if( 0 == strcmp( token->string, "+" ) )
		return SYNTAX_BINARY_OPERATOR;
	if( 0 == strcmp( token->string, "*" ) || 0 == strcmp( token->string, "/" ) )
		return SYNTAX_BINARY_OPERATOR;
	if( 0 == strcmp( token->string, "^" ) )
		return SYNTAX_BINARY_OPERATOR;
	if( 0 == strcmp( token->string, "." ) )
		return SYNTAX_BINARY_OPERATOR;
	if( 0 == strcmp( token->string, "~" ) )
		return SYNTAX_RIGHT_UNARY_OPERATOR;
	if( 0 == strcmp( token->string, "!" ) )
		return SYNTAX_RIGHT_UNARY_OPERATOR;
	if( 0 == strcmp( token->string, "#" ) )
		return SYNTAX_LEFT_UNARY_OPERATOR;
	if( 0 == strcmp( token->string, "<" ) || 0 == strcmp( token->string, "<=" ) )
		return SYNTAX_BINARY_OPERATOR;
	if( 0 == strcmp( token->string, ">" ) || 0 == strcmp( token->string, ">=" ) )
		return SYNTAX_BINARY_OPERATOR;
	if( 0 == strcmp( token->string, "==" ) )
		return SYNTAX_BINARY_OPERATOR;
	if( 0 == strcmp( token->string, "&" ) || 0 == strcmp( token->string, "|" ) )
		return SYNTAX_BINARY_OPERATOR;
	if( 0 == strcmp( token->string, ":" ) )
		return SYNTAX_BINARY_OPERATOR;

	// Handle the tricky case of the negative sign.
	if( 0 == strcmp( token->string, "-" ) )
	{
		if( !token->Left() )
			return SYNTAX_LEFT_UNARY_OPERATOR;

		Token* leftToken = ( Token* )token->Left();
		Token* rightToken = ( Token* )token->Right();
		
		bool leftIsSubject = ( leftToken->type == Token::TYPE_NUMBER || leftToken->type == Token::TYPE_NAME || leftToken->type == Token::TYPE_RIGHT_PARAN );
		bool rightIsSubject = ( rightToken->type == Token::TYPE_NUMBER || rightToken->type == Token::TYPE_NAME || rightToken->type == Token::TYPE_LEFT_PARAN );

		if( leftIsSubject && rightIsSubject )
			return SYNTAX_BINARY_OPERATOR;
		
		if( leftIsSubject )
			return SYNTAX_BINARY_OPERATOR;
		
		if( rightIsSubject )
			return SYNTAX_LEFT_UNARY_OPERATOR;
	}

	return SYNTAX_UNKNOWN;
}

//=========================================================================================
Evaluator* Parser::CreateString( Token* token, Environment& environment )
{
	StringEvaluator* string = new StringEvaluator();
	string->String( token->string );
	return string;
}

//=========================================================================================
Evaluator* Parser::CreateConstant( Token* token, Environment& environment )
{
	Number* number = environment.CreateNumber(0);

	if( !number->AssignFrom( token->string, environment ) )
	{
		environment.AddError( "Failed to create number from \"%s\".", token->string );
		delete number;
		return 0;
	}

	ConstantEvaluator* constant = new ConstantEvaluator( number );
	return constant;
}

//=========================================================================================
Evaluator* Parser::CreateVariable( Token* token, Environment& environment )
{
	Evaluator* variable = 0;
	
	// Give the environment a chance to create the variable first.
	// Some variables are actually special constants known to the environment.
	variable = environment.CreateVariable( token->string );
	if( !variable )
		variable = new VariableEvaluator( token->string );

	return variable;
}

//=========================================================================================
Evaluator* Parser::CreateBinaryOperator( Token* token, Utilities::List& leftSubTokenizedStatement, Utilities::List& rightSubTokenizedStatement, Environment& environment )
{
	bool success = false;
	Evaluator* leftOperand = 0, *rightOperand = 0;
	Evaluator* evaluator = 0;
	bool isBinaryOperationEvaluator = true;

	do
	{
		leftOperand = CreateEvaluationTree( leftSubTokenizedStatement, environment );
		if( !leftOperand )
		{
			environment.AddError( "Failed to create left operand for operator \"%s\".", token->string );
			break;
		}

		rightOperand = CreateEvaluationTree( rightSubTokenizedStatement, environment );
		if( !rightOperand )
		{
			environment.AddError( "Failed to create right operand for binary operator \"%s\".", token->string );
			break;
		}

		// Give the environment a chance to create the operator first.
		evaluator = environment.CreateBinaryOperator( token->string, leftOperand, rightOperand, isBinaryOperationEvaluator );
		if( !evaluator )
		{
			isBinaryOperationEvaluator = true;

			// Is it an arithmetic operation?
			BinaryArithmeticOperationEvaluator::ArithmeticOperation arithmeticOperation = BinaryArithmeticOperationEvaluator::ARITHMETIC_OPERATION_UNKNOWN;
			if( 0 == strcmp( token->string, "+" ) )
				arithmeticOperation = BinaryArithmeticOperationEvaluator::ARITHMETIC_OPERATION_ADD;
			else if( 0 == strcmp( token->string, "-" ) )
				arithmeticOperation = BinaryArithmeticOperationEvaluator::ARITHMETIC_OPERATION_SUBTRACT;
			else if( 0 == strcmp( token->string, "*" ) )
				arithmeticOperation = BinaryArithmeticOperationEvaluator::ARITHMETIC_OPERATION_MULTIPLY;
			else if( 0 == strcmp( token->string, "/" ) )
				arithmeticOperation = BinaryArithmeticOperationEvaluator::ARITHMETIC_OPERATION_DIVIDE;

			// Is it a comparison operation?
			BinaryComparisonOperationEvaluator::ComparisonOperation comparisonOperation = BinaryComparisonOperationEvaluator::COMPARE_OPERATION_UNKNOWN;
			if( 0 == strcmp( token->string, "<" ) )
				comparisonOperation = BinaryComparisonOperationEvaluator::COMPARE_OPERATION_LESS_THAN;
			else if( 0 == strcmp( token->string, "<=" ) )
				comparisonOperation = BinaryComparisonOperationEvaluator::COMPARE_OPERATION_LESS_THAN_OR_EQUAL_TO;
			else if( 0 == strcmp( token->string, ">" ) )
				comparisonOperation = BinaryComparisonOperationEvaluator::COMPARE_OPERATION_GREATER_THAN;
			else if( 0 == strcmp( token->string, ">=" ) )
				comparisonOperation = BinaryComparisonOperationEvaluator::COMPARE_OPERATION_GREATER_THAN_OR_EQUAL_TO;
			else if( 0 == strcmp( token->string, "==" ) )
				comparisonOperation = BinaryComparisonOperationEvaluator::COMPARE_OPERATION_EQUAL_TO;

			// Is it a logic operation?
			BinaryLogicOperationEvaluator::LogicOperation logicOperation = BinaryLogicOperationEvaluator::LOGIC_OPERATION_UNKOWN;
			if( 0 == strcmp( token->string, "&" ) )
				logicOperation = BinaryLogicOperationEvaluator::LOGIC_OPERATION_AND;
			else if( 0 == strcmp( token->string, "|" ) )
				logicOperation = BinaryLogicOperationEvaluator::LOGIC_OPERATION_INCLUSIVE_OR;

			// Create the operation evaluator.
			if( arithmeticOperation != BinaryArithmeticOperationEvaluator::ARITHMETIC_OPERATION_UNKNOWN )
			{
				BinaryArithmeticOperationEvaluator* binaryArithmeticEvaluator = new BinaryArithmeticOperationEvaluator( arithmeticOperation );
				evaluator = binaryArithmeticEvaluator;
			}
			else if( comparisonOperation != BinaryComparisonOperationEvaluator::COMPARE_OPERATION_UNKNOWN )
			{
				BinaryComparisonOperationEvaluator* binaryComparisonEvaluator = new BinaryComparisonOperationEvaluator( comparisonOperation );
				evaluator = binaryComparisonEvaluator;
			}
			else if( logicOperation != BinaryLogicOperationEvaluator::LOGIC_OPERATION_UNKOWN )
			{
				BinaryLogicOperationEvaluator* binaryLogicOperationEvaluator = new BinaryLogicOperationEvaluator( logicOperation );
				evaluator = binaryLogicOperationEvaluator;
			}
			else if( 0 == strcmp( token->string, "=" ) )
				evaluator = new BinaryAssignmentOperationEvaluator();
			else if( 0 == strcmp( token->string, ":" ) )
				evaluator = new BinaryIndexingOperationEvaluator();
			else
			{
				environment.AddError( "Cannot create binary operator \"%s\".", token->string );
				break;
			}
		}

		success = true;
	}
	while( false );

	if( !success )
	{
		delete leftOperand;
		delete rightOperand;
		delete evaluator;
		evaluator = 0;
	}
	else if( isBinaryOperationEvaluator )
	{
		BinaryOperationEvaluator* binaryOperationEvaluator = ( BinaryOperationEvaluator* )evaluator;
		binaryOperationEvaluator->LeftOperand( leftOperand );
		binaryOperationEvaluator->RightOperand( rightOperand );
	}

	return evaluator;
}

//=========================================================================================
Evaluator* Parser::CreateUnaryOperator( Token* token, Utilities::List& subTokenizedStatement, Environment& environment )
{
	bool success = false;
	Evaluator* operand = 0;
	Evaluator* evaluator = 0;
	bool isUnaryOperationEvaluator = true;

	do
	{
		operand = CreateEvaluationTree( subTokenizedStatement, environment );
		if( !operand )
		{
			environment.AddError( "Failed to create operand for unary operator \"%s\".", token->string );
			break;
		}

		// Give the environment a chance to create the operator first.
		evaluator = environment.CreateUnaryOperator( token->string, operand, isUnaryOperationEvaluator );
		if( !evaluator )
		{
			isUnaryOperationEvaluator = true;

			UnaryArithmeticOperationEvaluator::ArithmeticOperation arithmeticOperation = UnaryArithmeticOperationEvaluator::ARITHMETIC_OPERATOR_UNKNOWN;
			if( 0 == strcmp( token->string, "-" ) )
				arithmeticOperation = UnaryArithmeticOperationEvaluator::ARITHMETIC_OPERATOR_NEGATE;
			else if( 0 == strcmp( token->string, "~" ) )
				arithmeticOperation = UnaryArithmeticOperationEvaluator::ARITHMETIC_OPERATOR_INVERT;
			
			if( arithmeticOperation != UnaryArithmeticOperationEvaluator::ARITHMETIC_OPERATOR_UNKNOWN )
			{
				UnaryArithmeticOperationEvaluator* unaryArithmeticEvaluator = new UnaryArithmeticOperationEvaluator( arithmeticOperation );
				evaluator = unaryArithmeticEvaluator;
			}
			else if( 0 == strcmp( token->string, "#" ) )
			{
				UnaryStringExecutionEvaluator* unaryStringExecutionEvaluator = new UnaryStringExecutionEvaluator();
				evaluator = unaryStringExecutionEvaluator;
			}
			else
			{
				environment.AddError( "Cannot create unary operator \"%s\".", token->string );
				break;
			}
		}

		success = true;
	}
	while( false );

	if( !success )
	{
		delete operand;
		delete evaluator;
		evaluator = 0;
	}
	else if( isUnaryOperationEvaluator )
	{
		UnaryOperationEvaluator* unaryOperationEvaluator = ( UnaryOperationEvaluator* )evaluator;
		unaryOperationEvaluator->Operand( operand );
	}

	return evaluator;
}

//=========================================================================================
Evaluator* Parser::CreateFunction( Utilities::List& functionCallTokenizedStatement, Environment& environment )
{
	bool success = false;
	FunctionEvaluator* evaluator = 0;
	const char* functionName = 0;

	do
	{
		if( functionCallTokenizedStatement.Count() < 3 )
			break;

		// Determine which function to create and then call upon the environment to create it.
		Token* functionToken = ( Token* )functionCallTokenizedStatement.LeftMost();
		functionName = functionToken->string;
		evaluator = environment.CreateFunction( functionName );
		if( !evaluator )
		{
			environment.AddError( "Failed to create function \"%s\" from the environment.", functionName );
			break;
		}

		// Some functions don't take any arguments.
		// If this one does, then go soak up those arguments.
		functionCallTokenizedStatement.Remove( functionToken, true );
		RemoveOuterParentheses( functionCallTokenizedStatement );
		if( functionCallTokenizedStatement.Count() > 0 )
		{
			if( !CreateFunctionArguments( evaluator, functionCallTokenizedStatement, environment ) )
				break;
		}

		success = true;
	}
	while( false );

	if( !success )
	{
		if( evaluator )
			delete evaluator;
		evaluator = 0;
	}

	functionCallTokenizedStatement.RemoveAll( true );

	return evaluator;
}

//=========================================================================================
// I probably shouldn't have made this recursive.  Anyone going to make a big script?
bool Parser::CreateFunctionArguments( FunctionEvaluator* evaluator, Utilities::List& tokenizedListOfArguments, Environment& environment )
{
	// It's okay to encounter an empty statement.  Either the function
	// doesn't take any arguments, or a comma follows the final argument,
	// which is a convenient syntax to allow, especially for the do()
	// function, since it lets us put commas at the end of each "statement",
	// even the last statement.
	if( tokenizedListOfArguments.Count() == 0 )
		return true;

	bool success = false;

	do
	{
		Evaluator* argument = 0;

		Token* delimeterToken = FindFirstTokenOfType( tokenizedListOfArguments, Token::TYPE_DELIMETER, true );
		if( !delimeterToken )
		{
			argument = CreateEvaluationTree( tokenizedListOfArguments, environment );
			if( !argument )
				break;
		}
		else
		{
			Utilities::List leftTokenizedList, rightTokenizedList;
			tokenizedListOfArguments.Split( delimeterToken, &leftTokenizedList, &rightTokenizedList, Utilities::List::ITEM_PIVOT_TYPE_PART_OF_NEITHER_LIST );

			argument = CreateEvaluationTree( leftTokenizedList, environment );
			if( !argument )
				break;

			if( !CreateFunctionArguments( evaluator, rightTokenizedList, environment ) )
				break;
		}

		FunctionArgumentEvaluator* functionArgumentEvaluator = new FunctionArgumentEvaluator();
		functionArgumentEvaluator->Argument( argument );

		// It is important that we preserve the order of the arguments so that they can be evaluated in
		// the right order when the time comes.  This is mainly for the do() function.
		evaluator->AddArgument( functionArgumentEvaluator, FunctionEvaluator::PREPEND_TO_LIST_OF_ARGS );

		success = true;
	}
	while( false );

	tokenizedListOfArguments.RemoveAll( true );

	return success;
}

//=========================================================================================
void Parser::RemoveOuterParentheses( Utilities::List& listOfTokens )
{
	if( listOfTokens.Count() > 0 )
	{
		bool parenthesesRemoved;
		do
		{
			parenthesesRemoved = false;
			Token* leftParan = ( Token* )listOfTokens.LeftMost();
			if( leftParan->type == Token::TYPE_LEFT_PARAN )
			{
				Token* rightParan = FindMatchingParenthesis( leftParan );
				if( rightParan == ( Token* )listOfTokens.RightMost() )
				{
					listOfTokens.Remove( leftParan, true );
					listOfTokens.Remove( rightParan, true );
					parenthesesRemoved = true;
				}
			}
		}
		while( parenthesesRemoved && listOfTokens.Count() > 0 );
	}
}

//=========================================================================================
Token* Parser::FindMatchingParenthesis( Token* token )
{
	Token* foundToken = 0;
	if( token->type == Token::TYPE_LEFT_PARAN )
	{
		int nestingLevel = 0;
		do
		{
			if( token->type == Token::TYPE_LEFT_PARAN )
				nestingLevel++;
			else if( token->type == Token::TYPE_RIGHT_PARAN )
			{
				nestingLevel--;
				if( nestingLevel == 0 )
				{
					foundToken = token;
					break;
				}
			}
			token = ( Token* )token->Right();
		}
		while( token );
	}
	else if( token->type == Token::TYPE_RIGHT_PARAN )
	{
		//...
	}
	return foundToken;
}

// Parser.cpp