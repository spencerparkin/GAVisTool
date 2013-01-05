// RationalNumber.cpp

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
IMPLEMENT_CALCLIB_CLASS1( RationalNumber, Number );

//=========================================================================================
RationalNumber::RationalNumber( int base )
{
	this->base = base;
}

//=========================================================================================
RationalNumber::RationalNumber( int number, int base )
{
	this->base = base;
	int degree = 0;
	while( number != 0 )
	{
		int coeficient = number % base;
		Term term( coeficient, degree );
		Accumulate( term );
		degree++;
		number /= base;
	}
}

//=========================================================================================
/*virtual*/ RationalNumber::~RationalNumber( void )
{
	listOfTerms.RemoveAll( true );
}

//=========================================================================================
/*virtual*/ Number* RationalNumber::CreateCopy( Environment& environment ) const
{
	RationalNumber* rationalNumber = new RationalNumber( base );
	if( !rationalNumber->AssignFrom( this, environment ) )
	{
		delete rationalNumber;
		rationalNumber = 0;
	}
	return rationalNumber;
}

//=========================================================================================
/*virtual*/ bool RationalNumber::Print( char* buffer, int bufferSize, bool printLatex, Environment& environment ) const
{
	// Make sure there's room to put something.
	if( bufferSize <= 0 )
		return false;
	buffer[0] = '\0';
	if( listOfTerms.Count() == 0 )
	{
		strcat_s( buffer, bufferSize, "0" );
		return true;
	}

	if( !environment.IsTypeOf( ArbitraryPrecisionEnvironment::ClassName() ) )
	{
		environment.AddError( "Rational numbers can only be used in the arbitrary precision environment." );
		return false;
	}

	ArbitraryPrecisionEnvironment* arbitraryPrecisionEnvironment = ( ArbitraryPrecisionEnvironment* )&environment;
	switch( arbitraryPrecisionEnvironment->PrintFormat() )
	{
		case ArbitraryPrecisionEnvironment::PRINT_COMPACT_NOTATION:
		{
			Term* leadingTerm = ( Term* )listOfTerms.LeftMost();
			Term* trailingTerm = ( Term* )listOfTerms.RightMost();

			if( leadingTerm->coeficient < 0.0 )
				strcat_s( buffer, bufferSize, "-" );
			
			int maxDegree = leadingTerm->degree;
			int minDegree = trailingTerm->degree;
			if( maxDegree < 0 )
				maxDegree = 0;
			if( minDegree > 0 )
				minDegree = 0;

			for( int degree = maxDegree; degree >= minDegree; degree-- )
			{
				Term* term = const_cast< RationalNumber* const >( this )->FindLikeTerm( degree );
				if( !term )
					strcat_s( buffer, bufferSize, "0" );
				else
				{
					char digitChar = MapCoeficientToDigitChar( term->coeficient );
					char digitString[2] = { digitChar, '\0' };
					strcat_s( buffer, bufferSize, digitString );
				}
				if( degree == 0 && 0 > trailingTerm->degree )
					strcat_s( buffer, bufferSize, "." );
			}

			if( base != 10 )
			{
				char baseString[ PRINT_BUFFER_SIZE_SMALL ];
				sprintf_s( baseString, sizeof( baseString ), "_%d", base );
				strcat_s( buffer, bufferSize, baseString );
			}

			return true;
		}
		case ArbitraryPrecisionEnvironment::PRINT_POLYNOMIAL_NOTATION:
		{
			for( Term* term = ( Term* )listOfTerms.LeftMost(); term; term = ( Term* )term->Right() )
			{
				char termString[ PRINT_BUFFER_SIZE_LARGE ];
				if( !term->Print( termString, sizeof( termString ), base ) )
					return false;
				strcat_s( buffer, bufferSize, termString );
				if( term->Right() )
					strcat_s( buffer, bufferSize, " + " );
			}
			return true;
		}
	}

	return false;
}

