// MultivectorNumber.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

//=========================================================================================
class CalcLib::MultivectorNumber : public Number
{
	DECLARE_CALCLIB_CLASS( MultivectorNumber );

public:

	MultivectorNumber( void );
	MultivectorNumber( const GeometricAlgebra::Vector& vector );
	MultivectorNumber( const GeometricAlgebra::Blade& blade );
	MultivectorNumber( const GeometricAlgebra::SumOfBlades& multivector );
	virtual ~MultivectorNumber( void );

	virtual Number* CreateCopy( Environment& environment ) const override;

	virtual bool Print( char* buffer, int bufferSize, bool printLatex, Environment& environment ) const override;
	virtual bool AssignFrom( const char* numberString, Environment& environment ) override;
	virtual bool AssignFrom( const Number* number, Environment& environment ) override;
	virtual bool AssignFrom( double number, Environment& environment ) override;
	virtual bool AssignFrom( const GeometricAlgebra::Scalar& scalar, Environment& environment );
	virtual bool AssignFrom( const GeometricAlgebra::SumOfBlades& sumOfBlades, Environment& environment );
	virtual bool AssignTo( double& number, Environment& environment )  const override;
	virtual bool AssignTo( GeometricAlgebra::Blade& blade, Environment& environment )const;
	virtual bool AssignTo( GeometricAlgebra::SumOfBlades& sumOfBlades, Environment& environment ) const;
	virtual bool AssignSum( const Number* left, const Number* right, Environment& environment ) override;
	virtual bool AssignDifference( const Number* left, const Number* right, Environment& environment ) override;
	virtual bool AssignProduct( const Number* left, const Number* right, Environment& environment ) override;
	virtual bool AssignQuotient( const Number* left, const Number* right, Environment& environment ) override;
	virtual bool AssignAdditiveInverse( const Number* number, Environment& environment ) override;
	virtual bool AssignMultilicativeInverse( const Number* number, Environment& environment ) override;
	virtual bool AssignAdditiveIdentity( Environment& environment ) override;
	virtual bool AssignMultiplicativeIdentity( Environment& environment ) override;
	virtual bool AssignInnerProduct( const Number* left, const Number* right, Environment& environment ) override;
	virtual bool AssignOuterProduct( const Number* left, const Number* right, Environment& environment ) override;
	virtual bool IsAdditiveIdentity( bool& isAdditiveIdentity, Environment& environment ) const override;
	virtual bool IsMultiplicativeIdentity( bool& isMultiplicativeIdentity, Environment& environment ) const override;
	virtual bool CompareWith( const Number* number, Comparison& comparison, Environment& environment ) const override;
	virtual bool AssignDerivative( const Number* number, const char* variableName, Environment& environment ) override;
	virtual bool AssignAntiDerivative( const Number* number, const char* variableName, Environment& environment ) override;

	bool Reverse( Environment& environment );
	bool Bar( Environment& environment );

	bool CopyOperandsIfNeeded( const Number* left, const Number* right,
									const MultivectorNumber*& leftMultivectorNumber,
									const MultivectorNumber*& rightMultivectorNumber,
									Environment& environment ) const;

	

private:

	GeometricAlgebra::SumOfBlades multivector;
};

// MultivectorNumber.h