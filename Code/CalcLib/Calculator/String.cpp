// String.cpp

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

IMPLEMENT_CALCLIB_CLASS1( StringNumber, Number );

Tokenizer StringNumber::tokenizer;
Parser StringNumber::parser;
int StringNumber::recursionLevel = 0;

//=========================================================================================
StringNumber::StringNumber( void )
{
	string = 0;
	cachedEvaluationTree = 0;
	cacheIsValid = false;
}

//=========================================================================================
/*virtual*/ StringNumber::~StringNumber( void )
{
	if( string )
		delete[] string;
	if( cachedEvaluationTree )
		delete cachedEvaluationTree;
}

//=========================================================================================
const char* StringNumber::String( void ) const
{
	// Hmmm, hopefully they won't delete the pointer.
	return string;
}

//=========================================================================================
/*virtual*/ Number* StringNumber::CreateCopy( Environment& environment ) const
{
	StringNumber* stringNumber = new StringNumber();
	stringNumber->AssignFrom( string, environment );
	return stringNumber;
}

//=========================================================================================
bool StringNumber::Execute( Number& result, Environment& environment ) const
{
	bool success = false;
	recursionLevel++;

	do
	{
		if( recursionLevel > 10 )
		{
			environment.AddError( "Bailing out to prevent possible infinite recursion." );
			break;
		}

		if( !RefreshCache( environment ) )
		{
			environment.AddError( "Failed to refresh cache for string number \"%s\".", string );
			break;
		}

		if( !cachedEvaluationTree->EvaluateResult( result, environment ) )
		{
			environment.AddError( "Failed to evaluate cached evaluation tree for \"%s\".", string );
			break;
		}

		success = true;
	}
	while( false );

	recursionLevel--;
	return success;
}

//=========================================================================================
/*virtual*/ bool StringNumber::Print( char* buffer, int bufferSize, bool printLatex, Environment& environment ) const
{
	strcpy_s( buffer, bufferSize, string );
	return true;
}

//=========================================================================================
bool StringNumber::RefreshCache( Environment& environment ) const
{
	if( !cacheIsValid )
	{
		// If there is an old evaluation tree, destroy it before creating a new one.
		WipeCache();
		
		Utilities::List listOfTokens;
		if( !tokenizer.Tokenize( string, listOfTokens, environment ) )
		{
			environment.AddError( "The string \"%s\" does not tokenize.", string );
			return false;
		}

		if( !parser.Parse( listOfTokens, cachedEvaluationTree, environment ) )
		{
			environment.AddError( "The string \"%s\" does not parse.", string );
			return false;
		}

		cacheIsValid = true;
	}

	return true;
}

//=========================================================================================
void StringNumber::WipeCache( void ) const
{
	if( cachedEvaluationTree )
	{
		delete cachedEvaluationTree;
		cachedEvaluationTree = 0;
	}
	
	InvalidateCache();
}

//=========================================================================================
void StringNumber::InvalidateCache( void ) const
{
	cacheIsValid = false;
}

//=========================================================================================
/*virtual*/ bool StringNumber::AssignFrom( const char* numberString, Environment& environment )
{
	if( string )
		delete[] string;
	
	WipeCache();

	int length = strlen( numberString ) + 1;
	string = new char[ length ];
	strcpy_s( string, length, numberString );

	return true;
}

//=========================================================================================
/*virtual*/ bool StringNumber::AssignFrom( const Number* number, Environment& environment )
{
	if( number->IsTypeOf( StringNumber::ClassName() ) )
	{
		StringNumber* stringNumber = ( StringNumber* )number;
		if( !AssignFrom( stringNumber->string, environment ) )
			return false;
	}
	else
	{
		char stringBuffer[ 512 ];
		if( !number->Print( stringBuffer, sizeof( stringBuffer ), false, environment ) )
			return false;

		if( !AssignFrom( stringBuffer, environment ) )
			return false;
	}

	return true;
}

//=========================================================================================
/*virtual*/ bool StringNumber::AssignFrom( double number, Environment& environment )
{
	char stringBuffer[ 512 ];
	sprintf_s( stringBuffer, sizeof( stringBuffer ), "%f", number );

	if( !AssignFrom( stringBuffer, environment ) )
		return false;

	return true;
}

//=========================================================================================
/*virtual*/ bool StringNumber::AssignTo( Number& number, Environment& environment ) const
{
	if( number.IsTypeOf( StringNumber::ClassName() ) )
	{
		if( !number.AssignFrom( this, environment ) )
			return false;
	}
	
	if( !Execute( number, environment ) )
		return false;

	return true;
}

//=========================================================================================
/*virtual*/ bool StringNumber::AssignTo( double& number, Environment& environment ) const
{
	return false;
}

//=========================================================================================
/*virtual*/ bool StringNumber::AssignSum( const Number* left, const Number* right, Environment& environment )
{
	return false;
}

//=========================================================================================
/*virtual*/ bool StringNumber::AssignDifference( const Number* left, const Number* right, Environment& environment )
{
	return false;
}

//=========================================================================================
/*virtual*/ bool StringNumber::AssignProduct( const Number* left, const Number* right, Environment& environment )
{
	return false;
}

//=========================================================================================
/*virtual*/ bool StringNumber::AssignQuotient( const Number* left, const Number* right, Environment& environment )
{
	return false;
}

//=========================================================================================
/*virtual*/ bool StringNumber::AssignAdditiveIdentity( Environment& environment )
{
	return false;
}

//=========================================================================================
/*virtual*/ bool StringNumber::AssignMultiplicativeIdentity( Environment& environment )
{
	return false;
}

//=========================================================================================
/*virtual*/ bool StringNumber::IsAdditiveIdentity( bool& isAdditiveIdentity, Environment& environment ) const
{
	return false;
}

//=========================================================================================
/*virtual*/ bool StringNumber::IsMultiplicativeIdentity( bool& isMultiplicativeIdentity, Environment& environment ) const
{
	return false;
}

// String.cpp