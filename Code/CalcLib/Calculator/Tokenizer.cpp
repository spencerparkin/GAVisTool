// Tokenizer.cpp

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
Token::Token( void )
{
	string = 0;
	type = TYPE_UNKNOWN;
	lineNumber = -1;
}

//=========================================================================================
/*virtual*/ Token::~Token( void )
{
	if( string )
		delete[] string;
}

//=========================================================================================
Tokenizer::Tokenizer( void )
{
}

//=========================================================================================
/*virtual*/ Tokenizer::~Tokenizer( void )
{
}

//=========================================================================================
bool Tokenizer::Tokenize( const char* mathExpression, Utilities::List& listOfTokens, Environment& environment )
{
	int lineNumber = 1;
	const char* parsePosition = mathExpression;
	while( *parsePosition != '\0' )
	{
		parsePosition = EatWhiteSpace( parsePosition, lineNumber );
		if( *parsePosition == '\0' )
			break;
		Token* token = ConsumeToken( parsePosition );
		if( token )
		{
			token->lineNumber = lineNumber;
			listOfTokens.InsertRightOf( listOfTokens.RightMost(), token );
		}
		else
		{
			environment.AddError( "Failed to recognize token starting at: \"%s\".", parsePosition );
			return false;
		}
	}
	return true;
}

//=========================================================================================
bool Tokenizer::IsWhiteSpace( const char* parsePosition )
{
	if( *parsePosition == ' ' || *parsePosition == '\n' || *parsePosition == '\t' || *parsePosition == '\a' || *parsePosition == '\r' )
		return true;
	return false;
}

//=========================================================================================
const char* Tokenizer::EatWhiteSpace( const char*& parsePosition, int& lineNumber )
{
	bool commentEaten = false;

	do
	{
		while( IsWhiteSpace( parsePosition ) )
		{
			if( *parsePosition == '\n' )
				lineNumber++;
			parsePosition++;
		}

		// We also want to treat C-style comments as white space, and skip over those too.
		commentEaten = false;
		if( parsePosition[0] == '/' && parsePosition[1] == '*' )
		{
			// An unbalanced comment won't create an error if it happens
			// at the end of the script, but I wouldn't recomment making
			// that a practice.
			commentEaten = true;
			while( *parsePosition != '\0' )
			{
				if( parsePosition[0] == '*' && parsePosition[1] == '/' )
				{
					parsePosition += 2;
					break;
				}
				parsePosition++;
			}
		}
	}
	while( commentEaten );

	return parsePosition;
}

//=========================================================================================
Token* Tokenizer::ConsumeToken( const char*& parsePosition )
{
	Token* token = 0;
	Token::Type type = Token::TYPE_UNKNOWN;
	TokenConsumption tokenConsumption;

	// The leading character of a token determines the entire token.
	if( isdigit( *parsePosition ) )
	{
		type = Token::TYPE_NUMBER;
		tokenConsumption = CONSUME_ALPHABETICAL_AND_NUMERICAL_ONLY;
	}
	else if( isalpha( *parsePosition ) )
	{
		type = Token::TYPE_NAME;
		tokenConsumption = CONSUME_ALPHABETICAL_AND_NUMERICAL_ONLY;
	}
	else if( IsOperatorToken( parsePosition ) )
	{
		type = Token::TYPE_OPERATOR;
		tokenConsumption = CONSUME_SINGLE_CHARACTER;
		if( ( *parsePosition == '<' || *parsePosition == '>' || *parsePosition == '=' ) && parsePosition[1] == '=' )
			tokenConsumption = CONSUME_TWO_CHARACETERS;
	}
	else if( *parsePosition == '(' )
	{
		type = Token::TYPE_LEFT_PARAN;
		tokenConsumption = CONSUME_SINGLE_CHARACTER;
	}
	else if( *parsePosition == ')' )
	{
		type = Token::TYPE_RIGHT_PARAN;
		tokenConsumption = CONSUME_SINGLE_CHARACTER;
	}
	else if( *parsePosition == ',' )
	{
		type = Token::TYPE_DELIMETER;
		tokenConsumption = CONSUME_SINGLE_CHARACTER;
	}
	else if( *parsePosition == '"' )
	{
		type = Token::TYPE_STRING;
		tokenConsumption = CONSUME_STRING;
	}
	else if( *parsePosition == '$' )
	{
		type = Token::TYPE_ABSTRACT_NUMBER;
		tokenConsumption = CONSUME_ALPHABETICAL_AND_NUMERICAL_ONLY;
	}

	if( type != Token::TYPE_UNKNOWN )
	{
		token = new Token;
		token->string = CreateTokenString( parsePosition, tokenConsumption );
		token->type = type;
	}

	return token;
}

