// Scalar.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

//=========================================================================================
// TODO: I should get rid of all operator overloads here in favor of functions that can return error results.
//       Right now, if an error occurs, it gets ignored.  Alternatively, we could have these throw exceptions
//       and then always wrap scalar math with catch statements.
template< typename ScalarType >
class ScalarAlgebra::TemplatedScalar
{
public:

	TemplatedScalar( void );
	TemplatedScalar( const TemplatedScalar& scalar );
	TemplatedScalar( double scalar );

	operator double( void ) const;

	bool operator==( const TemplatedScalar& scalar ) const;
	bool operator==( double scalar ) const;

	bool operator<( const TemplatedScalar& scalar ) const;
	bool operator<( double scalar ) const;

	bool operator>( const TemplatedScalar& scalar ) const;
	bool operator>( double scalar ) const;

	TemplatedScalar& operator=( const TemplatedScalar& scalar );
	TemplatedScalar& operator=( double scalar );
	TemplatedScalar& operator=( const char* scalar );

	TemplatedScalar operator+( const TemplatedScalar& scalar ) const;
	TemplatedScalar operator+( double scalar ) const;

	TemplatedScalar operator-( const TemplatedScalar& scalar ) const;
	TemplatedScalar operator-( double scalar ) const;

	TemplatedScalar operator*( const TemplatedScalar& scalar ) const;
	TemplatedScalar operator*( double scalar ) const;

	TemplatedScalar operator/( const TemplatedScalar& scalar ) const;
	TemplatedScalar operator/( double scalar ) const;

	bool Invert( void );
	bool Negate( void );

	bool Differentiate( const char* variableName );
	bool AntiDifferentiate( const char* variableName );

	bool Print( char* printBuffer, int printBufferSize, PrintPurpose printPurpose, bool* parenthesisNeeded = 0 ) const;

	ScalarType scalar;
};

// Scalar.h