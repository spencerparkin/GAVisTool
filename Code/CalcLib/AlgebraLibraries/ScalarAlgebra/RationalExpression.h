// RationalExpression.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

//=========================================================================================
class ScalarAlgebra::RationalExpression
{
public:

	RationalExpression( void );
	virtual ~RationalExpression( void );

	bool Print( char* printBuffer, int printBufferSize, PrintPurpose printPurpose, bool* parenthesisNeeded = 0 ) const;
	bool AssignZero( void );
	bool Assign( double scalar );
	bool AssignTo( double& scalar ) const;
	bool Assign( const char* scalar );
	bool Assign( const RationalExpression& rationalExpression );
	bool AssignSum( const RationalExpression& left, const RationalExpression& right );
	bool AssignDifference( const RationalExpression& left, const RationalExpression& right );
	bool AssignProduct( const RationalExpression& left, const RationalExpression& right );
	bool AssignQuotient( const RationalExpression& left, const RationalExpression& right );
	bool Negate( void );
	bool Recipricate( void );
	bool Simplify( void );
	bool CompareWith( const RationalExpression& rationalExpression ) const;
	bool Evaluate( RationalExpression& result, const VariableEvaluator& variableEvaluator );
	bool IsZero( void ) const;
	bool Differentiate( const char* variableName );
	bool AntiDifferentiate( const char* variableName );

	const Expression& Numerator( void );
	const Expression& Denominator( void );

private:

	Expression numerator, denominator;
};

// RationalExpression.h