// SumOfBlades.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

//=========================================================================================
class GeometricAlgebra::SumOfBlades
{
	friend class SumOfPseudoVersors;

public:

	SumOfBlades( void );
	virtual ~SumOfBlades( void );

	enum InverseType
	{
		LEFT_INVERSE,
		RIGHT_INVERSE,
	};

	enum InverseResult
	{
		SINGULAR_MULTIVECTOR,
		NONSINGULAR_MULTIVECTOR,
	};

	bool Print( char* printBuffer, int printBufferSize, ScalarAlgebra::PrintPurpose printPurpose ) const;

	bool AssignZero( void );
	bool AssignScalar( const Scalar& scalar );
	bool AssignScalar( const char* scalar );
	bool AssignScalarFrom( double scalar );
	bool AssignScalarTo( double& scalar ) const;
	bool AssignVector( const Vector& vector );
	bool AssignBlade( const Blade& blade );
	bool AssignVersor( const PseudoVersor& pseudoVersor );
	bool AssignSumOfBlades( const SumOfBlades& sumOfBlades );
	bool AssignSumOfVersors( const SumOfPseudoVersors& sumOfPseudoVersors );
	bool AssignSum( const SumOfBlades& left, const SumOfBlades& right );
	bool AssignDifference( const SumOfBlades& left, const SumOfBlades& right );
	bool AssignOuterProduct( const Vector& left, const SumOfBlades& right );
	bool AssignOuterProduct( const SumOfBlades& left, const SumOfBlades& right );
	bool AssignInnerProduct( const Vector& left, const Blade& right );
	bool AssignInnerProduct( const Blade& left, const Vector& right );
	bool AssignInnerProduct( const Vector& left, const SumOfBlades& right );
	bool AssignInnerProduct( const SumOfBlades& left, const Vector& right );
	bool AssignInnerProduct( const Blade& left, const Blade& right );
	bool AssignInnerProduct( const SumOfBlades& left, const SumOfBlades& right );
	bool AssignGeometricProduct( const SumOfBlades& left, const SumOfBlades& right );
	bool AssignGeometricProduct( const Blade& left, const Blade& right );
	bool AssignGeometricInverse( const SumOfBlades& sumOfBlades, InverseType inverseType, InverseResult& inverseResult );
	bool AssignSquareMagnitudeTo( Scalar& squareMagnitude ) const;
	bool AssignDerivative( const SumOfBlades& sumOfBlades, const char* variableName );
	bool AssignAntiDerivative( const SumOfBlades& sumOfBlades, const char* variableName );
	
	bool Scale( const Scalar& scalar );
	bool Reverse( void );
	bool Bar( void );

	bool Accumulate( const Blade& blade );
	bool Accumulate( const SumOfBlades& sumOfBlades );

	Blade* FindLikeTerm( const Blade& blade, Scalar& relativeHandedness ) const;

	bool IsHomogeneousOfGrade( int grade ) const;
	bool RemoveGrade( int grade );

	int BladeCount( void ) const;
	bool AssignBladeTo( Blade& blade, int index ) const;

	bool ScalarPart( const Blade& blade, Scalar& scalar ) const;
	bool GradePart( int grade, SumOfBlades& homogeneousPart ) const;

private:

	Utilities::List sum;
};

// SumOfBlades.h