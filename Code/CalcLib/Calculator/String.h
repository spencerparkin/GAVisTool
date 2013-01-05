// String.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

//=========================================================================================
class CalcLib::StringNumber : public Number
{
	DECLARE_CALCLIB_CLASS( StringNumber );

public:

	StringNumber( void );
	virtual ~StringNumber( void );
	
	virtual Number* CreateCopy( Environment& environment ) const override;

	virtual bool Print( char* buffer, int bufferSize, bool printLatex, Environment& environment ) const override;
	virtual bool AssignFrom( const char* numberString, Environment& environment ) override;
	virtual bool AssignFrom( const Number* number, Environment& environment ) override;
	virtual bool AssignFrom( double number, Environment& environment ) override;
	virtual bool AssignTo( Number& number, Environment& environment ) const override;
	virtual bool AssignTo( double& number, Environment& environment )  const override;
	virtual bool AssignSum( const Number* left, const Number* right, Environment& environment ) override;
	virtual bool AssignDifference( const Number* left, const Number* right, Environment& environment ) override;
	virtual bool AssignProduct( const Number* left, const Number* right, Environment& environment ) override;
	virtual bool AssignQuotient( const Number* left, const Number* right, Environment& environment ) override;
	virtual bool AssignAdditiveIdentity( Environment& environment ) override;
	virtual bool AssignMultiplicativeIdentity( Environment& environment ) override;
	virtual bool IsAdditiveIdentity( bool& isAdditiveIdentity, Environment& environment ) const override;
	virtual bool IsMultiplicativeIdentity( bool& isMultiplicativeIdentity, Environment& environment ) const override;

	bool Execute( Number& result, Environment& environment ) const;
	const char* String( void ) const;

private:

	bool RefreshCache( Environment& environment ) const;
	void WipeCache( void ) const;
	void InvalidateCache( void ) const;

	char* string;
	mutable Evaluator* cachedEvaluationTree;
	mutable bool cacheIsValid;
	static Tokenizer tokenizer;
	static Parser parser;
	static int recursionLevel;
};

// String.h