//=========================================================================================
/*virtual*/ bool RationalNumber::AssignFrom( const char* numberString, Environment& environment )
{
	bool success = false;
	char* numberStringCopy = 0;

	do
	{
		if( !AssignAdditiveIdentity( environment ) )
			break;

		if( numberString[0] == '\0' )
			break;

		// Is the number negative or positive?
		bool negate = false;
		if( numberString[0] == '-' )
		{
			negate = true;
			numberString++;
		}

		// Work with a copy of the number that we can poke at.
		int numberStringLength = strlen( numberString ) + 1;
		char* numberStringCopy = new char[ numberStringLength ];
		strcpy_s( numberStringCopy, numberStringLength, numberString );

		// Construct the number in base 10 unless the base is given otherwise.
		base = 10;
		char* baseString = strstr( numberStringCopy, "_" );
		if( baseString )
		{
			base = atoi( baseString + 1 );
			if( base <= 1 )
			{
				environment.AddError( "Cannot create a number of base %d.", base );
				break;
			}

			// Truncate the string here.
			baseString[0] = '\0';
		}

		// Find the digit just left of the point.
		char* termOfDegreeZero = 0;
		for( termOfDegreeZero = numberStringCopy; *termOfDegreeZero != '\0'; termOfDegreeZero++ )
			if( termOfDegreeZero[1] == '\0' || termOfDegreeZero[1] == '.' )
				break;
	
		// If we didn't find it, then this number is bogus.
		if( termOfDegreeZero == '\0' )
		{
			environment.AddError( "Cannot parse \"%s\" into a rational number.", numberString );
			break;
		}

		// Calculate the degree of the smallest digit.
		int degree = 1;
		char* digitChar = 0;
		for( digitChar = termOfDegreeZero; *digitChar != '\0'; digitChar++ )
			if( *digitChar != '.' )
				degree--;

		// Go accumulate all the digits of the number.
		digitChar--;
		while( digitChar >= numberStringCopy )
		{
			if( *digitChar != '.' )
			{
				int coeficient = MapDigitCharToCeoficient( *digitChar );
				if( coeficient < 0 || coeficient >= base )
				{
					environment.AddError( "The digit character %c is inappropriate for a number of base %d.", *digitChar, base );
					break;
				}

				if( negate )
					coeficient = -coeficient;
				Term term( coeficient, degree );
				Accumulate( term );
				degree++;
			}
			
			digitChar--;
		}
		if( ++digitChar != numberStringCopy )
			break;

		success = true;
	}
	while( false );

	delete[] numberStringCopy;

	return success;
}

//=========================================================================================
/*virtual*/ bool RationalNumber::AssignFrom( const Number* number, Environment& environment )
{
	if( !AssignAdditiveIdentity( environment ) )
		return false;

	if( !number->IsTypeOf( RationalNumber::ClassName() ) )
	{
		environment.AddError( "Only rational numbers can be assigned to rational numbers." );
		return false;
	}

	const RationalNumber* rationalNumber = ( const RationalNumber* )number;
	base = rationalNumber->base;
	Accumulate( *rationalNumber, false );
	return true;
}

//=========================================================================================
/*virtual*/ bool RationalNumber::AssignFrom( double number, Environment& environment )
{
	char numberString[ 512 ];
	sprintf_s( numberString, sizeof( numberString ), "%f", number );
	return AssignFrom( numberString, environment );
}

//=========================================================================================
// This will fail if the integer we represent cannot fit into a 64-bit float.
// If it can fit, however, we should succeed.
/*virtual*/ bool RationalNumber::AssignTo( double& number, Environment& environment ) const
{
	bool lostPrecision = false;
	number = EvaluatePolynomial( lostPrecision );
	if( lostPrecision )
	{
		environment.AddError( "Precision was lost during the process of converting a number of arbitrary precision to a 64-bit floating-point number." );
		return false;
	}
	return true;
}

//=========================================================================================
double RationalNumber::EvaluatePolynomial( bool& precisionLost ) const
{
	precisionLost = false;
	double sum = 0.0;
	for( const Term* term = ( const Term* )listOfTerms.LeftMost(); term; term = ( const Term* )term->Right() )
	{
		sum += double( term->coeficient ) * pow( double( base ), double( term->degree ) );
		if( sum != sum )
		{
			precisionLost = true;
			break;
		}
	}
	return sum;
}

