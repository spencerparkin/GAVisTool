// PseudoVersor.cpp

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "GeometricAlgebra.h"

using namespace GeometricAlgebra;

//=========================================================================================
PseudoVersor::PseudoVersor( void )
{
	scalar = 0.0;
}

//=========================================================================================
/*virtual*/ PseudoVersor::~PseudoVersor( void )
{
	product.RemoveAll( true );
}

//=========================================================================================
bool PseudoVersor::Print( char* printBuffer, int printBufferSize, ScalarAlgebra::PrintPurpose printPurpose ) const
{
	printBuffer[0] = '\0';

	if( !( scalar == 1.0 ) || product.Count() == 0 )
	{
		bool parenthesisNeeded = false;
		char scalarString[ PRINT_BUFFER_SIZE_LARGE ];
		if( !scalar.Print( scalarString, sizeof( scalarString ), printPurpose, &parenthesisNeeded ) )
			return false;

		if( parenthesisNeeded && product.Count() > 0 )
			sprintf_s( printBuffer, printBufferSize, "(%s)", scalarString );
		else
			strcat_s( printBuffer, printBufferSize, scalarString );

		if( product.Count() > 0 )
			strcat_s( printBuffer, printBufferSize, "*" );
	}

	for( const Vector* vector = ( const Vector* )product.LeftMost(); vector; vector = ( const Vector* )vector->Right() )
	{
		strcat_s( printBuffer, printBufferSize, vector->Name() );
		if( vector->Right() )
			strcat_s( printBuffer, printBufferSize, "*" );
	}

	return true;
}

//=========================================================================================
bool PseudoVersor::AssignZero( void )
{
	product.RemoveAll( true );
	scalar = 0.0;
	return true;
}

//=========================================================================================
bool PseudoVersor::AssignScalar( const Scalar& scalar )
{
	product.RemoveAll( true );
	this->scalar = scalar;
	return true;
}

//=========================================================================================
bool PseudoVersor::AssignVector( const Vector& vector, const Scalar& scalar )
{
	product.RemoveAll( true );
	product.InsertLeftOf( 0, ( Vector* )vector.MakeCopy() );
	this->scalar = scalar;
	return true;
}

//=========================================================================================
bool PseudoVersor::AssignVersor( const PseudoVersor& pseudoVersor )
{
	product.RemoveAll( true );
	product.ConcatinateCopyOnLeft( pseudoVersor.product );
	scalar = pseudoVersor.scalar;
	return true;
}

//=========================================================================================
bool PseudoVersor::AssignGeometricProduct( const PseudoVersor& left, const PseudoVersor& right )
{
	product.RemoveAll( true );
	product.ConcatinateCopyOnLeft( left.product );
	product.ConcatinateCopyOnRight( right.product );
	scalar = left.scalar * right.scalar;

	// This call is not and should not be needed for the correctness
	// of our algorithms in performing any algebraic computations.
	// Done here, this provides an opportunity for optimization.
	// The smaller the pseudoVersor we create here, the faster we can
	// convert it back to a sum of blades.
	Simplify();

	return true;
}

//=========================================================================================
bool PseudoVersor::IsLikeTerm( const PseudoVersor& pseudoVersor ) const
{
	if( Grade() != pseudoVersor.Grade() )
		return false;

	const Vector* thisVector = ( const Vector* )product.LeftMost();
	const Vector* givenVector = ( const Vector* )pseudoVersor.product.LeftMost();
	while( thisVector && givenVector )
	{
		if( thisVector->SortCompare( givenVector ) != Utilities::List::SORT_COMPARE_EQUAL_TO )
			return false;

		thisVector = ( const Vector* )thisVector->Right();
		givenVector = ( const Vector* )givenVector->Right();
	}

	return true;
}

//=========================================================================================
bool PseudoVersor::Scale( const Scalar& scalar )
{
	this->scalar = this->scalar * scalar;
	return true;
}

//=========================================================================================
bool PseudoVersor::Reverse( void )
{
	return product.Reverse();
}

//=========================================================================================
// I'm not sure what the pseudoVersor's equivilant of a blade's grade is, but I'm going to call
// it the grade of the pseudoVersor.
int PseudoVersor::Grade( void ) const
{
	return product.Count();
}

//=========================================================================================
bool PseudoVersor::IsVersor( void ) const
{
	for( const Vector* vector = ( const Vector* )product.LeftMost(); vector; vector = ( const Vector* )vector->Right() )
	{
		double innerProduct = vector->InnerProduct( *vector );
		if( 0.0 == innerProduct )
			return false;
	}
	return true;
}

//=========================================================================================
/*virtual*/ Utilities::List::SortComparison PseudoVersor::SortCompare( const Item* item ) const
{
	return Utilities::List::SORT_COMPARE_UNKNOWN;
}

//=========================================================================================
/*virtual*/ Utilities::List::Item* PseudoVersor::MakeCopy( void ) const
{
	PseudoVersor* pseudoVersor = new PseudoVersor();
	pseudoVersor->AssignVersor( *this );
	return pseudoVersor;
}

//=========================================================================================
void PseudoVersor::Simplify( void )
{
	bool simplificationMade = false;
	do
	{
		simplificationMade = MakeSimplification();
	}
	while( simplificationMade );
}

//=========================================================================================
bool PseudoVersor::MakeSimplification( void )
{
	for( Vector* left = ( Vector* )product.LeftMost(); left; left = ( Vector* )left->Right() )
	{
		for( Vector* right = ( Vector* )left->Right(); right; right = ( Vector* )right->Right() )
		{
			if( left->SortCompare( right ) == Utilities::List::SORT_COMPARE_EQUAL_TO )
			{
				Scalar handednessForAdjacency;
				if( CanBeMadeAdjacent( left, right, handednessForAdjacency ) )
				{
					Scalar innerProduct = left->InnerProduct( *right );
					if( innerProduct == 0.0 )
					{
						product.RemoveAll( true );
						scalar = 0.0;
					}
					else
					{
						product.Remove( left, true );
						product.Remove( right, true );
						scalar = scalar * innerProduct * handednessForAdjacency;
					}
					return true;
				}
			}
		}
	}
	return false;
}

//=========================================================================================
// Assuming that the given vectors are part of this pseudoVersor's product,
// we calculate here the new "handedness of the pseudoVersor", if you will,
// in the event that the two vectors can be made adjacent.  We do not
// actually make them adjacent in the pseudoVersor.  Two such vectors can
// be made adjacent through a series of adjacent swaps.  An adjacent
// swap in a pseudoVersor is allowed if the two vectors in question have
// an inner product of zero.  We also assume that the given left vector
// is left of the given right vector.  If this is not so, we will crash!
bool PseudoVersor::CanBeMadeAdjacent( const Vector* left, const Vector* right, Scalar& handednessForAdjacency ) const
{
	int adjacentSwapCount = 0;
	const Vector* adjacent = ( const Vector* )left->Right();
	while( adjacent != right )
	{
		if( 0.0 != left->InnerProduct( *adjacent ) )
			return false;
		adjacent = ( const Vector* )adjacent->Right();
		adjacentSwapCount++;
	}
	if( adjacentSwapCount % 2 == 1 )
		handednessForAdjacency = -1.0;
	else
		handednessForAdjacency = 1.0;
	return true;
}

// PseudoVersor.cpp