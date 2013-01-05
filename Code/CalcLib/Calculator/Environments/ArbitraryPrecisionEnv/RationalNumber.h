// RationalNumber.h

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
class CalcLib::RationalNumber : public Number
{
	DECLARE_CALCLIB_CLASS( RationalNumber );

public:

	RationalNumber( int base );
	RationalNumber( int number, int base );
	virtual ~RationalNumber( void );
	
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

	bool RoundToNearestDegree( int degree );
	bool ConvertToBase( int base, RationalNumber& result, Environment& environment ) const;

private:

	class Term : public Utilities::List::Item
	{
	public:
		Term( int coeficient, int degree );
		Term( const Term& term );
		virtual ~Term( void );

		virtual Utilities::List::SortComparison SortCompare( const Item* compareWithItem ) const;
		virtual Item* MakeCopy( void ) const;
		virtual void DeleteThis( void );

		bool Print( char* buffer, int bufferSize, int base ) const;

		int coeficient;
		int degree;
	};

	enum Sign
	{
		SIGN_UNKNOWN,
		SIGN_NEGATIVE,
		SIGN_POSITIVE,
	};

	Sign SignOfLeadingCoeficient( void ) const;
	void Accumulate( const RationalNumber& rationalNumber, bool negate );
	void Accumulate( const Term& term );
	void AccumulationAlgorithm( const Term& term );
	Term* FindLikeTerm( int degree );
	bool FindTermWithSignHavingSmallestDegree( Term*& foundTerm, Sign sign  );
	static int MapDigitCharToCeoficient( char digitChar );
	static char MapCoeficientToDigitChar( int coeficient );
	double EvaluatePolynomial( bool& precisionLost ) const;
	bool CalculateResidueOfWholePart( int& residue, int modulus );
	void CalculateResidue( int& residue, int modulus );
	void RemoveAllTermsOfDegreeLessThan( int degree );
	void RemoveAllTermsOfDegreeGreaterThan( int degree );

	Utilities::List listOfTerms;
	int base;
};

// RationalNumber.h