//=========================================================================================
/*virtual*/ bool RationalNumber::AssignSum( const Number* left, const Number* right, Environment& environment )
{
	RationalNumber leftCopy( base ), rightCopy( base );
	if( this == left )
	{
		leftCopy.AssignFrom( this, environment );
		left = &leftCopy;
	}
	if( this == right )
	{
		rightCopy.AssignFrom( this, environment );
		right = &rightCopy;
	}

	if( !AssignAdditiveIdentity( environment ) )
		return false;

	if( !( left->IsTypeOf( RationalNumber::ClassName() ) && right->IsTypeOf( RationalNumber::ClassName() ) ) )
	{
		environment.AddError( "Rational number arithmetic operations require rational number operands." );
		return false;
	}

	const RationalNumber* leftRationalNumber = ( const RationalNumber* )left;
	const RationalNumber* rightRationalNumber = ( const RationalNumber* )right;

	if( leftRationalNumber->base != rightRationalNumber->base )
	{
		environment.AddError( "We do not allow addition between numbers of different bases.  Convert them to the same base first." );
		return false;
	}

	base = leftRationalNumber->base;
	Accumulate( *leftRationalNumber, false );
	Accumulate( *rightRationalNumber, false );
	return true;
}

//=========================================================================================
/*virtual*/ bool RationalNumber::AssignDifference( const Number* left, const Number* right, Environment& environment )
{
	RationalNumber leftCopy( base ), rightCopy( base );
	if( this == left )
	{
		leftCopy.AssignFrom( this, environment );
		left = &leftCopy;
	}
	if( this == right )
	{
		rightCopy.AssignFrom( this, environment );
		right = &rightCopy;
	}

	if( !AssignAdditiveIdentity( environment ) )
		return false;

	if( !( left->IsTypeOf( RationalNumber::ClassName() ) && right->IsTypeOf( RationalNumber::ClassName() ) ) )
	{
		environment.AddError( "Rational number arithmetic operations require rational number operands." );
		return false;
	}
	
	const RationalNumber* leftRationalNumber = ( const RationalNumber* )left;
	const RationalNumber* rightRationalNumber = ( const RationalNumber* )right;

	if( leftRationalNumber->base != rightRationalNumber->base )
	{
		environment.AddError( "We do not allow subtraction between numbers of different bases.  Convert them to the same base first." );
		return false;
	}

	base = leftRationalNumber->base;
	Accumulate( *leftRationalNumber, false );
	Accumulate( *rightRationalNumber, true );
	return true;
}

//=========================================================================================
/*virtual*/ bool RationalNumber::AssignProduct( const Number* left, const Number* right, Environment& environment )
{
	RationalNumber leftCopy( base ), rightCopy( base );
	if( this == left )
	{
		leftCopy.AssignFrom( this, environment );
		left = &leftCopy;
	}
	if( this == right )
	{
		rightCopy.AssignFrom( this, environment );
		right = &rightCopy;
	}

	if( !AssignAdditiveIdentity( environment ) )
		return false;

	if( !( left->IsTypeOf( RationalNumber::ClassName() ) && right->IsTypeOf( RationalNumber::ClassName() ) ) )
	{
		environment.AddError( "Rational number arithmetic operations require rational number operands." );
		return false;
	}

	const RationalNumber* leftRationalNumber = ( const RationalNumber* )left;
	const RationalNumber* rightRationalNumber = ( const RationalNumber* )right;

	if( leftRationalNumber->base != rightRationalNumber->base )
	{
		environment.AddError( "We do not allow multiplication between numbers of different bases.  Convert them to the same base first." );
		return false;
	}

	base = leftRationalNumber->base;

	for( Term* leftTerm = ( Term* )leftRationalNumber->listOfTerms.LeftMost(); leftTerm; leftTerm = ( Term* )leftTerm->Right() )
	{
		for( Term* rightTerm = ( Term* )rightRationalNumber->listOfTerms.LeftMost(); rightTerm; rightTerm = ( Term* )rightTerm->Right() )
		{
			int coeficient = leftTerm->coeficient * rightTerm->coeficient;
			int degree = leftTerm->degree + rightTerm->degree;
			Term productTerm( coeficient, degree );
			Accumulate( productTerm );
		}
	}

	return true;
}

