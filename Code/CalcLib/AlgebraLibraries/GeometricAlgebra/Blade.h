// Blade.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

//=========================================================================================
// More accurately, this class represents a basis blade, not a blade in general.
// A blade in general is any element of a geometric algebra that can be factored
// as the outer product of zero or more vectors.  A blade in general would be
// represented as a linear combination of basis blades, which should not be confused
// with an N-vector, if it does not form an N-blade.  All N-blades are N-vectors, but not
// all N-vectors are N-blades.  An N-vector is any element of a geometric algebra
// that is homogenous of grade N.  An N-vector is not an N-blade when it cannot
// be written as a single general blade.
class GeometricAlgebra::Blade : public Utilities::List::Item
{
	friend class SumOfBlades;
	friend class SumOfPseudoVersors;

public:

	Blade( void );
	virtual ~Blade( void );

	bool Print( char* printBuffer, int printBufferSize, ScalarAlgebra::PrintPurpose printPurpose ) const;

	bool AssignZero( void );
	bool AssignScalar( const Scalar& scalar );
	bool AssignScalar( const char* scalar );
	bool AssignScalarPartTo( Scalar& scalar ) const;
	bool AssignVector( const Vector& vector, const Scalar& scalar );
	bool AssignBlade( const Blade& blade, bool sortProduct = true );
	bool AssignBlade( Utilities::List& outerProduct, const Scalar& scalar, bool sortProduct = true );
	bool AssignOuterProduct( const Blade& left, const Blade& right, bool sortProduct = true );
	bool AssignSquareMagnitudeTo( Scalar& squareMagnitude ) const;
	bool AssignDerivative( const Blade& blade, const char* variableName );
	bool AssignAntiDerivative( const Blade& blade, const char* variableName );

	bool IsLikeTerm( const Blade& blade, Scalar& relativeHandedness ) const;
	bool Scale( const Scalar& scalar );
	bool RemoveVector( const char* vectorName );
	bool Bar( void );

	int Grade( void ) const;

	// Accounting for the change in handedness, sorting a blade
	// does not change the blade being represented by this class.
	// Never-the-less, we do not declare this as a const function, since
	// the sort order of the product of the blade is sometimes a
	// desired and noted characteristic of the blade, even if, again,
	// it does not have any bearing on the actual blade being represented.
	bool Sort( int* adjacentSwapCount = 0 );

	virtual Utilities::List::SortComparison SortCompare( const Item* item ) const;
	virtual Utilities::List::Item* MakeCopy( void ) const;

private:

	Scalar scalar;
	Utilities::List product;
	bool sorted;
};

// Blade.h