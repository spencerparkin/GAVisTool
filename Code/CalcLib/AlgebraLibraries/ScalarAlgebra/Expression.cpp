// Expression.cpp

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
Expression::Expression( void )
{
}

//=========================================================================================
/*virtual*/ Expression::~Expression( void )
{
	sumOfTerms.RemoveAll( true );
}

//=========================================================================================
Expression::Variable::Variable( const char* name, int exponent )
{
	int nameSize = strlen( name ) + 1;
	this->name = new char[ nameSize ];
	strcpy_s( this->name, nameSize, name );
	this->exponent = exponent;
}

//=========================================================================================
/*virtual*/ Expression::Variable::~Variable( void )
{
	delete[] name;
}

//=========================================================================================
/*virtual*/ Utilities::List::Item* Expression::Variable::MakeCopy( void ) const
{
	return new Variable( name, exponent );
}

//=========================================================================================
/*virtual*/ Utilities::List::SortComparison Expression::Variable::SortCompare( const Utilities::List::Item* item ) const
{
	const Variable* variable = ( const Variable* )item;
	int cmp = strcmp( name, variable->name );
	if( cmp < 0 )
		return Utilities::List::SORT_COMPARE_LESS_THAN;
	else if( cmp > 0 )
		return Utilities::List::SORT_COMPARE_GREATER_THAN;
	return Utilities::List::SORT_COMPARE_EQUAL_TO;
}

//=========================================================================================
Expression::Term::Term( void )
{
	coeficient = 0.0;
}

//=========================================================================================
/*virtual*/ Expression::Term::~Term( void )
{
	AssignZero();
}

//=========================================================================================
bool Expression::Term::AssignProduct( const Term& left, const Term& right )
{
	if( this == &left || this == &right )
		return false;

	productOfVariables.RemoveAll( true );

	coeficient = left.coeficient * right.coeficient;

	for( const Variable* leftVariable = ( const Variable* )left.productOfVariables.LeftMost(); leftVariable; leftVariable = ( const Variable* )leftVariable->Right() )
	{
		Variable* variable = 0;
		const Variable* rightVariable = right.FindLikeVariable( leftVariable );
		if( !rightVariable )
			variable = ( Variable* )leftVariable->MakeCopy();
		else
		{
			int exponent = leftVariable->exponent + rightVariable->exponent;
			if( exponent != 0 )
				variable = new Variable( leftVariable->name, exponent );
		}

		if( variable )
			productOfVariables.InsertSorted( variable->MakeCopy(), Utilities::List::SORT_ORDER_ASCENDING );
	}

	for( const Variable* rightVariable = ( const Variable* )right.productOfVariables.LeftMost(); rightVariable; rightVariable = ( const Variable* )rightVariable->Right() )
	{
		const Variable* leftVariable = left.FindLikeVariable( rightVariable );
		if( !leftVariable )
			productOfVariables.InsertSorted( rightVariable->MakeCopy(), Utilities::List::SORT_ORDER_ASCENDING );
	}

	return true;
}

//=========================================================================================
const Expression::Variable* Expression::Term::FindLikeVariable( const Variable* givenVariable ) const
{
	Variable* foundVariable = 0;
	for( Variable* variable = ( Variable* )productOfVariables.LeftMost(); variable && !foundVariable; variable = ( Variable* )variable->Right() )
		if( 0 == strcmp( variable->name, givenVariable->name ) )
			foundVariable = variable;
	return foundVariable;
}

//=========================================================================================
// We treat this term's product of variables here
// as a mutable member, because changing the order
// of the list does not actually change the value
// of the term.
bool Expression::Term::Sort( void ) const
{
	const_cast< Utilities::List* >( &productOfVariables )->Sort( Utilities::List::SORT_ORDER_ASCENDING );
	return true;
}

