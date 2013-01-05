// Scalar.cpp

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "ScalarAlgebra.h"

//=========================================================================================
template<>
ScalarAlgebra::TemplatedScalar< double >::TemplatedScalar( void )
{
	scalar = 0.0;
}

//=========================================================================================
template<>
ScalarAlgebra::TemplatedScalar< double >::TemplatedScalar( const TemplatedScalar< double >& scalar )
{
	this->scalar = scalar.scalar;
}

//=========================================================================================
template<>
ScalarAlgebra::TemplatedScalar< double >::TemplatedScalar( double scalar )
{
	this->scalar = scalar;
}

//=========================================================================================
template<>
ScalarAlgebra::TemplatedScalar< double >::operator double( void ) const
{
	return scalar;
}

//=========================================================================================
template<>
bool ScalarAlgebra::TemplatedScalar< double >::operator==( const TemplatedScalar& scalar ) const
{
	if( this->scalar == scalar.scalar )
		return true;
	return false;
}

//=========================================================================================
template<>
bool ScalarAlgebra::TemplatedScalar< double >::operator==( double scalar ) const
{
	if( this->scalar == scalar )
		return true;
	return false;
}

//=========================================================================================
template<>
bool ScalarAlgebra::TemplatedScalar< double >::operator<( const TemplatedScalar& scalar ) const
{
	if( this->scalar < scalar.scalar )
		return true;
	return false;
}

//=========================================================================================
template<>
bool ScalarAlgebra::TemplatedScalar< double >::operator<( double scalar ) const
{
	if( this->scalar < scalar )
		return true;
	return false;
}

//=========================================================================================
template<>
bool ScalarAlgebra::TemplatedScalar< double >::operator>( const TemplatedScalar& scalar ) const
{
	if( this->scalar > scalar.scalar )
		return true;
	return false;
}

//=========================================================================================
template<>
bool ScalarAlgebra::TemplatedScalar< double >::operator>( double scalar ) const
{
	if( this->scalar > scalar )
		return true;
	return false;
}

//=========================================================================================
template<>
typename ScalarAlgebra::TemplatedScalar< double >& ScalarAlgebra::TemplatedScalar< double >::operator=( const TemplatedScalar< double >& scalar )
{
	this->scalar = scalar.scalar;
	return *this;
}

//=========================================================================================
template<>
typename ScalarAlgebra::TemplatedScalar< double >& ScalarAlgebra::TemplatedScalar< double >::operator=( double scalar )
{
	this->scalar = scalar;
	return *this;
}

//=========================================================================================
template<>
typename ScalarAlgebra::TemplatedScalar< double >& ScalarAlgebra::TemplatedScalar< double >::operator=( const char* scalar )
{
	// We can't support this in general.  See the specialized version of this method.
	this->scalar = 0.0;
	return *this;
}

//=========================================================================================
template<>
typename ScalarAlgebra::TemplatedScalar< double > ScalarAlgebra::TemplatedScalar< double >::operator+( const TemplatedScalar< double >& scalar ) const
{
	ScalarAlgebra::TemplatedScalar< double > result;
	result.scalar = this->scalar + scalar.scalar;
	return result;
}

//=========================================================================================
template<>
typename ScalarAlgebra::TemplatedScalar< double > ScalarAlgebra::TemplatedScalar< double >::operator+( double scalar ) const
{
	ScalarAlgebra::TemplatedScalar< double > result;
	result.scalar = this->scalar + scalar;
	return result;
}

//=========================================================================================
template<>
typename ScalarAlgebra::TemplatedScalar< double > ScalarAlgebra::TemplatedScalar< double >::operator-( const TemplatedScalar< double >& scalar ) const
{
	ScalarAlgebra::TemplatedScalar< double > result;
	result.scalar = this->scalar - scalar.scalar;
	return result;
}

//=========================================================================================
template<>
typename ScalarAlgebra::TemplatedScalar< double > ScalarAlgebra::TemplatedScalar< double >::operator-( double scalar ) const
{
	ScalarAlgebra::TemplatedScalar< double > result;
	result.scalar = this->scalar - scalar;
	return result;
}

