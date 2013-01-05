// RationalExpression.cpp

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "ScalarAlgebra.h"

using namespace ScalarAlgebra;

//=========================================================================================
RationalExpression::RationalExpression( void )
{
}

//=========================================================================================
/*virtual*/ RationalExpression::~RationalExpression( void )
{
}

//=========================================================================================
bool RationalExpression::Print( char* printBuffer, int printBufferSize, PrintPurpose printPurpose, bool* parenthesisNeeded /*= 0*/ ) const
{
	printBuffer[0] = '\0';

	char numeratorString[ PRINT_BUFFER_SIZE_LARGE ];
	if( !numerator.Print( numeratorString, sizeof( numeratorString ), printPurpose ) )
		return false;

	if( parenthesisNeeded )
		*parenthesisNeeded = false;

	if( denominator.IsOne() )
	{
		strcat_s( printBuffer, printBufferSize, numeratorString );
		if( parenthesisNeeded && numerator.sumOfTerms.Count() > 1 )
			*parenthesisNeeded = true;
	}
	else
	{
		char denominatorString[ PRINT_BUFFER_SIZE_LARGE ];
		if( !denominator.Print( denominatorString, sizeof( denominatorString ), printPurpose ) )
			return false;

		if( printPurpose == PRINT_FOR_LATEX )
			sprintf_s( printBuffer, printBufferSize, "\\frac{%s}{%s}", numeratorString, denominatorString );
		else if( printPurpose == PRINT_FOR_READING )
			sprintf_s( printBuffer, printBufferSize, "(%s)/(%s)", numeratorString, denominatorString );
	}

	return true;
}

//=========================================================================================
bool RationalExpression::AssignZero( void )
{
	if( !numerator.AssignZero() )
		return false;

	if( !denominator.AssignOne() )
		return false;

	return true;
}

//=========================================================================================
bool RationalExpression::Assign( double scalar )
{
	if( !numerator.Assign( scalar ) )
		return false;

	if( !denominator.AssignOne() )
		return false;

	return true;
}

//=========================================================================================
bool RationalExpression::AssignTo( double& scalar ) const
{
	// This only succeeds if there are no variables in the expression.
	double numerator, denominator;
	if( !this->numerator.AssignTo( numerator ) )
		return false;
	if( !this->denominator.AssignTo( denominator ) )
		return false;
	if( denominator == 0.0 )
		return false;
	scalar = numerator / denominator;
	return true;
}

//=========================================================================================
bool RationalExpression::Assign( const char* scalar )
{
	if( !numerator.Assign( scalar ) )
		return false;

	if( !denominator.AssignOne() )
		return false;

	return true;
}

//=========================================================================================
bool RationalExpression::Assign( const RationalExpression& rationalExpression )
{
	if( !numerator.Assign( rationalExpression.numerator ) )
		return false;

	if( !denominator.Assign( rationalExpression.denominator ) )
		return false;

	return true;
}

//=========================================================================================
// a/b + c/d = (ad+bc)/(bd)
bool RationalExpression::AssignSum( const RationalExpression& left, const RationalExpression& right )
{
	if( left.IsZero() )
	{
		if( !Assign( right ) )
			return false;

		return true;
	}

	if( right.IsZero() )
	{
		if( !Assign( left ) )
			return false;

		return true;
	}

	Expression leftProduct, rightProduct;

	if( !leftProduct.AssignProduct( left.numerator, right.denominator ) )
		return false;

	if( !rightProduct.AssignProduct( left.denominator, right.numerator ) )
		return false;

	if( !numerator.AssignSum( leftProduct, rightProduct ) )
		return false;

	if( !denominator.AssignProduct( left.denominator, right.denominator ) )
		return false;

	if( !Simplify() )
		return false;

	return true;
}

//=========================================================================================
// a/b - c/d = (ad-bc)/(bd)
bool RationalExpression::AssignDifference( const RationalExpression& left, const RationalExpression& right )
{
	if( left.IsZero() )
	{
		if( !Assign( right ) )
			return false;

		if( !Negate() )
			return false;

		return true;
	}

	if( right.IsZero() )
	{
		if( !Assign( left ) )
			return false;

		return true;
	}

	Expression leftProduct, rightProduct;

	if( !leftProduct.AssignProduct( left.numerator, right.denominator ) )
		return false;

	if( !rightProduct.AssignProduct( left.denominator, right.numerator ) )
		return false;

	if( !rightProduct.Negate() )
		return false;

	if( !numerator.AssignSum( leftProduct, rightProduct ) )
		return false;

	if( !denominator.AssignProduct( left.denominator, right.denominator ) )
		return false;

	if( !Simplify() )
		return false;

	return true;
}

//=========================================================================================
// (a/b)(c/d) = (ac)/(bd)
bool RationalExpression::AssignProduct( const RationalExpression& left, const RationalExpression& right )
{
	if( !numerator.AssignProduct( left.numerator, right.numerator ) )
		return false;

	if( !denominator.AssignProduct( left.denominator, right.denominator ) )
		return false;

	if( !Simplify() )
		return false;

	return true;
}