//=========================================================================================
// TODO: We should provide a sort compare here that will aid us
// in performing the division algorithm.  For now, this sort is
// based simply on visibility and readability.
/*virtual*/ Utilities::List::SortComparison Expression::Term::SortCompare( const Utilities::List::Item* item ) const
{
	const Term* term = ( const Term* )item;
	
	char leftTermString[ PRINT_BUFFER_SIZE_LARGE ];
	char rightTermString[ PRINT_BUFFER_SIZE_LARGE ];

	if( !Print( leftTermString, sizeof( leftTermString ), PRINT_FOR_SORTING ) )
		return Utilities::List::SORT_COMPARE_UNKNOWN;

	if( !term->Print( rightTermString, sizeof( rightTermString ), PRINT_FOR_SORTING ) )
		return Utilities::List::SORT_COMPARE_UNKNOWN;

	int cmp = strcmp( leftTermString, rightTermString );
	if( cmp < 0 )
		return Utilities::List::SORT_COMPARE_LESS_THAN;
	else if( cmp > 0 )
		return Utilities::List::SORT_COMPARE_GREATER_THAN;

	// We should never get here.  If we do, it means that we have
	// two like terms in the sum that were not combined, which would be bad.
	return Utilities::List::SORT_COMPARE_EQUAL_TO;
}

//=========================================================================================
/*virtual*/ Utilities::List::Item* Expression::Term::MakeCopy( void ) const
{
	Term* termCopy = new Term();
	termCopy->coeficient = coeficient;
	termCopy->productOfVariables.ConcatinateCopyOnLeft( productOfVariables );
	return termCopy;
}

//=========================================================================================
bool Expression::Print( char* printBuffer, int printBufferSize, PrintPurpose printPurpose ) const
{
	printBuffer[0] = '\0';

	if( sumOfTerms.Count() == 0 )
		strcat_s( printBuffer, printBufferSize, "0" );
	else
	{
		for( const Term* term = ( const Term* )sumOfTerms.LeftMost(); term; term = ( const Term* )term->Right() )
		{
			char termString[ PRINT_BUFFER_SIZE_LARGE ];
			if( !term->Print( termString, sizeof( termString ), printPurpose ) )
				return false;

			strcat_s( printBuffer, printBufferSize, termString );
			if( term->Right() )
				strcat_s( printBuffer, printBufferSize, " + " );
		}
	}

	return true;
}

//=========================================================================================
bool Expression::Term::Print( char* printBuffer, int printBufferSize, PrintPurpose printPurpose ) const
{
	printBuffer[0] = '\0';

	if( ( printPurpose == PRINT_FOR_READING || printPurpose == PRINT_FOR_LATEX ) && ( coeficient != 1.0 || productOfVariables.Count() == 0 ) )
	{
		bool printFormattedScalar = false;
		
		if( productOfVariables.Count() > 0 )
		{
			if( coeficient == 1.0 )
				;		// No need to print anything.
			else if( coeficient == -1.0 )
				strcat_s( printBuffer, printBufferSize, "-" );
			else
				printFormattedScalar = true;
		}
		else
			printFormattedScalar = true;

		if( printFormattedScalar )
		{
			if( printPurpose == PRINT_FOR_LATEX )
				sprintf_s( printBuffer, printBufferSize, "%f", coeficient );
			else
				sprintf_s( printBuffer, printBufferSize, "%1.2f", coeficient );
		}

		// Don't write out unnecessary zeros past the decimal point.
		if( printPurpose == PRINT_FOR_LATEX )
		{
			if( 0 != strstr( printBuffer, "." ) )
			{
				int index = strlen( printBuffer );
				while( index > 0 )
				{
					index--;
					if( printBuffer[ index ] == '0'  )
						printBuffer[ index ] = '\0';
					else if( printBuffer[ index ] == '.' )
					{
						printBuffer[ index ] = '\0';
						break;
					}
					else
						break;
				}
			}
		}

		if( productOfVariables.Count() > 0 && printPurpose != PRINT_FOR_LATEX )
			strcat_s( printBuffer, printBufferSize, "*" );
	}

	for( const Variable* variable = ( const Variable* )productOfVariables.LeftMost(); variable; variable = ( const Variable* )variable->Right() )
	{
		char variableString[ PRINT_BUFFER_SIZE_LARGE ];
		if( !variable->Print( variableString, sizeof( variableString ), printPurpose ) )
			return false;

		strcat_s( printBuffer, printBufferSize, variableString );
		if( printPurpose == PRINT_FOR_READING && variable->Right() && printPurpose != PRINT_FOR_LATEX )
			strcat_s( printBuffer, printBufferSize, "*" );
	}

	return true;
}