//=========================================================================================
/*virtual*/ bool RationalNumber::AssignQuotient( const Number* left, const Number* right, Environment& environment )
{
	RationalNumber leftCopy( base ), rightCopy( base );
	if( this == left )
	{
		leftCopy.AssignFrom( this, environment );
		left = &leftCopy;
	}
	if( this == right )
	{
		rightCopy.AssignFrom( this, environment );
		right = &rightCopy;
	}

	bool isAdditiveIdentity = false;
	if( !right->IsAdditiveIdentity( isAdditiveIdentity, environment ) )
		return false;
	if( isAdditiveIdentity )
	{
		environment.AddError( "Cannot divide by zero." );
		return false;
	}

	if( !AssignAdditiveIdentity( environment ) )
		return false;

	if( !( left->IsTypeOf( RationalNumber::ClassName() ) && right->IsTypeOf( RationalNumber::ClassName() ) ) )
	{
		environment.AddError( "Rational number arithmetic operations require rational number operands." );
		return false;
	}

	const RationalNumber* leftRationalNumber = ( const RationalNumber* )left;
	const RationalNumber* rightRationalNumber = ( const RationalNumber* )right;

	if( leftRationalNumber->base != rightRationalNumber->base )
	{
		environment.AddError( "We do not allow division between numbers of different bases.  Convert them to the same base first." );
		return false;
	}

	base = leftRationalNumber->base;

	RationalNumber dividend( base );
	if( !dividend.AssignFrom( leftRationalNumber, environment ) )
		return false;

	const RationalNumber& divisor = *rightRationalNumber;
	RationalNumber quotient( base ), approximateDividend( base );

	if( !environment.IsTypeOf( ArbitraryPrecisionEnvironment::ClassName() ) )
	{
		environment.AddError( "Rational numbers can only be used in the arbitrary precision environment." );
		return false;
	}

	ArbitraryPrecisionEnvironment* arbitraryPrecisionEnvironment = ( ArbitraryPrecisionEnvironment* )&environment;
	int divisionLimit = arbitraryPrecisionEnvironment->DivisionLimit();

	// The key to this algorithm is the diminishing of the dividend.
	// Each iteration should decrease the degree of the dividend.
	// It can then be proven that the limit of this process for the
	// dividend is zero.
	for(;;)
	{
		// Our algorithm terminates when the dividend is zero.
		if( !dividend.IsAdditiveIdentity( isAdditiveIdentity, environment ) )
			return false;
		if( isAdditiveIdentity )
			break;

		// What is the degree of our polynomial?  Our algorithm
		// also terminates when we have reached the division limit.
		Term* dividendLeadingTerm = ( Term* )dividend.listOfTerms.LeftMost();
		int dividendDegree = dividendLeadingTerm->degree;
		if( dividendDegree < divisionLimit )
			break;		// Do any rounding?

		// Accumualte the division of the leading terms.
		Term* divisorLeadingTerm = ( Term* )divisor.listOfTerms.LeftMost();
		int divisorDegree = divisorLeadingTerm->degree;
		int coeficient = dividendLeadingTerm->coeficient / divisorLeadingTerm->coeficient;
		int degree = dividendDegree - divisorDegree;
		if( coeficient == 0 )
		{
			coeficient = ( dividendLeadingTerm->coeficient * base ) / divisorLeadingTerm->coeficient;
			degree--;
		}
		Term quotientTerm( coeficient, degree );
		Accumulate( quotientTerm );

		// Accumulating the approximate dividend should knock out the dividend's leading term.
		if( !quotient.AssignAdditiveIdentity( environment ) )
			return false;
		quotient.Accumulate( quotientTerm );
		if( !approximateDividend.AssignProduct( &divisor, &quotient, environment ) )
			return false;
		dividend.Accumulate( approximateDividend, true );
	}

	return true;
}

