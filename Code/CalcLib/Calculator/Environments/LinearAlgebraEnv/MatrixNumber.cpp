// MatrixNumber.cpp

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "../../CalcLib.h"

using namespace CalcLib;

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( MatrixNumber, Number );

//=========================================================================================
MatrixNumber::MatrixNumber( void )
{
}

//=========================================================================================
/*virtual*/ MatrixNumber::~MatrixNumber( void )
{
}

//=========================================================================================
/*virtual*/ Number* MatrixNumber::CreateCopy( Environment& environment ) const
{
	return 0;
}

//=========================================================================================
/*virtual*/ bool MatrixNumber::Print( char* buffer, int bufferSize, bool printLatex, Environment& environment ) const
{
	ScalarAlgebra::PrintPurpose printPurpose = ScalarAlgebra::PRINT_FOR_READING;
	if( printLatex )
		printPurpose = ScalarAlgebra::PRINT_FOR_SORTING;

	if( !element.Print( buffer, bufferSize, printPurpose ) )
		return false;

	return true;
}

//=========================================================================================
/*virtual*/ bool MatrixNumber::AssignFrom( const char* numberString, Environment& environment )
{
	// We might want to support a matrix syntax here such as [[1,2][3,4]], but for now,
	// here we only recognize scalars.  So to create a matrix and manipulate it, you must
	// use functions.

	LinearAlgebra::Scalar scalar;
	if( *numberString != '$' )
		scalar = atof( numberString );
	else
		scalar = ++numberString;
	
	if( !element.AssignScalarFrom( scalar ) )
		return false;

	return true;
}

//=========================================================================================
/*virtual*/ bool MatrixNumber::AssignFrom( const Number* number, Environment& environment )
{
	if( !number->IsTypeOf( MatrixNumber::ClassName() ) )
		return false;

	MatrixNumber* matrixNumber = ( MatrixNumber* )number;
	if( !element.AssignFrom( matrixNumber->element ) )
		return false;

	return true;
}

//=========================================================================================
/*virtual*/ bool MatrixNumber::AssignFrom( double number, Environment& environment )
{
	LinearAlgebra::Scalar scalar;
	scalar = number;

	if( !element.AssignScalarFrom( scalar ) )
		return false;

	return true;
}

//=========================================================================================
/*virtual*/ bool MatrixNumber::AssignTo( double& number, Environment& environment ) const
{
	const LinearAlgebra::Scalar* scalar = element.AccessScalar();
	if( !scalar )
		return false;

	number = *scalar;
	return true;
}

//=========================================================================================
bool MatrixNumber::CopyArgumentsIfNeeded( const MatrixNumber*& leftMatrix, const MatrixNumber*& rightMatrix, const Number* left, const Number* right, Environment& environment ) const
{
	static MatrixNumber leftCopy, rightCopy;

	if( !( left->IsTypeOf( MatrixNumber::ClassName() ) && right->IsTypeOf( MatrixNumber::ClassName() ) ) )
		return false;

	leftMatrix = ( MatrixNumber* )left;
	rightMatrix = ( MatrixNumber* )right;

	if( leftMatrix == this )
	{
		if( !leftCopy.AssignFrom( this, environment ) )
			return false;
		leftMatrix = &leftCopy;
	}

	if( rightMatrix == this )
	{
		if( !rightCopy.AssignFrom( this, environment ) )
			return false;
		rightMatrix = &rightCopy;
	}

	return true;
}

//=========================================================================================
/*virtual*/ bool MatrixNumber::AssignSum( const Number* left, const Number* right, Environment& environment )
{
	const MatrixNumber* leftMatrix, *rightMatrix;
	if( !CopyArgumentsIfNeeded( leftMatrix, rightMatrix, left, right, environment ) )
		return false;

	if( !element.AssignSum( leftMatrix->element, rightMatrix->element ) )
		return false;

	return true;
}

//=========================================================================================
/*virtual*/ bool MatrixNumber::AssignDifference( const Number* left, const Number* right, Environment& environment )
{
	const MatrixNumber* leftMatrix, *rightMatrix;
	if( !CopyArgumentsIfNeeded( leftMatrix, rightMatrix, left, right, environment ) )
		return false;

	LinearAlgebra::Element element;
	if( !element.AssignFrom( rightMatrix->element ) )
		return false;

	if( !element.Scale( -1.0 ) )
		return false;

	if( !this->element.AssignSum( leftMatrix->element, element ) )
		return false;

	return true;
}

//=========================================================================================
/*virtual*/ bool MatrixNumber::AssignProduct( const Number* left, const Number* right, Environment& environment )
{
	const MatrixNumber* leftMatrix, *rightMatrix;
	if( !CopyArgumentsIfNeeded( leftMatrix, rightMatrix, left, right, environment ) )
		return false;

	if( !element.AssignProduct( leftMatrix->element, rightMatrix->element ) )
		return false;

	return true;
}

//=========================================================================================
/*virtual*/ bool MatrixNumber::AssignQuotient( const Number* left, const Number* right, Environment& environment )
{
	const MatrixNumber* leftMatrix, *rightMatrix;
	if( !CopyArgumentsIfNeeded( leftMatrix, rightMatrix, left, right, environment ) )
		return false;

	LinearAlgebra::Element element;
	if( !element.AssignInverse( rightMatrix->element ) )
		return false;

	if( !this->element.AssignProduct( leftMatrix->element, element ) )
		return false;

	return true;
}

//=========================================================================================
/*virtual*/ bool MatrixNumber::AssignAdditiveInverse( const Number* number, Environment& environment )
{
	if( !number->IsTypeOf( MatrixNumber::ClassName() ) )
		return false;
	const MatrixNumber* matrixNumber = ( const MatrixNumber* )number;

	if( !element.AssignFrom( matrixNumber->element ) )
		return false;

	if( !element.Scale( -1.0 ) )
		return false;

	return true;
}

//=========================================================================================
/*virtual*/ bool MatrixNumber::AssignMultilicativeInverse( const Number* number, Environment& environment )
{
	if( !number->IsTypeOf( MatrixNumber::ClassName() ) )
		return false;
	const MatrixNumber* matrixNumber = ( const MatrixNumber* )number;

	if( !element.AssignInverse( matrixNumber->element ) )
		return false;

	return true;
}

//=========================================================================================
/*virtual*/ bool MatrixNumber::AssignAdditiveIdentity( Environment& environment )
{
	if( !element.AssignScalar( 0.0 ) )
		return false;

	return true;
}

//=========================================================================================
/*virtual*/ bool MatrixNumber::AssignMultiplicativeIdentity( Environment& environment )
{
	if( !element.AssignIdentity() )
		return false;

	return true;
}

//=========================================================================================
/*virtual*/ bool MatrixNumber::AssignInnerProduct( const Number* left, const Number* right, Environment& environment )
{
	// Research this.
	return false;
}

//=========================================================================================
/*virtual*/ bool MatrixNumber::AssignOuterProduct( const Number* left, const Number* right, Environment& environment )
{
	// Research this.
	return false;
}

//=========================================================================================
/*virtual*/ bool MatrixNumber::IsAdditiveIdentity( bool& isAdditiveIdentity, Environment& environment ) const
{
	return false;
}

//=========================================================================================
/*virtual*/ bool MatrixNumber::IsMultiplicativeIdentity( bool& isMultiplicativeIdentity, Environment& environment ) const
{
	return false;
}

//=========================================================================================
/*virtual*/ bool MatrixNumber::CompareWith( const Number* number, Comparison& comparison, Environment& environment ) const
{
	return false;
}

// MatrixNumber.cpp