//=========================================================================================
bool Expression::Variable::Print( char* printBuffer, int printBufferSize, PrintPurpose printPurpose ) const
{
	if( printPurpose == PRINT_FOR_READING )
	{
		if( exponent > 1 )
			sprintf_s( printBuffer, printBufferSize, "%s^%d", name, exponent );
		else if( exponent < 0 )
			sprintf_s( printBuffer, printBufferSize, "%s^(%d)", name, exponent );
		else if( exponent == 0 )
			sprintf_s( printBuffer, printBufferSize, "1" );		// Hmmm.  We should probably just display the zero exponent.
		else // The exponent is 1 in this case.
			sprintf_s( printBuffer, printBufferSize, "%s", name );
	}
	else if( printPurpose == PRINT_FOR_SORTING )
		sprintf_s( printBuffer, printBufferSize, "%s", name );
	else if( printPurpose == PRINT_FOR_LATEX )
	{
		char latexName[ PRINT_BUFFER_SIZE_SMALL ];
		int nameLen = strlen( name );
		if( nameLen == 1 )
			strcpy_s( latexName, sizeof( latexName ), name );
		else
		{
			latexName[0] = name[0];
			latexName[1] = '\0';
			strcat_s( latexName, sizeof( latexName ), "_{" );
			strcat_s( latexName, sizeof( latexName ), name+1 );
			strcat_s( latexName, sizeof( latexName ), "}" );
		}

		if( exponent == 1 )
			strcpy_s( printBuffer, printBufferSize, latexName );
		else
			sprintf_s( printBuffer, printBufferSize, "%s^{%d}", latexName, exponent );
	}
	else
		return false;
	return true;
}

//=========================================================================================
bool Expression::Assign( double scalar )
{
	sumOfTerms.RemoveAll( true );
	if( scalar != 0.0 )
	{
		Term* term = new Term();
		term->coeficient = scalar;
		sumOfTerms.InsertLeftOf( 0, term );
	}
	return true;
}

//=========================================================================================
// This only works if there are no variables in the expression.
// If this expression is some literal number, then there will be
// either one or no terms.
bool Expression::AssignTo( double& scalar ) const
{
	if( sumOfTerms.Count() > 1 )
		return false;
	if( sumOfTerms.Count() == 0 )
		scalar = 0.0;
	else
	{
		Term* term = ( Term* )sumOfTerms.LeftMost();
		if( term->productOfVariables.Count() > 0 )
			return false;
		scalar = term->coeficient;
	}
	return true;
}

//=========================================================================================
bool Expression::Assign( const char* scalar )
{
	sumOfTerms.RemoveAll( true );
	Term* term = new Term();
	term->coeficient = 1.0;
	Variable* variable = new Variable( scalar, 1 );
	term->productOfVariables.InsertLeftOf( 0, variable );
	sumOfTerms.InsertLeftOf( 0, term );
	return true;
}

//=========================================================================================
bool Expression::Assign( const Term& term )
{
	sumOfTerms.RemoveAll( true );
	sumOfTerms.InsertLeftOf( 0, term.MakeCopy() );
	return true;
}

//=========================================================================================
bool Expression::AssignZero( void )
{
	sumOfTerms.RemoveAll( true );
	return true;
}

//=========================================================================================
bool Expression::AssignOne( void )
{
	sumOfTerms.RemoveAll( true );
	Term* term = new Term();
	term->coeficient = 1.0;
	sumOfTerms.InsertLeftOf( 0, term );
	return true;
}

//=========================================================================================
// We never keep zero terms, so we're zero if and only if
// our list of terms is empty.
bool Expression::IsZero( void ) const
{
	if( sumOfTerms.Count() > 0 )
		return false;

	return true;
}

//=========================================================================================
bool Expression::IsOne( void ) const
{
	return IsScalar( 1.0 );
}

//=========================================================================================
bool Expression::IsNegativeOne( void ) const
{
	return IsScalar( -1.0 );
}