//=========================================================================================
template<>
typename ScalarAlgebra::TemplatedScalar< double > ScalarAlgebra::TemplatedScalar< double >::operator*( const TemplatedScalar< double >& scalar ) const
{
	ScalarAlgebra::TemplatedScalar< double > result;
	result.scalar = this->scalar * scalar.scalar;
	return result;
}

//=========================================================================================
template<>
typename ScalarAlgebra::TemplatedScalar< double > ScalarAlgebra::TemplatedScalar< double >::operator*( double scalar ) const
{
	ScalarAlgebra::TemplatedScalar< double > result;
	result.scalar = this->scalar * scalar;
	return result;
}

//=========================================================================================
template<>
typename ScalarAlgebra::TemplatedScalar< double > ScalarAlgebra::TemplatedScalar< double >::operator/( const TemplatedScalar< double >& scalar ) const
{
	ScalarAlgebra::TemplatedScalar< double > result;
	result.scalar = this->scalar / scalar.scalar;
	return result;
}

//=========================================================================================
template<>
typename ScalarAlgebra::TemplatedScalar< double > ScalarAlgebra::TemplatedScalar< double >::operator/( double scalar ) const
{
	ScalarAlgebra::TemplatedScalar< double > result;
	result.scalar = this->scalar / scalar;
	return result;
}

//=========================================================================================
template<>
bool ScalarAlgebra::TemplatedScalar< double >::Invert( void )
{
	if( scalar == 0.0 )
		return false;
	scalar = 1.0 / scalar;
	return true;
}

//=========================================================================================
template<>
bool ScalarAlgebra::TemplatedScalar< double >::Negate( void )
{
	scalar = -scalar;
	return true;
}

//=========================================================================================
template<>
bool ScalarAlgebra::TemplatedScalar< double >::Differentiate( const char* variableName )
{
	scalar = 0.0;
	return true;
}

//=========================================================================================
template<>
bool ScalarAlgebra::TemplatedScalar< double >::AntiDifferentiate( const char* variableName )
{
	scalar = 0.0;
	return false;
}

//=========================================================================================
template<>
bool ScalarAlgebra::TemplatedScalar< double >::Print( char* printBuffer, int printBufferSize, PrintPurpose printPurpose, bool* parenthesisNeeded /*= 0*/ ) const
{
	sprintf_s( printBuffer, printBufferSize, "%1.2f", scalar );
	if( parenthesisNeeded )
		*parenthesisNeeded = false;
	return true;
}

//=========================================================================================
template<>
ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression >::TemplatedScalar( void )
{
	( void )scalar.AssignZero();
}

//=========================================================================================
template<>
ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression >::TemplatedScalar( const TemplatedScalar< ScalarAlgebra::RationalExpression >& scalar )
{
	this->scalar.Assign( scalar.scalar );
}

//=========================================================================================
template<>
ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression >::TemplatedScalar( double scalar )
{
	this->scalar.Assign( scalar );
}

//=========================================================================================
template<>
ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression >::operator double( void ) const
{
	double scalar;
	if( !this->scalar.AssignTo( scalar ) )
		return 0.0;
	return scalar;
}

//=========================================================================================
template<>
bool ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression >::operator==( const TemplatedScalar& scalar ) const
{
	return this->scalar.CompareWith( scalar.scalar );
}

//=========================================================================================
template<>
bool ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression >::operator==( double scalar ) const
{
	ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression > givenScalar;
	givenScalar.scalar.Assign( scalar );
	return this->scalar.CompareWith( givenScalar.scalar );
}

//=========================================================================================
template<>
bool ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression >::operator<( const TemplatedScalar& scalar ) const
{
	double thisScalar = 0.0;
	this->scalar.AssignTo( thisScalar );
	double givenScalar = 0.0;
	scalar.scalar.AssignTo( givenScalar );
	return thisScalar < givenScalar;
}

//=========================================================================================
template<>
bool ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression >::operator<( double scalar ) const
{
	double thisScalar = 0.0;
	this->scalar.AssignTo( thisScalar );
	double givenScalar = scalar;
	return thisScalar < givenScalar;
}

