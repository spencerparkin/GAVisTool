// SumOfPseudoVersors.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

//=========================================================================================
class GeometricAlgebra::SumOfPseudoVersors
{
	friend class SumOfBlades;

public:

	SumOfPseudoVersors( void );
	virtual ~SumOfPseudoVersors( void );

	bool Print( char* printBuffer, int printBufferSize, ScalarAlgebra::PrintPurpose printPurpose ) const;

	bool AssignZero( void );
	bool AssignScalar( const Scalar& scalar );
	bool AssignVector( const Vector& vector );
	bool AssignBlade( const Blade& blade );
	bool AssignVersor( const PseudoVersor& pseudoVersor );
	bool AssignSumOfBlades( const SumOfBlades& sumOfBlades );
	bool AssignSumOfVersors( const SumOfPseudoVersors& sumOfPseudoVersors );
	bool AssignSum( const SumOfPseudoVersors& left, const SumOfPseudoVersors& right );
	bool AssignDifference( const SumOfPseudoVersors& left, const SumOfPseudoVersors& right );
	bool AssignOuterProduct( const SumOfPseudoVersors& left, const SumOfPseudoVersors& right );
	bool AssignInnerProduct( const SumOfPseudoVersors& left, const SumOfPseudoVersors& right );
	bool AssignGeometricProduct( const Vector& left, const SumOfPseudoVersors& right );
	bool AssignGeometricProduct( const SumOfPseudoVersors& left, const SumOfPseudoVersors& right );

	bool Scale( const Scalar& scalar );
	bool Reverse( void );

	bool Accumulate( const PseudoVersor& pseudoVersor );
	bool Accumulate( const SumOfPseudoVersors& sumOfPseudoVersors );

	PseudoVersor* FindLikeTerm( const PseudoVersor& pseudoVersor );

private:

	Utilities::List sum;
};

// SumOfPseudoVersors.h