//=========================================================================================
bool Expression::IsScalar( double scalar ) const
{
	if( scalar == 0.0 )
		return IsZero();

	if( sumOfTerms.Count() != 1 )
		return false;

	const Term* term = ( const Term* )sumOfTerms.LeftMost();
	if( term->coeficient != scalar )
		return false;

	if( term->productOfVariables.Count() > 0 )
		return false;

	return true;
}

//=========================================================================================
// I guess zero is not a monomial.
bool Expression::IsMonomial( void ) const
{
	if( sumOfTerms.Count() != 1 )
		return false;

	return true;
}

//=========================================================================================
bool Expression::Assign( const Expression& expression )
{
	if( this != &expression )
	{
		sumOfTerms.RemoveAll( true );
		sumOfTerms.ConcatinateCopyOnRight( expression.sumOfTerms );
	}
	return true;
}

//=========================================================================================
bool Expression::AssignDerivative( const Expression& expression, const char* variableName )
{
	if( !AssignZero() )
		return false;

	Term derivative;
	for( const Term* term = ( const Term* )expression.sumOfTerms.LeftMost(); term; term = ( const Term* )term->Right() )
	{
		if( !derivative.AssignDerivative( *term, variableName ) )
			return false;

		if( !Accumulate( derivative ) )
			return false;
	}

	return true;
}

//=========================================================================================
bool Expression::Term::AssignZero( void )
{
	coeficient = 0.0;
	productOfVariables.RemoveAll( true );
	return true;
}

//=========================================================================================
bool Expression::Term::Assign( const Term& term )
{
	coeficient = term.coeficient;
	productOfVariables.RemoveAll( true );
	productOfVariables.ConcatinateCopyOnRight( term.productOfVariables );
	return true;
}

//=========================================================================================
bool Expression::Term::AssignDerivative( const Term& term, const char* variableName )
{
	if( !Assign( term ) )
		return false;

	Variable variable( variableName, 1 );
	Variable* foundVariable = const_cast< Variable* >( FindLikeVariable( &variable ) );
	if( !foundVariable )
	{
		if( !AssignZero() )
			return false;
	}
	else
	{
		double scalar = foundVariable->exponent;
		coeficient *= scalar;
		foundVariable->exponent--;
		if( foundVariable->exponent == 0 )
			productOfVariables.Remove( foundVariable, true );
	}

	return true;
}

//=========================================================================================
bool Expression::CopyArgumentsIfNeeded( const Expression& left, const Expression& right, const Expression*& leftExpression, const Expression*& rightExpression )
{
	leftExpression = &left;
	rightExpression = &right;

	// Hmmm, this makes us non-thread-safe, by the way.
	static Expression leftCopy, rightCopy;

	if( this == &left )
	{
		if( !leftCopy.Assign( left ) )
			return false;
		leftExpression = &leftCopy;
	}

	if( this == &right )
	{
		if( !rightCopy.Assign( right ) )
			return false;
		rightExpression = &rightCopy;
	}

	return true;
}

//=========================================================================================
// Addition is commutative, so it doesn't matter which
// of the given expression we accumulate first.
bool Expression::AssignSum( const Expression& left, const Expression& right )
{
	const Expression* leftExpression = 0;
	const Expression* rightExpression = 0;
	if( !CopyArgumentsIfNeeded( left, right, leftExpression, rightExpression ) )
		return false;

	if( !Assign( *leftExpression ) )
		return false;

	if( !Accumulate( *rightExpression ) )
		return false;

	return true;
}

//=========================================================================================
bool Expression::AssignDifference( const Expression& left, const Expression& right )
{
	const Expression* leftExpression = 0;
	const Expression* rightExpression = 0;
	if( !CopyArgumentsIfNeeded( left, right, leftExpression, rightExpression ) )
		return false;

	if( !Assign( *rightExpression ) )
		return false;

	if( !Negate() )
		return false;

	if( !Accumulate( *leftExpression ) )
		return false;

	return true;
}

