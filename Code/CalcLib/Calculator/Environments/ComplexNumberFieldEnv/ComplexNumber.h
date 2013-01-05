// ComplexNumber.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

//=========================================================================================
class CalcLib::ComplexNumber : public Number
{
	DECLARE_CALCLIB_CLASS( ComplexNumber );

public:

	ComplexNumber( double realPart, double imaginaryPart );
	virtual ~ComplexNumber( void );
	
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
	virtual bool AssignAdditiveIdentity( Environment& environment ) override;
	virtual bool AssignMultiplicativeIdentity( Environment& environment ) override;
	virtual bool IsAdditiveIdentity( bool& isAdditiveIdentity, Environment& environment ) const override;
	virtual bool IsMultiplicativeIdentity( bool& isMultiplicativeIdentity, Environment& environment ) const override;
	virtual bool CompareWith( const Number* number, Comparison& comparison, Environment& environment ) const override;

private:

	bool VerifyArguments( const ComplexNumber*& leftNumber, const ComplexNumber*& rightNumber, const Number* left, const Number* right, Environment& environment );

	double realPart;
	double imaginaryPart;
};

// ComplexNumber.h