// ComplexNumber.cpp

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
IMPLEMENT_CALCLIB_CLASS1( ComplexNumber, Number );

//=========================================================================================
ComplexNumber::ComplexNumber( double realPart, double imaginaryPart )
{
	this->realPart = realPart;
	this->imaginaryPart = imaginaryPart;
}

//=========================================================================================
/*virtual*/ ComplexNumber::~ComplexNumber( void )
{
}

//=========================================================================================
/*virtual*/ Number* ComplexNumber::CreateCopy( Environment& environment ) const
{
	return new ComplexNumber( realPart, imaginaryPart );
}

//=========================================================================================
/*virtual*/ bool ComplexNumber::Print( char* buffer, int bufferSize, bool printLatex, Environment& environment ) const
{
	sprintf_s( buffer, bufferSize, "%f + %f*i", realPart, imaginaryPart );
	return true;
}

//=========================================================================================
/*virtual*/ bool ComplexNumber::AssignFrom( const char* numberString, Environment& environment )
{
	realPart = atof( numberString );
	imaginaryPart = 0.0;
	return true;
}

//=========================================================================================
/*virtual*/ bool ComplexNumber::AssignFrom( const Number* number, Environment& environment )
{
	if( !number->IsTypeOf( ComplexNumber::ClassName() ) )
		return false;

	ComplexNumber* complexNumber = ( ComplexNumber* )number;
	realPart = complexNumber->realPart;
	imaginaryPart = complexNumber->imaginaryPart;
	return true;
}

//=========================================================================================
/*virtual*/ bool ComplexNumber::AssignFrom( double number, Environment& environment )
{
	realPart = number;
	imaginaryPart = 0.0;
	return true;
}

//=========================================================================================
/*virtual*/ bool ComplexNumber::AssignTo( double& number, Environment& environment ) const
{
	if( imaginaryPart != 0.0 )
	{
		environment.AddError( "A complex number with non-zero imaginary part cannot be assigned to a real number." );
		return false;
	}

	number = realPart;
	return true;
}

//=========================================================================================
bool ComplexNumber::VerifyArguments( const ComplexNumber*& leftNumber, const ComplexNumber*& rightNumber, const Number* left, const Number* right, Environment& environment )
{
	if( !( left->IsTypeOf( ComplexNumber::ClassName() ) && right->IsTypeOf( ComplexNumber::ClassName() ) ) )
	{
		environment.AddError( "We deal only with complex numbers in the complex number field environment." );
		return false;
	}

	leftNumber = ( const ComplexNumber* )left;
	rightNumber = ( const ComplexNumber* )right;
	return true;
}

//=========================================================================================
/*virtual*/ bool ComplexNumber::AssignSum( const Number* left, const Number* right, Environment& environment )
{
	const ComplexNumber* leftNumber = 0;
	const ComplexNumber* rightNumber = 0;
	if( !VerifyArguments( leftNumber, rightNumber, left, right, environment ) )
		return false;

	realPart = leftNumber->realPart + rightNumber->realPart;
	imaginaryPart = leftNumber->imaginaryPart + rightNumber->imaginaryPart;
	return true;
}

//=========================================================================================
/*virtual*/ bool ComplexNumber::AssignDifference( const Number* left, const Number* right, Environment& environment )
{
	const ComplexNumber* leftNumber = 0;
	const ComplexNumber* rightNumber = 0;
	if( !VerifyArguments( leftNumber, rightNumber, left, right, environment ) )
		return false;

	realPart = leftNumber->realPart - rightNumber->realPart;
	imaginaryPart = leftNumber->imaginaryPart - rightNumber->imaginaryPart;
	return true;
}

//=========================================================================================
/*virtual*/ bool ComplexNumber::AssignProduct( const Number* left, const Number* right, Environment& environment )
{
	const ComplexNumber* leftNumber = 0;
	const ComplexNumber* rightNumber = 0;
	if( !VerifyArguments( leftNumber, rightNumber, left, right, environment ) )
		return false;

	// Do this in case either argument is this number.
	double leftRealPart = leftNumber->realPart;
	double rightRealPart = rightNumber->realPart;
	double leftImaginaryPart = leftNumber->imaginaryPart;
	double rightImaginaryPart = rightNumber->imaginaryPart;

	// (a+bi)(c+di) = ac-bd+(ad+bc)i
	realPart = leftRealPart * rightRealPart - leftImaginaryPart * rightImaginaryPart;
	imaginaryPart = leftRealPart * rightImaginaryPart + leftImaginaryPart * rightRealPart;
	return true;
}

//=========================================================================================
/*virtual*/ bool ComplexNumber::AssignQuotient( const Number* left, const Number* right, Environment& environment )
{
	const ComplexNumber* leftNumber = 0;
	const ComplexNumber* rightNumber = 0;
	if( !VerifyArguments( leftNumber, rightNumber, left, right, environment ) )
		return false;

	// Do this in case either argument is this number.
	double leftRealPart = leftNumber->realPart;
	double rightRealPart = rightNumber->realPart;
	double leftImaginaryPart = leftNumber->imaginaryPart;
	double rightImaginaryPart = rightNumber->imaginaryPart;

	// (a+bi)/(c+di) = (a+bi)(c+di)^{-1} = (a+bi)(c-di)/(c^2+d^2) = (ac+bd+(bc-ad)i)/(c^2+d^2)
	double rightSquareMagnitude = rightRealPart * rightRealPart + rightImaginaryPart * rightImaginaryPart;
	realPart = ( leftRealPart * leftRealPart + rightImaginaryPart * rightImaginaryPart ) / rightSquareMagnitude;
	imaginaryPart = ( leftImaginaryPart * rightRealPart - leftRealPart * rightImaginaryPart ) / rightSquareMagnitude;
	return true;
}

//=========================================================================================
/*virtual*/ bool ComplexNumber::AssignAdditiveIdentity( Environment& environment )
{
	realPart = 0.0;
	imaginaryPart = 0.0;
	return true;
}

//=========================================================================================
/*virtual*/ bool ComplexNumber::AssignMultiplicativeIdentity( Environment& environment )
{
	realPart = 1.0;
	imaginaryPart = 0.0;
	return true;
}

//=========================================================================================
/*virtual*/ bool ComplexNumber::IsAdditiveIdentity( bool& isAdditiveIdentity, Environment& environment ) const
{
	isAdditiveIdentity = false;
	if( realPart == 0.0 && imaginaryPart == 0.0 )
		isAdditiveIdentity = true;
	return true;
}

//=========================================================================================
/*virtual*/ bool ComplexNumber::IsMultiplicativeIdentity( bool& isMultiplicativeIdentity, Environment& environment ) const
{
	isMultiplicativeIdentity = false;
	if( realPart == 1.0 && imaginaryPart == 0.0 )
		isMultiplicativeIdentity = true;
	return true;
}

//=========================================================================================
/*virtual*/ bool ComplexNumber::CompareWith( const Number* number, Comparison& comparison, Environment& environment ) const
{
	// This is an interesting question.  We can't really impose an order on the complex numbers.
	comparison = Number::COMPARISON_UNKNOWN;
	environment.AddError( "How do we define an order on the field of complex numbers?" );
	return false;
}

// ComplexNumber.cpp