//=========================================================================================
bool Expression::AssignProduct( const Term& left, const Expression& right )
{
	Expression leftExpression;
	if( !leftExpression.Assign( left ) )
		return false;

	if( !AssignProduct( leftExpression, right ) )
		return false;

	return true;
}

//=========================================================================================
bool Expression::AssignProduct( const Expression& left, const Expression& right )
{
	const Expression* leftExpression = 0;
	const Expression* rightExpression = 0;
	if( !CopyArgumentsIfNeeded( left, right, leftExpression, rightExpression ) )
		return false;

	if( !AssignZero() )
		return false;

	for( const Term* leftTerm = ( const Term* )leftExpression->sumOfTerms.LeftMost(); leftTerm; leftTerm = ( const Term* )leftTerm->Right() )
	{
		for( const Term* rightTerm = ( const Term* )rightExpression->sumOfTerms.LeftMost(); rightTerm; rightTerm = ( const Term* )rightTerm->Right() )
		{
			Term product;
			if( !product.AssignProduct( *leftTerm, *rightTerm ) )
				return false;

			if( !Accumulate( product ) )
				return false;
		}
	}

	return true;
}

//=========================================================================================
// An empty list is considered one.
bool Expression::AssignProduct( const Utilities::List& productOfExpressions )
{
	if( !AssignOne() )
		return false;

	for( const Expression* expression = ( const Expression* )productOfExpressions.LeftMost(); expression; expression = ( const Expression* )expression->Right() )
		if( !AssignProduct( *this, *expression ) )
			return false;

	return true;
}

//=========================================================================================
// As we go through the division algorithm, the key here is
// to make sure that whatever term we choose to accumulate
// in the quotient, that it is a term that will knock out a
// term in the current dividend without creating any new terms.
bool Expression::AssignQuotient( const Expression& dividend, const Expression& divisor, Expression& remainder, int divisionLimit )
{
	const Expression* dividendExpression = 0;
	const Expression* divisorExpression = 0;
	if( !CopyArgumentsIfNeeded( dividend, divisor, dividendExpression, divisorExpression ) )
		return false;

	if( !AssignZero() )
		return false;

	if( dividendExpression->IsZero() )
		return true;

	if( divisorExpression->IsZero() )
		return false;

	Expression iterativeDividend;
	if( !iterativeDividend.Assign( *dividendExpression ) )
		return false;

	if( divisionLimit < 0 )
		divisionLimit = dividendExpression->sumOfTerms.Count();

	Expression product;
	Term quotientTerm;
	for( int iteration = 0; iteration < divisionLimit; iteration++ )
	{
		const Term* dividendTerm, *divisorTerm;
		if( !ChooseBestTermsForDivision( iterativeDividend, *divisorExpression, dividendTerm, divisorTerm ) )
			return false;

		if( !quotientTerm.AssignQuotient( *dividendTerm, *divisorTerm ) )
			return false;

		if( !Accumulate( quotientTerm ) )
			return false;

		if( !product.AssignProduct( quotientTerm, *divisorExpression ) )
			return false;

		if( !iterativeDividend.AssignDifference( iterativeDividend, product ) )
			return false;

		if( iterativeDividend.IsZero() )
			break;
	}

	return true;
}

//=========================================================================================
/*static*/ bool Expression::ChooseBestTermsForDivision( const Expression& dividend, const Expression& divisor, const Term*& dividendTerm, const Term*& divisorTerm )
{
	if( dividend.IsZero() || divisor.IsZero() )
		return false;

	if( divisor.IsMonomial() )
	{
		// In this case it doesn't matter which dividend term we choose,
		// and there is clearly only one divisor term to choose from.
		divisorTerm = ( const Term* )divisor.sumOfTerms.LeftMost();
		dividendTerm = ( const Term* )dividend.sumOfTerms.LeftMost();
	}
	else
	{
		// This is the harder case.  In this case, we may want to
		// detect if we're working with a polynomial in one variable
		// for each of the dividend and divisor, in which case we
		// always want to choose the terms of highest degree.  If that
		// is not the case, then we need to work off of some theory
		// about dividing such expressions.  In any case, what we want
		// to do is choose a divisor term and a dividend term such that
		// the next dividend term generated in the division algorithm
		// has less terms in it than before.
		return false;
	}

	return true;
}