//=========================================================================================
/*virtual*/ bool RationalNumber::AssignAdditiveIdentity( Environment& environment )
{
	listOfTerms.RemoveAll( true );
	return true;
}

//=========================================================================================
/*virtual*/ bool RationalNumber::AssignMultiplicativeIdentity( Environment& environment )
{
	listOfTerms.RemoveAll( true );
	Term one( 1, 0 );
	Accumulate( one );
	return true;
}

//=========================================================================================
/*virtual*/ bool RationalNumber::IsAdditiveIdentity( bool& isAdditiveIdentity, Environment& environment ) const
{
	isAdditiveIdentity = false;
	if( listOfTerms.Count() == 0 )
		isAdditiveIdentity = true;
	return true;
}

//=========================================================================================
/*virtual*/ bool RationalNumber::IsMultiplicativeIdentity( bool& isMultiplicativeIdentity, Environment& environment ) const
{
	if( listOfTerms.Count() != 1 )
		return false;
	Term* term = ( Term* )listOfTerms.LeftMost();
	if( term->coeficient == 1 && term->degree == 0 )
		return true;
	return false;
}

//=========================================================================================
/*virtual*/ bool RationalNumber::CompareWith( const Number* number, Comparison& comparison, Environment& environment ) const
{
	comparison = COMPARISON_UNKNOWN;

	if( !number->IsTypeOf( RationalNumber::ClassName() ) )
	{
		environment.AddError( "Only rational numbers can be compared with rational numbers." );
		return false;
	}

	RationalNumber* rationalNumber = ( RationalNumber* )number;

	Term* thisTerm = ( Term* )listOfTerms.LeftMost();
	Term* givenTerm = ( Term* )rationalNumber->listOfTerms.LeftMost();
	while( comparison == COMPARISON_UNKNOWN )
	{
		if( !( thisTerm || givenTerm ) )
			comparison = IS_EQUAL_TO;
		else if( !thisTerm || ( givenTerm && thisTerm->degree < givenTerm->degree ) )
		{
			if( givenTerm->coeficient < 0 )
				comparison = IS_GREATER_THAN;
			else if( givenTerm->coeficient > 0 )
				comparison = IS_LESS_THAN;
			else
				break;		// This should never happen.
		}
		else if( !givenTerm || ( thisTerm && thisTerm->degree > givenTerm->degree ) )
		{
			if( thisTerm->coeficient < 0 )
				comparison = IS_LESS_THAN;
			else if( thisTerm->coeficient > 0 )
				comparison = IS_GREATER_THAN;
			else
				break;		// This should never happen.
		}
		else if( thisTerm->coeficient < givenTerm->coeficient )
			comparison = IS_LESS_THAN;
		else if( thisTerm->coeficient > givenTerm->coeficient )
			comparison = IS_GREATER_THAN;
		else
		{
			thisTerm = ( Term* )thisTerm->Right();
			givenTerm = ( Term* )givenTerm->Right();
		}
	}

	if( comparison == COMPARISON_UNKNOWN )
		return false;

	return true;
}

//=========================================================================================
// This is the sign of the number if it is in normalized form.
RationalNumber::Sign RationalNumber::SignOfLeadingCoeficient( void ) const
{
	if( listOfTerms.Count() == 0 )
		return SIGN_UNKNOWN;
	Term* leadingTerm = ( Term* )listOfTerms.LeftMost();
	if( leadingTerm->coeficient < 0 )
		return SIGN_NEGATIVE;
	if( leadingTerm->coeficient > 0 )
		return SIGN_POSITIVE;

	// We should never encounter a zero coeficient, but return this anyway.
	return SIGN_UNKNOWN;
}

//=========================================================================================
void RationalNumber::Accumulate( const RationalNumber& rationalNumber, bool negate )
{
	for( Term* term = ( Term* )rationalNumber.listOfTerms.RightMost(); term; term = ( Term* )term->Left() )
	{
		if( !negate )
			Accumulate( *term );
		else
		{
			Term negatedTerm( -term->coeficient, term->degree );
			Accumulate( negatedTerm );
		}
	}
}

