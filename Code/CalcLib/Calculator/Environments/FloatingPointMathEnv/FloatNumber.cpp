// FloatNumber.cpp

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "../../CalcLib.h"

using namespace CalcLib;

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( FloatingPointNumber, Number );

//=========================================================================================
FloatingPointNumber::FloatingPointNumber( void )
{
	value = 0.0;
}

//=========================================================================================
/*virtual*/ FloatingPointNumber::~FloatingPointNumber( void )
{
}

//=========================================================================================
/*virtual*/ Number* FloatingPointNumber::CreateCopy( Environment& environment ) const
{
	FloatingPointNumber* floatingPointNumber = new FloatingPointNumber();
	floatingPointNumber->value = value;
	return floatingPointNumber;
}

//=========================================================================================
/*virtual*/ bool FloatingPointNumber::Print( char* buffer, int bufferSize, bool printLatex, Environment& environment ) const
{
	sprintf_s( buffer, bufferSize, "%f", value );
	return true;
}

//=========================================================================================
/*virtual*/ bool FloatingPointNumber::AssignFrom( const char* numberString, Environment& environment )
{
	value = atof( numberString );
	return true;
}

//=========================================================================================
/*virtual*/ bool FloatingPointNumber::AssignFrom( const Number* number, Environment& environment )
{
	FloatingPointNumber* floatPointNumber = ( FloatingPointNumber* )number;
	value = floatPointNumber->value;
	return true;
}

//=========================================================================================
/*virtual*/ bool FloatingPointNumber::AssignFrom( double number, Environment& environment )
{
	value = number;
	return true;
}

//=========================================================================================
/*virtual*/ bool FloatingPointNumber::AssignTo( double& number, Environment& environment ) const
{
	number = value;
	return true;
}

//=========================================================================================
/*virtual*/ bool FloatingPointNumber::AssignSum( const Number* left, const Number* right, Environment& environment )
{
	FloatingPointNumber* leftFloatPointNumber = ( FloatingPointNumber* )left;
	FloatingPointNumber* rightFloatPointNumber = ( FloatingPointNumber* )right;
	value = leftFloatPointNumber->value + rightFloatPointNumber->value;
	return true;
}

//=========================================================================================
/*virtual*/ bool FloatingPointNumber::AssignDifference( const Number* left, const Number* right, Environment& environment )
{
	FloatingPointNumber* leftFloatPointNumber = ( FloatingPointNumber* )left;
	FloatingPointNumber* rightFloatPointNumber = ( FloatingPointNumber* )right;
	value = leftFloatPointNumber->value - rightFloatPointNumber->value;
	return true;
}

//=========================================================================================
/*virtual*/ bool FloatingPointNumber::AssignProduct( const Number* left, const Number* right, Environment& environment )
{
	FloatingPointNumber* leftFloatPointNumber = ( FloatingPointNumber* )left;
	FloatingPointNumber* rightFloatPointNumber = ( FloatingPointNumber* )right;
	value = leftFloatPointNumber->value * rightFloatPointNumber->value;
	return true;
}

//=========================================================================================
/*virtual*/ bool FloatingPointNumber::AssignQuotient( const Number* left, const Number* right, Environment& environment )
{
	FloatingPointNumber* leftFloatPointNumber = ( FloatingPointNumber* )left;
	FloatingPointNumber* rightFloatPointNumber = ( FloatingPointNumber* )right;
	value = leftFloatPointNumber->value / rightFloatPointNumber->value;
	return true;
}

//=========================================================================================
/*virtual*/ bool FloatingPointNumber::AssignAdditiveIdentity( Environment& environment )
{
	value = 0.0;
	return true;
}

//=========================================================================================
/*virtual*/ bool FloatingPointNumber::AssignMultiplicativeIdentity( Environment& environment )
{
	value = 1.0;
	return true;
}

//=========================================================================================
/*virtual*/ bool FloatingPointNumber::IsAdditiveIdentity( bool& isAdditiveIdentity, Environment& environment ) const
{
	if( value == 0.0 )
		return true;
	return false;
}

//=========================================================================================
/*virtual*/ bool FloatingPointNumber::IsMultiplicativeIdentity( bool& isMultiplicativeIdentity, Environment& environment ) const
{
	if( value == 1.0 )
		return true;
	return false;
}

// FloatNumber.cpp