//=========================================================================================
bool Expression::Term::AssignQuotient( const Term& dividend, const Term& divisor )
{
	if( this == &dividend || this == &divisor )
		return false;

	productOfVariables.RemoveAll( true );

	coeficient = dividend.coeficient / divisor.coeficient;

	for( const Variable* divisorVariable = ( const Variable* )divisor.productOfVariables.LeftMost(); divisorVariable; divisorVariable = ( const Variable* )divisorVariable->Right() )
	{
		int dividendExponent = 0;
		const Variable* dividendVariable = dividend.FindLikeVariable( divisorVariable );
		if( dividendVariable )
			dividendExponent = dividendVariable->exponent;

		int quotientExponent = dividendExponent - divisorVariable->exponent;
		if( quotientExponent != 0 )
		{
			Variable* quotientVariable = new Variable( divisorVariable->name, quotientExponent );
			productOfVariables.InsertSorted( quotientVariable, Utilities::List::SORT_ORDER_ASCENDING );
		}
	}

	for( const Variable* dividendVariable = ( const Variable* )dividend.productOfVariables.LeftMost(); dividendVariable; dividendVariable = ( const Variable* )dividendVariable->Right() )
	{
		int divisorExponent = 0;
		const Variable* divisorVariable = divisor.FindLikeVariable( dividendVariable );
		if( divisorVariable )
			continue;		// We have already accounted for this.

		int quotientExponent = dividendVariable->exponent - divisorExponent;
		if( quotientExponent != 0 )		// This should always be the case.
		{
			Variable* quotientVariable = new Variable( dividendVariable->name, quotientExponent );
			productOfVariables.InsertSorted( quotientVariable, Utilities::List::SORT_ORDER_ASCENDING );
		}
	}

	return true;
}

//=========================================================================================
bool Expression::Accumulate( const Expression& expression )
{
	for( const Term* term = ( const Term* )expression.sumOfTerms.LeftMost(); term; term = ( const Term* )term->Right() )
		if( !Accumulate( *term ) )
			return false;
	return true;
}

//=========================================================================================
// Notice that we never keep a zero term in the list.
// This way, we can easily check that an expression is
// zero by seeing if it has an empty list.
bool Expression::Accumulate( const Term& term )
{
	if( term.coeficient == 0.0 )
		return true;

	Term* likeTerm = FindLikeTerm( term );
	if( likeTerm )
	{
		likeTerm->coeficient += term.coeficient;
		if( likeTerm->coeficient == 0.0 )
			sumOfTerms.Remove( likeTerm, true );
	}
	else
	{
		// By always keeping the terms sorted, this makes it easier to compare to
		// different expressions to see if they are the same.
		sumOfTerms.InsertSorted( term.MakeCopy(), Utilities::List::SORT_ORDER_ASCENDING );
	}

	return true;
}

//=========================================================================================
bool Expression::Negate( void )
{
	for( Term* term = ( Term* )sumOfTerms.LeftMost(); term; term = ( Term* )term->Right() )
		if( !term->Negate() )
			return false;
	return true;
}

//=========================================================================================
bool Expression::Term::Negate( void )
{
	coeficient *= -1.0;
	return true;
}

//=========================================================================================
Expression::Term* Expression::FindLikeTerm( const Term& givenTerm )
{
	Term* foundTerm = 0;
	for( Term* term = ( Term* )sumOfTerms.LeftMost(); term && !foundTerm; term = ( Term* )term->Right() )
		if( term->IsScalarMultipleOf( givenTerm ) )
			foundTerm = term;
	return foundTerm;
}

//=========================================================================================
// Since we keep all terms sorted all the time,
// we are going to assume that the given term is
// sorted and that this term is sorted.  This makes
// the process of finding like-terms easy.
bool Expression::Term::IsScalarMultipleOf( const Term& givenTerm ) const
{
	if( productOfVariables.Count() != givenTerm.productOfVariables.Count() )
		return false;

	const Variable* thisVariable = ( const Variable* )productOfVariables.LeftMost();
	const Variable* givenVariable = ( const Variable* )givenTerm.productOfVariables.LeftMost();

	while( thisVariable && givenVariable )
	{
		if( Utilities::List::SORT_COMPARE_EQUAL_TO != thisVariable->SortCompare( givenVariable ) )
			return false;

		if( thisVariable->exponent != givenVariable->exponent )
			return false;

		thisVariable = ( const Variable* )thisVariable->Right();
		givenVariable = ( const Variable* )givenVariable->Right();
	}

	return true;
}