//=========================================================================================
// Each of the four basic arithmetic operations, (addition, subtraction, multiplication,
// and division), are based upon this subroutine.  The algorithms for those operations
// can expect that the result we produce here is a number in normalized form.  This means
// that for every term c*b^n, the coeficient c is an integer in the range [0,b), or for
// every term c*b^n, the coeficient c is an integer in the range (-b,0], where b is the
// number base.  In the former case, the number is positive, negative in the latter case.
// The number zero has no terms.
void RationalNumber::Accumulate( const Term& term )
{
	// Correctness does not require us handling this case, but early out now if we can.
	if( term.coeficient == 0 )
		return;

	// Add the term to this number, potentially taking us from fully normalized to semi-normalized.
	Term accumulationTerm( term );
	AccumulationAlgorithm( accumulationTerm );

	// If there is no leading term, then the number is zero.
	Term* leadingTerm = ( Term* )listOfTerms.LeftMost();
	if( !leadingTerm )
		return;

	// In semi-normalized form, we can deduce the over-all
	// sign of the number from the sign of the leading coeficient.
	// This is not hard to prove.
	Sign sign = SIGN_POSITIVE;
	if( leadingTerm->coeficient < 0 )
		sign = SIGN_NEGATIVE;

	// Our task now is to take our number from semi-normalized
	// form to fully normalized form.  In fully normalized form,
	// all coeficient have the same sign.
	Sign oppositeSign = sign == SIGN_POSITIVE ? SIGN_NEGATIVE : SIGN_POSITIVE;
	Term* foundTerm = 0;
	while( FindTermWithSignHavingSmallestDegree( foundTerm, oppositeSign ) )
	{
		if( oppositeSign == SIGN_NEGATIVE )
		{
			// c*b^n = (c+b-b)*b^n = (c+b)*b^n - b^{n+1}
			foundTerm->coeficient += base;
			Term overflowTerm( -1, foundTerm->degree + 1 );
			AccumulationAlgorithm( overflowTerm );
		}
		else if( oppositeSign == SIGN_POSITIVE )
		{
			// c*b^n = (c-b+b)^b^n = (c-b)*b^n + b^{n+1}
			foundTerm->coeficient -= base;
			Term overflowTerm( 1, foundTerm->degree + 1 );
			AccumulationAlgorithm( overflowTerm );
		}
	}
}

//=========================================================================================
// This algorithm accumulates the given term into this number.  Our result is a
// number where each monomial is of the form c*b^n, where c is an integer in the
// range (-b,b), where b is the number base.  This is refered to as semi-normalized form.
void RationalNumber::AccumulationAlgorithm( const Term& term )
{
	// Add the accumulating term until there is nothing to add.
	Term accumulationTerm( term );
	while( accumulationTerm.coeficient != 0 )
	{
		// If there is no like term, create one.
		Term* likeTerm = FindLikeTerm( accumulationTerm.degree );
		if( !likeTerm )
		{
			likeTerm = new Term( 0, accumulationTerm.degree );
			listOfTerms.InsertSorted( likeTerm, Utilities::List::SORT_ORDER_DESCENDING );
		}

		// Combine the accumulating term with its like term.
		// Recalculate the accumulating term as the over-flow term.
		likeTerm->coeficient += accumulationTerm.coeficient;
		if( -base < likeTerm->coeficient && likeTerm->coeficient < base )
			accumulationTerm.coeficient = 0;
		else
		{
			// c*b^n = (c - m*b + m*b)*b^n = (c - m*b)*b^n + m*b^{n+1}
			accumulationTerm.coeficient = likeTerm->coeficient / base;
			likeTerm->coeficient -= accumulationTerm.coeficient * base;
			accumulationTerm.degree++;
		}

		// Never allow zero terms to exist in the polynomial.
		if( likeTerm->coeficient == 0 )
			listOfTerms.Remove( likeTerm, true );
	}
}

