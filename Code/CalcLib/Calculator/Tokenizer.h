// Tokenizer.h

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
// TODO: It would be nice if the tokenizer could recognize and skip over comments.
class CalcLib::Token : public Utilities::List::Item
{
public:

	enum Type
	{
		TYPE_UNKNOWN,
		TYPE_NUMBER,
		TYPE_ABSTRACT_NUMBER,
		TYPE_OPERATOR,
		TYPE_NAME,
		TYPE_LEFT_PARAN,
		TYPE_RIGHT_PARAN,
		TYPE_DELIMETER,
		TYPE_STRING,
	};

	Token( void );
	virtual ~Token( void );

	char* string;
	Type type;
	int lineNumber;
};

//=========================================================================================
class CalcLib::Tokenizer
{
public:

	Tokenizer( void );
	virtual ~Tokenizer( void );

	bool Tokenize( const char* mathExpression, Utilities::List& listOfTokens, Environment& environment );

private:

	// TODO: Change this to a set of OR-able flags and fix the bugs in the tokenizer.
	enum TokenConsumption
	{
		CONSUME_NON_WHITE_SPACE,
		CONSUME_NUMERICAL_ONLY,
		CONSUME_ALPHABETICAL_AND_NUMERICAL_ONLY,
		CONSUME_SINGLE_CHARACTER,
		CONSUME_TWO_CHARACETERS,
		CONSUME_STRING,
	};

	bool IsWhiteSpace( const char* parsePosition );
	const char* EatWhiteSpace( const char*& parsePosition, int& lineNumber );
	Token* ConsumeToken( const char*& parsePosition );
	bool IsOperatorToken( const char* parsePosition );
	char* CreateTokenString( const char*& parsePosition, TokenConsumption tokenConsumption );
	bool CanConsumeCharacter( char character, int consumptionCount, char initialCharacter, TokenConsumption tokenConsumption );
};

// Tokenizer.h