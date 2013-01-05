// FractionNumber.cpp

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
IMPLEMENT_CALCLIB_CLASS1( FractionNumber, Number );

//=========================================================================================
FractionNumber::FractionNumber( void )
{
	numerator = 0;
	denominator = 1;
}

//=========================================================================================
/*virtual*/ FractionNumber::~FractionNumber( void )
{
}

//=========================================================================================
/*virtual*/ Number* FractionNumber::CreateCopy( Environment& environment ) const
{
	FractionNumber* copy = new FractionNumber;
	copy->numerator = numerator;
	copy->denominator = denominator;
	return copy;
}

//=========================================================================================
/*virtual*/ bool FractionNumber::Print( char* buffer, int bufferSize, bool printLatex, Environment& environment ) const
{
	if( denominator == 1 )
		sprintf_s( buffer, bufferSize, "%d", numerator );
	else
		sprintf_s( buffer, bufferSize, "%d/%d", numerator, denominator );
	return true;
}

//=========================================================================================
/*virtual*/ bool FractionNumber::AssignFrom( const char* numberString, Environment& environment )
{
	double number = atof( numberString );
	if( !AssignFrom( number, environment ) )
		return false;
	return true;
}

//=========================================================================================
/*virtual*/ bool FractionNumber::AssignFrom( const Number* number, Environment& environment )
{
	if( !number->IsTypeOf( FractionNumber::ClassName() ) )
		return false;

	FractionNumber* fraction = ( FractionNumber* )number;
	numerator = fraction->numerator;
	denominator = fraction->denominator;
	return true;
}

//=========================================================================================
/*virtual*/ bool FractionNumber::AssignFrom( double number, Environment& environment )
{
	double multiplier = 1.0;
	double fractionalPart = 0.0;
	double epsilon = 1e-20;
	double maxMultiplier = 100000000.0;
	while( multiplier < maxMultiplier )
	{
		double shiftedNumber = number * multiplier;
		fractionalPart = fmod( shiftedNumber, 1.0 );
		if( fabs( fractionalPart ) <= epsilon )
			break;
		multiplier *= 10.0;
	}

	numerator = long( number * multiplier );
	denominator = long( multiplier );

	ReduceFraction();

	return true;
}

//=========================================================================================
/*virtual*/ bool FractionNumber::AssignTo( double& number, Environment& environment ) const
{
	number = double( numerator ) / double( denominator );
	return true;
}

//=========================================================================================
/*virtual*/ bool FractionNumber::AssignSum( const Number* left, const Number* right, Environment& environment )
{
	FractionNumber* leftFraction = ( FractionNumber* )left;
	FractionNumber* rightFraction = ( FractionNumber* )right;

	long commonDenominator;
	long leftNumerator, rightNumerator;
	UnifyFractions( leftFraction, rightFraction, commonDenominator, leftNumerator, rightNumerator );

	numerator = leftNumerator + rightNumerator;
	denominator = commonDenominator;

	ReduceFraction();

	return true;
}

//=========================================================================================
/*virtual*/ bool FractionNumber::AssignDifference( const Number* left, const Number* right, Environment& environment )
{
	FractionNumber* leftFraction = ( FractionNumber* )left;
	FractionNumber* rightFraction = ( FractionNumber* )right;

	long commonDenominator;
	long leftNumerator, rightNumerator;
	UnifyFractions( leftFraction, rightFraction, commonDenominator, leftNumerator, rightNumerator );

	numerator = leftNumerator - rightNumerator;
	denominator = commonDenominator;

	ReduceFraction();

	return true;
}

//=========================================================================================
/*virtual*/ bool FractionNumber::AssignProduct( const Number* left, const Number* right, Environment& environment )
{
	FractionNumber* leftFraction = ( FractionNumber* )left;
	FractionNumber* rightFraction = ( FractionNumber* )right;

	numerator = leftFraction->numerator * rightFraction->numerator;
	denominator = leftFraction->denominator * rightFraction->denominator;

	ReduceFraction();

	return true;
}

//=========================================================================================
/*virtual*/ bool FractionNumber::AssignQuotient( const Number* left, const Number* right, Environment& environment )
{
	FractionNumber reciprical;
	if( !reciprical.AssignMultilicativeInverse( right, environment ) )
		return false;

	if( !AssignProduct( left, &reciprical, environment ) )
		return false;

	return true;
}

//=========================================================================================
/*virtual*/ bool FractionNumber::AssignAdditiveInverse( const Number* number, Environment& environment )
{
	FractionNumber* fraction = ( FractionNumber* )number;

	// We always keep the denominator positive.
	numerator = -fraction->numerator;
	denominator = fraction->denominator;

	return true;
}

//=========================================================================================
/*virtual*/ bool FractionNumber::AssignMultilicativeInverse( const Number* number, Environment& environment )
{
	FractionNumber* fraction = ( FractionNumber* )number;

	if( fraction->numerator == 0 )
	{
		environment.AddError( "Zero does not have a multiplicative inverse!" );
		return false;
	}

	// Assign the reciprical.
	numerator = fraction->denominator;
	denominator = fraction->numerator;

	// We always keep the denominator positive.
	if( denominator < 0 )
	{
		numerator *= -1;
		denominator *= -1;
	}

	return true;
}