//=========================================================================================
RationalNumber::Term* RationalNumber::FindLikeTerm( int degree )
{
	Term* likeTerm = 0;
	for( Term* term = ( Term* )listOfTerms.LeftMost(); term && !likeTerm; term = ( Term* )term->Right() )
		if( term->degree == degree )
			likeTerm = term;
	return likeTerm;
}

//=========================================================================================
bool RationalNumber::FindTermWithSignHavingSmallestDegree( Term*& foundTerm, Sign sign )
{
	// Note that there should never exist a zero term in the polynomial.
	foundTerm = 0;
	for( Term* term = ( Term* )listOfTerms.RightMost(); term && !foundTerm; term = ( Term* )term->Left() )
		if( ( sign == SIGN_POSITIVE && term->coeficient > 0.0 ) || ( sign == SIGN_NEGATIVE && term->coeficient < 0.0 ) )
			foundTerm = term;
	if( foundTerm )
		return true;
	return false;
}

//=========================================================================================
/*static*/ int RationalNumber::MapDigitCharToCeoficient( char digitChar )
{
	int coeficient = -1;
	if( '0' <= digitChar && digitChar <= '9' )
		coeficient = int( digitChar - '0' );
	if( 'A' <= digitChar && digitChar <= 'Z' )
		coeficient = int( digitChar - 'A' ) + 10;
	return coeficient;
}

//=========================================================================================
/*static*/ char RationalNumber::MapCoeficientToDigitChar( int coeficient )
{
	int index = coeficient;
	if( index < 0 )
		index = -index;
	char digitMap[] =
	{
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C',
		'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
		'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
	};
	if( index < sizeof( digitMap ) )
		return digitMap[ index ];
	return '?';
}

//=========================================================================================
bool RationalNumber::RoundToNearestDegree( int degree )
{
	Term* term = FindLikeTerm( degree - 1 );
	if( term )
	{
		int halfBase = base / 2;
		if( term->coeficient >= halfBase )
		{
			Term roundingTerm( 1, degree );
			Accumulate( roundingTerm );
		}
		else if( term->coeficient <= -halfBase )
		{
			Term roundingTerm( -1, degree );
			Accumulate( roundingTerm );
		}
	}

	RemoveAllTermsOfDegreeLessThan( degree );
	return true;
}

//=========================================================================================
void RationalNumber::RemoveAllTermsOfDegreeLessThan( int degree )
{
	for(;;)
	{
		Term* trailingTerm = ( Term* )listOfTerms.RightMost();
		if( !trailingTerm )
			break;

		if( trailingTerm->degree < degree )
			listOfTerms.Remove( trailingTerm, true );
		else
			break;
	}
}

//=========================================================================================
void RationalNumber::RemoveAllTermsOfDegreeGreaterThan( int degree )
{
	for(;;)
	{
		Term* leadingTerm = ( Term* )listOfTerms.LeftMost();
		if( !leadingTerm )
			break;

		if( leadingTerm->degree > degree )
			listOfTerms.Remove( leadingTerm, true );
		else
			break;
	}
}

