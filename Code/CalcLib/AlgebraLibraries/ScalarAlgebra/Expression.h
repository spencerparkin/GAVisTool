// Expression.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

//=========================================================================================
// Expressions are a generalization of polynomials.
// That is, polynomials are a special case of expressions.
// Naturally, this makes expressions even harder to work with.
// When we can identify an expression as a polynomial, we can
// treat it as such, being able to draw upon all the theorems
// of algebra about polynomials.
class ScalarAlgebra::Expression : public Utilities::List::Item
{
	friend class RationalExpression;

public:

	Expression( void );
	virtual ~Expression( void );

	class Variable : public Utilities::List::Item
	{
	public:

		Variable( const char* name, int exponent );
		virtual ~Variable( void );

		bool Print( char* printBuffer, int printBufferSize, PrintPurpose printPurpose ) const;

		virtual Utilities::List::Item* MakeCopy( void ) const override;
		virtual Utilities::List::SortComparison SortCompare( const Utilities::List::Item* item ) const override;

		char* name;
		int exponent;
	};

	class Term : public Utilities::List::Item
	{
	public:

		Term( void );
		virtual ~Term( void );

		bool Print( char* printBuffer, int printBufferSize, PrintPurpose printPurpose ) const;
		bool Assign( const Term& term );
		bool AssignZero( void );
		bool AssignDerivative( const Term& term, const char* variableName );
		bool AssignProduct( const Term& left, const Term& right );
		bool AssignQuotient( const Term& dividend, const Term& divisor );
		bool Sort( void ) const;
		bool IsScalarMultipleOf( const Term& term ) const;
		bool Negate( void );
		const Variable* FindLikeVariable( const Variable* givenVariable ) const;
		bool Evaluate( Term& result, const VariableEvaluator& variableEvaluator ) const;

		virtual Utilities::List::Item* MakeCopy( void ) const override;
		virtual Utilities::List::SortComparison SortCompare( const Utilities::List::Item* item ) const override;

		double coeficient;
		Utilities::List productOfVariables;
	};

	bool Print( char* printBuffer, int printBufferSize, PrintPurpose printPurpose ) const;
	bool AssignZero( void );
	bool AssignOne( void );
	bool Assign( double scalar );
	bool AssignTo( double& scalar ) const;
	bool Assign( const char* scalar );
	bool Assign( const Term& term );
	bool Assign( const Expression& expression );
	bool AssignDerivative( const Expression& expression, const char* variableName );
	bool AssignSum( const Expression& left, const Expression& right );
	bool AssignDifference( const Expression& left, const Expression& right );
	bool AssignProduct( const Expression& left, const Expression& right );
	bool AssignProduct( const Term& left, const Expression& right );
	bool AssignProduct( const Utilities::List& productOfExpressions );
	bool AssignQuotient( const Expression& dividend, const Expression& divisor, Expression& remainder, int divisionLimit = -1 );
	bool Accumulate( const Expression& expression );
	bool Accumulate( const Term& term );
	bool Negate( void );
	bool IsZero( void ) const;
	bool IsOne( void ) const;
	bool IsNegativeOne( void ) const;
	bool IsScalar( double scalar ) const;
	bool IsMonomial( void ) const;
	bool Factor( Utilities::List& productOfExpressions ) const;
	bool FindAnyNonTrivialFactorization( Expression& left, Expression& right ) const;
	bool CompareWith( const Expression& expression ) const;
	bool Evaluate( Expression& result, const VariableEvaluator& variableEvaluator ) const;

private:

	bool CopyArgumentsIfNeeded( const Expression& left, const Expression& right, const Expression*& leftExpression, const Expression*& rightExpression );
	Term* FindLikeTerm( const Term& givenTerm );
	static bool ChooseBestTermsForDivision( const Expression& dividend, const Expression& divisor, const Term*& dividendTerm, const Term*& divisorTerm );

	Utilities::List sumOfTerms;
};

// Expression.h