//=========================================================================================
/*virtual*/ bool FractionNumber::AssignAdditiveIdentity( Environment& environment )
{
	numerator = 0;
	denominator = 1;
	return true;
}

//=========================================================================================
/*virtual*/ bool FractionNumber::AssignMultiplicativeIdentity( Environment& environment )
{
	numerator = 1;
	denominator = 1;
	return true;
}

//=========================================================================================
/*virtual*/ bool FractionNumber::AssignInnerProduct( const Number* left, const Number* right, Environment& environment )
{
	return false;
}

//=========================================================================================
/*virtual*/ bool FractionNumber::AssignOuterProduct( const Number* left, const Number* right, Environment& environment )
{
	return false;
}

//=========================================================================================
/*virtual*/ bool FractionNumber::IsAdditiveIdentity( bool& isAdditiveIdentity, Environment& environment ) const
{
	isAdditiveIdentity = false;
	if( numerator == 0 )
		isAdditiveIdentity = true;
	return true;
}

//=========================================================================================
/*virtual*/ bool FractionNumber::IsMultiplicativeIdentity( bool& isMultiplicativeIdentity, Environment& environment ) const
{
	isMultiplicativeIdentity = false;
	if( numerator == 1 && denominator == 1 )
		isMultiplicativeIdentity = true;
	return true;
}

//=========================================================================================
/*virtual*/ bool FractionNumber::CompareWith( const Number* number, Comparison& comparison, Environment& environment ) const
{
	FractionNumber* fraction = ( FractionNumber* )number;

	// We don't really need to do these early checks for the correctness of our algorithm here.
	if( numerator == 0 )
	{
		if( fraction->numerator < 0 )
		{
			comparison = IS_GREATER_THAN;
			return true;
		}
		else if( fraction->numerator > 0 )
		{
			comparison = IS_LESS_THAN;
			return true;
		}
		else if( fraction->numerator == 0 )
		{
			comparison = IS_EQUAL_TO;
			return true;
		}
	}
	else if( fraction->numerator == 0 )
	{
		if( numerator < 0 )
		{
			comparison = IS_LESS_THAN;
			return true;
		}
		else if( numerator > 0 )
		{
			comparison = IS_GREATER_THAN;
			return true;
		}
		else if( numerator == 0 )
		{
			comparison = IS_EQUAL_TO;
			return true;
		}
	}
	else
	{
		if( numerator < 0 && fraction->numerator > 0 )
		{
			comparison = IS_LESS_THAN;
			return true;
		}
		else if( numerator > 0 && fraction->numerator < 0 )
		{
			comparison = IS_GREATER_THAN;
			return true;
		}
	}

	// The following test will work in all situations.
	// That is, we didn't have to do the checks we did prior to this.
	long commonDenominator;
	long leftNumerator, rightNumerator;
	UnifyFractions( this, fraction, commonDenominator, leftNumerator, rightNumerator );

	if( leftNumerator < rightNumerator )
		comparison = IS_LESS_THAN;
	else if( leftNumerator > rightNumerator )
		comparison = IS_GREATER_THAN;
	else
		comparison = IS_EQUAL_TO;

	return true;
}

//=========================================================================================
// This is a commutative operation, despite the argument names.
/*static*/ void FractionNumber::UnifyFractions( const FractionNumber* left, const FractionNumber* right, long& leastCommonDenominator, long& leftNumerator, long& rightNumerator )
{
	leastCommonDenominator = LeastCommonMultiple( left->denominator, right->denominator );
	leftNumerator = left->numerator * ( leastCommonDenominator / left->denominator );
	rightNumerator = right->numerator * ( leastCommonDenominator / right->denominator );
}

//=========================================================================================
void FractionNumber::ReduceFraction( void )
{
	if( numerator == 0 )
		denominator = 1;
	else
	{
		long gcd = GreatestCommonDivisor( numerator, denominator );
		numerator /= gcd;
		denominator /= gcd;
	}
}

//=========================================================================================
// Find the smallest integer greater than or equal to the largest
// of the two given integers that is also divisible by both integers.
// We assume that both numbers are positive (i.e., non-negative and non-zero.)
/*static*/ long FractionNumber::LeastCommonMultiple( long a, long b )
{
	long gcd = GreatestCommonDivisor( a, b );

	// Notice that a/gcd and b/gcd are co-prime.
	// This means that the LCM of a/gcd and b/gcd is ab/gcd^2.
	// The LCM of a and b would therefore be (ab/gcd^2)*gcd = ab/gcd.
	return( a * b / gcd );
}

//=========================================================================================
// Find the largest integer less than or equal to the smallest
// of the two given integers that also divides both of the integers.
// We assume that both numbers are positive (i.e., non-negative and non-zero.)
/*static*/ long FractionNumber::GreatestCommonDivisor( long a, long b )
{
	long smaller, larger;
	if( a < b )
	{
		smaller = a;
		larger = b;
	}
	else
	{
		smaller = b;
		larger = a;
	}

	// Do the Euclidean algorithm.
	do
	{
		long remainder = larger % smaller;
		larger = smaller;
		smaller = remainder;
	}
	while( smaller != 0 );
	
	return larger;
}

// FractionNumber.cpp