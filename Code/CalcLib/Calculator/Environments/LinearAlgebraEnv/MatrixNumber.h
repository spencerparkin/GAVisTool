// MatrixNumber.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

//=========================================================================================
class CalcLib::MatrixNumber : public Number
{
	DECLARE_CALCLIB_CLASS( MatrixNumber );

	friend MatrixFunctionEvaluator;

public:

	MatrixNumber( void );
	virtual ~MatrixNumber( void );
	
	virtual Number* CreateCopy( Environment& environment ) const override;

	virtual bool Print( char* buffer, int bufferSize, bool printLatex, Environment& environment ) const override;
	virtual bool AssignFrom( const char* numberString, Environment& environment ) override;
	virtual bool AssignFrom( const Number* number, Environment& environment ) override;
	virtual bool AssignFrom( double number, Environment& environment ) override;
	virtual bool AssignTo( double& number, Environment& environment )  const override;
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

private:

	bool CopyArgumentsIfNeeded( const MatrixNumber*& leftMatrix, const MatrixNumber*& rightMatrix, const Number* left, const Number* right, Environment& environment ) const;

	LinearAlgebra::Element element;
};

// MatrixNumber.h