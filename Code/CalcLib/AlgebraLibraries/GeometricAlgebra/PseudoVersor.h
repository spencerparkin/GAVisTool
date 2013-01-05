// PseudoVersor.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

//=========================================================================================
// This class represents any geometric product of basis vectors.
// These are versors when all vectors in the product are invertiable,
// which is to say that we represent a versor here if and only if
// the product does not contain a null vector.
class GeometricAlgebra::PseudoVersor : public Utilities::List::Item
{
	friend class SumOfBlades;
	friend class SumOfPseudoVersors;

public:

	PseudoVersor( void );
	virtual ~PseudoVersor( void );

	bool Print( char* printBuffer, int printBufferSize, ScalarAlgebra::PrintPurpose printPurpose ) const;

	bool AssignZero( void );
	bool AssignScalar( const Scalar& scalar );
	bool AssignVector( const Vector& vector, const Scalar& scalar );
	bool AssignVersor( const PseudoVersor& pseudoVersor );
	bool AssignGeometricProduct( const PseudoVersor& left, const PseudoVersor& right );

	bool IsLikeTerm( const PseudoVersor& pseudoVersor ) const;

	bool Scale( const Scalar& scalar );
	bool Reverse( void );

	int Grade( void ) const;

	bool IsVersor( void ) const;

	// Though this does not change the actual pseudoVersor
	// being represented by this class, we do not
	// declare this as a const-method.
	void Simplify( void );

	virtual Utilities::List::SortComparison SortCompare( const Item* item ) const;
	virtual Utilities::List::Item* MakeCopy( void ) const;

private:

	bool MakeSimplification( void );
	bool CanBeMadeAdjacent( const Vector* left, const Vector* right, Scalar& handednessForAdjacency ) const;

	Scalar scalar;
	Utilities::List product;
};

// PseudoVersor.h