//=========================================================================================
bool RationalNumber::ConvertToBase( int base, RationalNumber& result, Environment& environment ) const
{
	// Make sure the given base makes sense.
	if( base <= 1 )
		return false;

	// If our job is easy, do it the easy way.
	if( base == this->base )
	{
		result.AssignFrom( this, environment );
		return true;
	}

	// We will accumulate terms in the given base.
	if( !result.AssignAdditiveIdentity( environment ) )
		return false;
	result.base = base;

	// We deal with the whole and fractional parts differently.
	RationalNumber wholePart( this->base );
	RationalNumber fractionalPart( this->base );
	for( Term* term = ( Term* )listOfTerms.LeftMost(); term; term = ( Term* )term->Right() )
	{
		if( term->degree >= 0 )
			wholePart.Accumulate( *term );
		else
			fractionalPart.Accumulate( *term );
	}

	// Now go crank out the digits of the whole part.
	int degree = 0;
	RationalNumber baseNumber( base, this->base );
	while( wholePart.listOfTerms.Count() > 0 )
	{
		int coeficient;
		if( !wholePart.CalculateResidueOfWholePart( coeficient, base ) )
			return false;
		Term digitTerm( coeficient, degree++ );
		result.Accumulate( digitTerm );
		if( !wholePart.AssignQuotient( &wholePart, &baseNumber, environment ) )
			return false;
		wholePart.RemoveAllTermsOfDegreeLessThan(0);
	}

	// Now go crank out the digit of the fractional part.
	degree = -1;
	while( fractionalPart.listOfTerms.Count() > 0 )
	{
		int countBeforeShift = fractionalPart.listOfTerms.Count();
		if( !fractionalPart.AssignProduct( &fractionalPart, &baseNumber, environment ) )
			return false;
		int countAfterShift = fractionalPart.listOfTerms.Count();
		Term* trailingTerm = ( Term* )listOfTerms.RightMost();
		for( int roundingDegree = trailingTerm->degree + 1; roundingDegree <= 0 && countAfterShift > countBeforeShift; roundingDegree++ )
		{
			fractionalPart.RoundToNearestDegree( roundingDegree );
			countAfterShift = fractionalPart.listOfTerms.Count();
		}
		int coeficient;
		if( !fractionalPart.CalculateResidueOfWholePart( coeficient, base ) )
			return false;
		Term digitTerm( coeficient, degree-- );
		result.Accumulate( digitTerm );
		fractionalPart.RemoveAllTermsOfDegreeGreaterThan(-1);
	}

	return true;
}

//=========================================================================================
// What we do here is simply evaluate the whole number in the given modulus.
bool RationalNumber::CalculateResidueOfWholePart( int& residue, int modulus )
{
	residue = 0;

	// Make sure our modulus makes sense.
	if( modulus <= 1 )
		return false;

	// If we're zero, our residue in any modulus is zero.
	Term* trailingTerm = ( Term* )listOfTerms.RightMost();
	if( !trailingTerm )
		return true;

	// Go calculate the residue of the whole part of this number.
	for( Term* term = ( Term* )listOfTerms.LeftMost(); term; term = ( Term* )term->Right() )
	{
		if( term->degree >= 0 )
		{
			residue += term->coeficient;
			CalculateResidue( residue, modulus );
			for( int power = 0; power < term->degree; power++ )
			{
				residue *= base;
				CalculateResidue( residue, modulus );
				if( residue == 0 )
					break;
			}
		}
	}

	return true;
}

//=========================================================================================
void RationalNumber::CalculateResidue( int& residue, int modulus )
{
	while( residue >= modulus )
		residue -= modulus;
	while( residue <= -modulus )
		residue += modulus;
}

//=========================================================================================
RationalNumber::Term::Term( int coeficient, int degree )
{
	this->coeficient = coeficient;
	this->degree = degree;
}

//=========================================================================================
RationalNumber::Term::Term( const Term& term )
{
	coeficient = term.coeficient;
	degree = term.degree;
}

//=========================================================================================
/*virtual*/ RationalNumber::Term::~Term( void )
{
}

//=========================================================================================
/*virtual*/ Utilities::List::SortComparison RationalNumber::Term::SortCompare( const Item* compareWithItem ) const
{
	const Term* term = ( const Term* )compareWithItem;
	if( degree < term->degree )
		return Utilities::List::SORT_COMPARE_LESS_THAN;
	if( degree > term->degree )
		return Utilities::List::SORT_COMPARE_GREATER_THAN;
	
	// We should never encounter two monomials in our polynomial having the same degree.
	return Utilities::List::SORT_COMPARE_UNKNOWN;
}

//=========================================================================================
/*virtual*/ Utilities::List::Item* RationalNumber::Term::MakeCopy( void ) const
{
	Term* term = new Term( coeficient, degree );
	return term;
}

//=========================================================================================
/*virtual*/ void RationalNumber::Term::DeleteThis( void )
{
	delete this;
}

//=========================================================================================
bool RationalNumber::Term::Print( char* buffer, int bufferSize, int base ) const
{
	if( bufferSize <= 0 )
		return false;
	sprintf_s( buffer, bufferSize, "%d*%d^%d", coeficient, base, degree );
	return true;
}

// RationalNumber.cpp