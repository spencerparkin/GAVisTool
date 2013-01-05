// Number.h

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
// Here we abstract away the concept of a number.  The calculator library
// is designed to work with a variaty of types of numbers.  We do not
// impose any constraints on the mathematical structure except to provide
// what operations may be closed on the structure.  We do not assume any
// special properties such as commutativity and distributivity.  All numbers
// that can be input are assumed to be the scalars of the underlying mathematical
// structure.  The scalars probably form a ring, but the over-all mathematical
// structure could be something like the complex numbers of a geometric algebra.
// Such algebras require the scalar algebra to form a ring.  This all works,
// because the higher-level mathematical structure is built upon the lower-level
// mathematical structure (the scalars) and a set of basis elements.
// The behavior of the basis elements is determined by the algebra in question.
class CalcLib::Number : public CalcLibClass
{
	DECLARE_CALCLIB_CLASS( Number );

	friend FunctionArgumentEvaluator;

public:

	enum Comparison
	{
		COMPARISON_UNKNOWN,
		IS_GREATER_THAN,
		IS_LESS_THAN,
		IS_EQUAL_TO,
	};

	Number( void );
	virtual ~Number( void );

	virtual Number* CreateCopy( Environment& environment ) const = 0;

	// Derivatives of this class must implement the following interface.
	// Number pointers can be safely cast to pointers of the implementing derived type.
	// When printing a number, it is sometimes desirable to output a format that can
	// also be re-read by the parser.
	virtual bool Print( char* buffer, int bufferSize, bool printLatex, Environment& environment ) const = 0;
	virtual bool AssignFrom( const char* numberString, Environment& environment ) = 0;
	virtual bool AssignFrom( const Number* number, Environment& environment ) = 0;
	virtual bool AssignFrom( double number, Environment& environment ) = 0;
	virtual bool AssignTo( Number& number, Environment& environment ) const;
	virtual bool AssignTo( double& number, Environment& environment ) const = 0;
	virtual bool AssignSum( const Number* left, const Number* right, Environment& environment ) = 0;
	virtual bool AssignDifference( const Number* left, const Number* right, Environment& environment ) = 0;
	virtual bool AssignProduct( const Number* left, const Number* right, Environment& environment ) = 0;
	virtual bool AssignQuotient( const Number* left, const Number* right, Environment& environment ) = 0;
	virtual bool AssignAdditiveIdentity( Environment& environment ) = 0;
	virtual bool AssignMultiplicativeIdentity( Environment& environment ) = 0;
	virtual bool IsAdditiveIdentity( bool& isAdditiveIdentity, Environment& environment ) const = 0;
	virtual bool IsMultiplicativeIdentity( bool& isMultiplicativeIdentity, Environment& environment ) const = 0;
	virtual bool CompareWith( const Number* number, Comparison& comparison, Environment& environment ) const;
	//virtual bool AssignMagnitude( const Number& number ) = 0;
	virtual bool AssignDerivative( const Number* number, const char* variableName, Environment& environment );
	virtual bool AssignAntiDerivative( const Number* number, const char* variableName, Environment& environment );

	// Derivatives of this class may choose to implement this interface, but
	// a default implementation is also provided if not need be overridden.
	virtual bool AssignAdditiveInverse( const Number* number, Environment& environment );
	virtual bool AssignMultilicativeInverse( const Number* number, Environment& environment );

	// These may be optionally overridden to support a more complicated
	// mathematical structure, such as a geometric algebra.
	virtual bool AssignInnerProduct( const Number* left, const Number* right, Environment& environment );
	virtual bool AssignOuterProduct( const Number* left, const Number* right, Environment& environment );

	// Print to the environment.
	bool Print(	Environment& environment ) const;
};

// Number.h