//=========================================================================================
// (a/b)/(c/d) = (a/b)(d/c) = (ad)/(bc)
bool RationalExpression::AssignQuotient( const RationalExpression& left, const RationalExpression& right )
{
	if( !numerator.AssignProduct( left.numerator, right.denominator ) )
		return false;

	if( !denominator.AssignProduct( left.denominator, right.numerator ) )
		return false;

	if( !Simplify() )
		return false;

	return true;
}

//=========================================================================================
bool RationalExpression::Negate( void )
{
	if( !numerator.Negate() )
		return false;

	return true;
}

//=========================================================================================
bool RationalExpression::Recipricate( void )
{
	if( numerator.IsZero() )
		return false;

	if( ( numerator.IsOne() || numerator.IsNegativeOne() ) && denominator.IsOne() )
		return true;

	Expression expression;
	if( !expression.Assign( numerator ) )
		return false;

	if( !numerator.Assign( denominator ) )
		return false;

	if( !denominator.Assign( expression ) )
		return false;

	return true;
}

//=========================================================================================
// Since simplification does not change the value
// being represented by the rational expression,
// we could make this a const function, but then
// we would have to do const-casts to treat the
// members as mutable.
bool RationalExpression::Simplify( void )
{
	// If the denominator is a monomial, we can always reduce the denominator to one.
	if( denominator.IsMonomial() )
	{
		Expression remainder;
		if( !numerator.AssignQuotient( numerator, denominator, remainder ) )
			return false;

		// This is a sanity check.
		if( !remainder.IsZero() )
			return false;

		if( !denominator.AssignOne() )
			return false;

		return true;
	}

	// Simply thing we can do here is see if there exists
	// a non-trivial monomial that divides the top and bottom.
	// Do that here...

	// Factor out all common monmials...
	// Factor out all common binomials...
	// etc...?

	// Early out now, because I am not ready to test the code that follows.
	return true;

	Utilities::List factoredNumerator, factoredDenominator;

	if( !numerator.Factor( factoredNumerator ) )
		return false;

	if( !denominator.Factor( factoredDenominator ) )
		return false;

	Expression* divisor = ( Expression* )factoredDenominator.LeftMost();
	while( divisor )
	{
		Expression* nextDivisor = ( Expression* )divisor->Right();

		Expression* dividend = ( Expression* )factoredNumerator.LeftMost();
		while( dividend )
		{
			Expression* nextDividend = ( Expression* )dividend->Right();

			Expression quotient, remainder;
			if( !quotient.AssignQuotient( *dividend, *divisor, remainder ) )
				return false;

			if( remainder.IsZero() )
			{
				factoredNumerator.Remove( dividend, true );
				factoredDenominator.Remove( divisor, true );

				if( !quotient.IsOne() )
					factoredNumerator.InsertLeftOf( factoredNumerator.LeftMost(), quotient.MakeCopy() );

				break;
			}

			dividend = nextDividend;
		}

		divisor = nextDivisor;
	}

	if( !numerator.AssignProduct( factoredNumerator ) )
		return false;

	if( !denominator.AssignProduct( factoredDenominator ) )
		return false;

	return true;
}

//=========================================================================================
bool RationalExpression::CompareWith( const RationalExpression& rationalExpression ) const
{
	return( numerator.CompareWith( rationalExpression.numerator ) && denominator.CompareWith( rationalExpression.denominator ) );
}

//=========================================================================================
bool RationalExpression::Evaluate( RationalExpression& result, const VariableEvaluator& variableEvaluator )
{
	if( !numerator.Evaluate( result.numerator, variableEvaluator ) )
		return false;

	if( !denominator.Evaluate( result.denominator, variableEvaluator ) )
		return false;

	if( result.denominator.IsZero() )
		return false;

	return true;
}

//=========================================================================================
const Expression& RationalExpression::Numerator( void )
{
	return numerator;
}

//=========================================================================================
const Expression& RationalExpression::Denominator( void )
{
	return denominator;
}

//=========================================================================================
bool RationalExpression::IsZero( void ) const
{
	if( numerator.IsZero() )
		return true;

	return false;
}

//=========================================================================================
bool RationalExpression::Differentiate( const char* variableName )
{
	if( denominator.IsOne() )
	{
		Expression diffNumer;
		if( !diffNumer.AssignDerivative( numerator, variableName ) )
			return false;
		if( !numerator.Assign( diffNumer ) )
			return false;
	}
	else
	{
		// Do the quotient rule.

		Expression diffNumer, diffDenom;
		if( !diffNumer.AssignDerivative( numerator, variableName ) )
			return false;
		if( !diffDenom.AssignDerivative( denominator, variableName ) )
			return false;

		Expression firstProd, secondProd;
		if( !firstProd.AssignProduct( diffNumer, denominator ) )
			return false;
		if( !secondProd.AssignProduct( numerator, diffDenom ) )
			return false;
		if( !numerator.AssignDifference( firstProd, secondProd ) )
			return false;

		Expression denomSquared;
		if( !denomSquared.AssignProduct( denominator, denominator ) )
			return false;
		if( !denominator.Assign( denomSquared ) )
			return false;
	}

	return true;
}

//=========================================================================================
bool RationalExpression::AntiDifferentiate( const char* variableName )
{
	// Not implemented yet...maybe never!
	return false;
}

// RationalExpression.cpp