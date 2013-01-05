// Number.cpp

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "CalcLib.h"

using namespace CalcLib;

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS0( Number );

//=========================================================================================
Number::Number( void )
{
}

//=========================================================================================
/*virtual*/ Number::~Number( void )
{
}

//=========================================================================================
bool Number::Print( Environment& environment ) const
{
	char buffer[ PRINT_BUFFER_SIZE_LARGE ];
	if( Print( buffer, sizeof( buffer ), false, environment ) )
	{
		environment.Print( buffer );
		return true;
	}
	return false;
}

//=========================================================================================
/*virtual*/ bool Number::AssignTo( Number& number, Environment& environment ) const
{
	return number.AssignFrom( this, environment );
}

//=========================================================================================
/*virtual*/ bool Number::AssignAdditiveInverse( const Number* number, Environment& environment )
{
	bool success = false;
	Number* zero = 0;
	
	do
	{
		zero = environment.CreateNumber(0);
		if( !zero )
			break;

		if( !zero->AssignAdditiveIdentity( environment ) )
			break;

		if( !AssignDifference( zero, number, environment ) )
			break;

		success = true;
	}
	while( false );

	delete zero;
	
	return success;
}

//=========================================================================================
/*virtual*/ bool Number::AssignMultilicativeInverse( const Number* number, Environment& environment )
{
	bool success = false;
	Number* one = 0;

	do
	{
		one = environment.CreateNumber(0);
		if( !one )
			break;

		if( !one->AssignMultiplicativeIdentity( environment ) )
			break;

		if( !AssignQuotient( one, number, environment ) )
			break;

		success = true;
	}
	while( false );

	delete one;
	
	return false;
}

//=========================================================================================
/*virtual*/ bool Number::AssignInnerProduct( const Number* left, const Number* right, Environment& environment )
{
	environment.AddError( "The derived class did not implement the inner product." );
	return false;
}

//=========================================================================================
/*virtual*/ bool Number::AssignOuterProduct( const Number* left, const Number* right, Environment& environment )
{
	environment.AddError( "The derived class did not implement the outer product." );
	return false;
}

//=========================================================================================
/*virtual*/ bool Number::CompareWith( const Number* number, Comparison& comparison, Environment& environment ) const
{
	comparison = COMPARISON_UNKNOWN;
	return true;
}

//=========================================================================================
/*virtual*/ bool Number::AssignDerivative( const Number* number, const char* variableName, Environment& environment )
{
	return false;
}

//=========================================================================================
/*virtual*/ bool Number::AssignAntiDerivative( const Number* number, const char* variableName, Environment& environment )
{
	return false;
}

// Number.cpp