//=========================================================================================
bool Expression::Factor( Utilities::List& productOfExpressions ) const
{
	productOfExpressions.RemoveAll( true );

	Utilities::List queue;
	queue.InsertLeftOf( 0, MakeCopy() );

	while( queue.Count() > 0 )
	{
		Expression* expression = ( Expression* )queue.LeftMost();
		queue.Remove( expression, false );

		Expression leftFactor, rightFactor;
		if( !expression->FindAnyNonTrivialFactorization( leftFactor, rightFactor ) )
			productOfExpressions.InsertRightOf( productOfExpressions.RightMost(), expression );
		else
		{
			queue.InsertLeftOf( queue.LeftMost(), rightFactor.MakeCopy() );
			queue.InsertLeftOf( queue.LeftMost(), leftFactor.MakeCopy() );
			delete expression;
		}
	}

	return true;
}

//=========================================================================================
// This is a very hard problem to solve and so
// while I may not solve it completely here, if in
// some cases I can find a non-trivial factorization,
// I can make the software a little smarter and able
// to simplify some rational expressions, even if not all.
bool Expression::FindAnyNonTrivialFactorization( Expression& left, Expression& right ) const
{
	// The first and most obvious non-trivial factorization
	// we should look for is that of a non-trivial term dividing all
	// of the terms in the sum.  By non-trivial I mean non-one.

	return false;
}

//=========================================================================================
bool Expression::CompareWith( const Expression& expression ) const
{
	// We never keep zero terms or like terms in the list, so this is an easy early out.
	if( sumOfTerms.Count() != expression.sumOfTerms.Count() )
		return false;

	// If the expressions are the same, then their terms
	// are also in the same order, because we always keep
	// terms sorted the same way in the list.
	const Term* thisTerm = ( const Term* )sumOfTerms.LeftMost();
	const Term* givenTerm = ( const Term* )expression.sumOfTerms.LeftMost();
	while( thisTerm && givenTerm )
	{
		if( thisTerm->coeficient != givenTerm->coeficient )
			return false;

		if( !thisTerm->IsScalarMultipleOf( *givenTerm ) )
			return false;

		thisTerm = ( const Term* )thisTerm->Right();
		givenTerm = ( const Term* )givenTerm->Right();
	}

	return true;
}

//=========================================================================================
bool Expression::Evaluate( Expression& result, const VariableEvaluator& variableEvaluator ) const
{
	if( !result.AssignZero() )
		return false;

	for( Term* term = ( Term* )sumOfTerms.LeftMost(); term; term = ( Term* )term->Right() )
	{
		Term evaluatedTerm;
		if( !term->Evaluate( evaluatedTerm, variableEvaluator ) )
			return false;

		if( !result.Accumulate( evaluatedTerm ) )
			return false;
	}

	return true;
}

//=========================================================================================
bool Expression::Term::Evaluate( Term& result, const VariableEvaluator& variableEvaluator ) const
{
	result.coeficient = 1.0;
	result.productOfVariables.RemoveAll( true );
	for( const Variable* variable = ( const Variable* )productOfVariables.LeftMost(); variable; variable = ( const Variable* )variable->Right() )
	{
		double variableValue;
		if( !variableEvaluator.Evaluate( variable->name, variableValue ) )
			result.productOfVariables.InsertSorted( variable->MakeCopy(), Utilities::List::SORT_ORDER_ASCENDING );
		else
		{
			if( variableValue != 0.0 )
				result.coeficient *= pow( variableValue, variable->exponent );
			else
			{
				result.coeficient = 0.0;
				result.productOfVariables.RemoveAll( true );
			}
		}	
	}
	return true;
}

// Expression.cpp