//=========================================================================================
bool Tokenizer::IsOperatorToken( const char* parsePosition )
{
	if( *parsePosition == '+' || *parsePosition == '-' || *parsePosition == '*' || *parsePosition == '/' )
		return true;
	if( *parsePosition == '^' )
		return true;
	if( *parsePosition == '.' )		// Numbers must start with a numerical character.  That is, ".5" must be written as "0.5" and "F" must be "0F_16".
		return true;
	if( *parsePosition == '~' )
		return true;
	if( *parsePosition == '#' )
		return true;
	if( *parsePosition == '=' )
		return true;
	if( *parsePosition == '<' )
		return true;
	if( *parsePosition == '>' )
		return true;
	if( *parsePosition == '&' || *parsePosition == '|' )
		return true;
	if( *parsePosition == ':' )
		return true;
	return false;
}

//=========================================================================================
char* Tokenizer::CreateTokenString( const char*& parsePosition, TokenConsumption tokenConsumption )
{
	char* string = 0;

	// Scan the token.  Calculate the length of it.
	int length = 0;
	char initialCharacter = parsePosition[0];
	while( CanConsumeCharacter( parsePosition[ length ], length, initialCharacter, tokenConsumption ) )
		length++;

	// In this case, don't capture the quotes.
	if( tokenConsumption == CONSUME_STRING )
	{
		// Consume the final quote, if any.
		if( parsePosition[ length ] == '"' )
			length++;
		int lengthWithoutQuotes = 1;
		if( length >= 2 )
			lengthWithoutQuotes = length - 2;
		string = new char[ lengthWithoutQuotes + 1 ];
		strncpy_s( string, lengthWithoutQuotes + 1, parsePosition + 1, lengthWithoutQuotes );
	}
	else
	{
		string = new char[ length + 1 ];
		strncpy_s( string, length + 1, parsePosition, length );
	}

	parsePosition += length;
	return string;
}

//=========================================================================================
bool Tokenizer::CanConsumeCharacter( char character, int consumptionCount, char initialCharacter, TokenConsumption tokenConsumption )
{
	if( character == '\0' )
		return false;

	switch( tokenConsumption )
	{
		case CONSUME_NON_WHITE_SPACE:
		{
			return !IsWhiteSpace( &character );
		}
		case CONSUME_NUMERICAL_ONLY:
		{
			if( isdigit( character ) || character == '.' || character == '_' )
				return true;
			return false;
		}
		case CONSUME_ALPHABETICAL_AND_NUMERICAL_ONLY:
		{
			if( isdigit( character ) || isalpha( character ) )
				return true;
			if( character == '_' )
				return true;
			if( isdigit( initialCharacter ) && character == '.' )
				return true;
			if( character == '$' )
				return true;
			return false;
		}
		case CONSUME_SINGLE_CHARACTER:
		{
			if( consumptionCount < 1 )
				return true;
			return false;
		}
		case CONSUME_TWO_CHARACETERS:
		{
			if( consumptionCount < 2 )
				return true;
			return false;
		}
		case CONSUME_STRING:
		{
			// If we encounter an unbalanced quote, we take the quotation to the end of the string, and report no error.
			// This will likely result in some other type of error, which at times can make finding the real error harder,
			// but I'm not too worried about that right now.
			if( character == '"' && consumptionCount > 0 )
				return false;
			return true;
		}
	}
	return false;
}

// Tokenizer.cpp