//=========================================================================================
template<>
bool ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression >::operator>( const TemplatedScalar& scalar ) const
{
	double thisScalar = 0.0;
	this->scalar.AssignTo( thisScalar );
	double givenScalar = 0.0;
	scalar.scalar.AssignTo( givenScalar );
	return thisScalar > givenScalar;
}

//=========================================================================================
template<>
bool ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression >::operator>( double scalar ) const
{
	double thisScalar = 0.0;
	this->scalar.AssignTo( thisScalar );
	double givenScalar = scalar;
	return thisScalar > givenScalar;
}

//=========================================================================================
template<>
typename ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression >& ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression >::operator=( const TemplatedScalar& scalar )
{
	( void )this->scalar.Assign( scalar.scalar );
	return *this;
}

//=========================================================================================
template<>
typename ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression >& ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression >::operator=( double scalar )
{
	( void )this->scalar.Assign( scalar );
	return *this;
}

//=========================================================================================
template<>
typename ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression >& ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression >::operator=( const char* scalar )
{
	( void )this->scalar.Assign( scalar );
	return *this;
}

//=========================================================================================
template<>
typename ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression > ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression >::operator+( const TemplatedScalar& scalar ) const
{
	ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression > result;
	( void )result.scalar.AssignSum( this->scalar, scalar.scalar );
	return result;
}

//=========================================================================================
template<>
typename ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression > ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression >::operator+( double scalar ) const
{
	ScalarAlgebra::RationalExpression givenScalar;
	givenScalar.Assign( scalar );

	ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression > result;
	result.scalar.AssignSum( this->scalar, givenScalar );
	return result;
}

//=========================================================================================
template<>
typename ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression > ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression >::operator-( const TemplatedScalar& scalar ) const
{
	ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression > result;
	( void )result.scalar.AssignDifference( this->scalar, scalar.scalar );
	return result;
}

//=========================================================================================
template<>
typename ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression > ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression >::operator-( double scalar ) const
{
	ScalarAlgebra::RationalExpression givenScalar;
	givenScalar.Assign( scalar );

	ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression > result;
	result.scalar.AssignDifference( this->scalar, givenScalar );
	return result;
}

//=========================================================================================
template<>
typename ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression > ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression >::operator*( const TemplatedScalar& scalar ) const
{
	ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression > result;
	( void )result.scalar.AssignProduct( this->scalar, scalar.scalar );
	return result;
}

//=========================================================================================
template<>
typename ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression > ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression >::operator*( double scalar ) const
{
	ScalarAlgebra::RationalExpression givenScalar;
	givenScalar.Assign( scalar );

	ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression > result;
	result.scalar.AssignProduct( this->scalar, givenScalar );
	return result;
}

//=========================================================================================
template<>
typename ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression > ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression >::operator/( const TemplatedScalar& scalar ) const
{
	ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression > result;
	( void )result.scalar.AssignQuotient( this->scalar, scalar.scalar );
	return result;
}

//=========================================================================================
template<>
typename ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression > ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression >::operator/( double scalar ) const
{
	ScalarAlgebra::RationalExpression givenScalar;
	givenScalar.Assign( scalar );

	ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression > result;
	result.scalar.AssignQuotient( this->scalar, givenScalar );
	return result;
}

//=========================================================================================
template<>
bool ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression >::Invert( void )
{
	if( !scalar.Recipricate() )
		return false;
	return true;
}

//=========================================================================================
template<>
bool ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression >::Negate( void )
{
	if( !scalar.Negate() )
		return false;
	return true;
}

//=========================================================================================
template<>
bool ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression >::Differentiate( const char* variableName )
{
	if( !scalar.Differentiate( variableName ) )
		return false;
	return true;
}

//=========================================================================================
template<>
bool ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression >::AntiDifferentiate( const char* variableName )
{
	if( !scalar.AntiDifferentiate( variableName ) )
		return false;
	return true;
}

//=========================================================================================
template<>
bool ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression >::Print( char* printBuffer, int printBufferSize, PrintPurpose printPurpose, bool* parenthesisNeeded /*= 0*/ ) const
{
	return scalar.Print( printBuffer, printBufferSize, printPurpose, parenthesisNeeded );
}

// Scalar.cpp