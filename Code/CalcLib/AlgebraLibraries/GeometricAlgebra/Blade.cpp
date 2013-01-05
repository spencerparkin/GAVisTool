// Blade.cpp

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
Blade::Blade( void )
{
	sorted = false;
	scalar = 0.0;
}

//=========================================================================================
/*virtual*/ Blade::~Blade( void )
{
	product.RemoveAll( true );
}

//=========================================================================================
bool Blade::Print( char* printBuffer, int printBufferSize, ScalarAlgebra::PrintPurpose printPurpose ) const
{
	printBuffer[0] = '\0';

	if( printPurpose == ScalarAlgebra::PRINT_FOR_READING || printPurpose == ScalarAlgebra::PRINT_FOR_LATEX )
	{
		bool printScalarNeeded = false;
		bool parenthesisNeeded = false;

		char scalarString[ PRINT_BUFFER_SIZE_LARGE ];
		scalarString[0] = '\0';

		if( Grade() > 0 )
		{
			if( scalar == 1.0 )
				;		// An empty string is fine.
			else if( scalar == -1.0 )
				strcat_s( scalarString, sizeof( scalarString ), "-" );
			else
				printScalarNeeded = true;
		}
		else
			printScalarNeeded = true;
			
		if( printScalarNeeded )
		{
			if( !scalar.Print( scalarString, sizeof( scalarString ), printPurpose, &parenthesisNeeded ) )
				return false;
		}

		if( parenthesisNeeded && Grade() > 0 )
		{
			if( printPurpose == ScalarAlgebra::PRINT_FOR_LATEX )
				sprintf_s( printBuffer, printBufferSize, "\\left(%s\\right)", scalarString );
			else
				sprintf_s( printBuffer, printBufferSize, "(%s)", scalarString );
		}
		else
			strcat_s( printBuffer, printBufferSize, scalarString );

		if( printScalarNeeded && Grade() > 0 && printPurpose != ScalarAlgebra::PRINT_FOR_LATEX )
			strcat_s( printBuffer, printBufferSize, "*" );
	}

	for( Vector* vector = ( Vector* )product.LeftMost(); vector; vector = ( Vector* )vector->Right() )
	{
		if( printPurpose == ScalarAlgebra::PRINT_FOR_LATEX )
			strcat_s( printBuffer, printBufferSize, vector->LatexName() );
		else
			strcat_s( printBuffer, printBufferSize, vector->Name() );
		if( vector->Right() )
		{
			if( printPurpose == ScalarAlgebra::PRINT_FOR_LATEX )
				strcat_s( printBuffer, printBufferSize, "\\wedge " );
			else
				strcat_s( printBuffer, printBufferSize, "^" );
		}
	}

	return true;
}

//=========================================================================================
bool Blade::AssignZero( void )
{
	product.RemoveAll( true );
	scalar = 0.0;
	sorted = true;
	return true;
}

//=========================================================================================
bool Blade::AssignScalar( const Scalar& scalar )
{
	product.RemoveAll( true );
	this->scalar = scalar;
	sorted = true;
	return true;
}

//=========================================================================================
bool Blade::AssignScalar( const char* scalar )
{
	product.RemoveAll( true );
	this->scalar = scalar;
	sorted = true;
	return true;
}

//=========================================================================================
bool Blade::AssignScalarPartTo( Scalar& scalar ) const
{
	scalar = this->scalar;
	return true;
}

//=========================================================================================
bool Blade::AssignVector( const Vector& vector, const Scalar& scalar )
{
	product.RemoveAll( true );
	product.InsertLeftOf( 0, ( Vector* )vector.MakeCopy() );
	this->scalar = scalar;
	sorted = true;
	return true;
}

//=========================================================================================
bool Blade::AssignBlade( const Blade& blade, bool sortProduct /*= true*/ )
{
	product.RemoveAll( true );
	product.ConcatinateCopyOnLeft( blade.product );
	scalar = blade.scalar;
	sorted = blade.sorted;

	if( sortProduct && !Sort() )
		return false;

	return true;
}

//=========================================================================================
// Notice that we empty the given list into our list and we assume ownership of the memory.
bool Blade::AssignBlade( Utilities::List& outerProduct, const Scalar& scalar, bool sortProduct /* = true*/ )
{
	product.RemoveAll( true );
	while( outerProduct.Count() > 0 )
	{
		Vector* vector = ( Vector* )outerProduct.LeftMost();
		outerProduct.Remove( vector, false );
		product.InsertRightOf( product.RightMost(), vector );
	}
	this->scalar = scalar;
	sorted = false;

	if( sortProduct && !Sort() )
		return false;

	return true;
}

//=========================================================================================
bool Blade::AssignOuterProduct( const Blade& left, const Blade& right, bool sortProduct /*= true*/ )
{
	product.RemoveAll( true );
	scalar = 0.0;
	sorted = true;

	// All vectors in a blade must for a linearly independent set.
	for( const Vector* leftVector = ( const Vector* )left.product.LeftMost(); leftVector; leftVector = ( const Vector* )leftVector->Right() )
		for( const Vector* rightVector = ( const Vector* )right.product.LeftMost(); rightVector; rightVector = ( const Vector* )rightVector->Right() )
			if( leftVector->SortCompare( rightVector ) == Utilities::List::SORT_COMPARE_EQUAL_TO )
				return true;

	product.ConcatinateCopyOnLeft( left.product );
	product.ConcatinateCopyOnRight( right.product );
	scalar = left.scalar * right.scalar;
	sorted = false;

	// Keeping blades sorted upon creation makes the process of finding
	// like-terms much faster.  However, it is often useful to be able
	// detect like-terms among blades having different product orders.
	// For example, the canonical product orders being used by the user
	// of the geometric algebra library may not be the same as the product
	// orders being used internally by the library.
	if( sortProduct && !Sort() )
		return false;

	return true;
}

//=========================================================================================
bool Blade::AssignSquareMagnitudeTo( Scalar& squareMagnitude ) const
{
	squareMagnitude = scalar * scalar;
	return true;
}

//=========================================================================================
bool Blade::AssignDerivative( const Blade& blade, const char* variableName )
{
	if( !AssignBlade( blade ) )
		return false;

	if( !scalar.Differentiate( variableName ) )
		return false;

	return true;
}

//=========================================================================================
bool Blade::AssignAntiDerivative( const Blade& blade, const char* variableName )
{
	if( !AssignBlade( blade ) )
		return false;

	if( !scalar.AntiDifferentiate( variableName ) )
		return false;

	return true;
}

//=========================================================================================
bool Blade::IsLikeTerm( const Blade& blade, Scalar& relativeHandedness ) const
{
	if( Grade() != blade.Grade() )
		return false;

	if( sorted && blade.sorted )
	{
		const Vector* thisVector = ( const Vector* )product.LeftMost();
		const Vector* givenVector = ( const Vector* )blade.product.LeftMost();
		while( thisVector && givenVector )
		{
			if( thisVector->SortCompare( givenVector ) != Utilities::List::SORT_COMPARE_EQUAL_TO )
				return false;

			thisVector = ( const Vector* )thisVector->Right();
			givenVector = ( const Vector* )givenVector->Right();
		}
		relativeHandedness = 1.0;
		return true;
	}

	Blade thisBlade, givenBlade;
	
	if( !thisBlade.AssignBlade( *this ) )
		return false;

	if( !givenBlade.AssignBlade( blade ) )
		return false;

	int thisSwapCount;
	if( !thisBlade.Sort( &thisSwapCount ) )
		return false;

	int givenSwapCount;
	if( !givenBlade.Sort( &givenSwapCount ) )
		return false;

	bool likeTerms = thisBlade.IsLikeTerm( givenBlade, relativeHandedness );

	if( ( thisSwapCount + givenSwapCount ) % 2 == 1 )
		relativeHandedness = -1.0;
	else
		relativeHandedness = 1.0;

	return likeTerms;
}

//=========================================================================================
bool Blade::Scale( const Scalar& scalar )
{
	this->scalar = this->scalar * scalar;
	return true;
}

//=========================================================================================
bool Blade::RemoveVector( const char* vectorName )
{
	for( Vector* vector = ( Vector* )product.LeftMost(); vector; vector = ( Vector* )vector->Right() )
	{
		if( 0 == strcmp( vector->Name(), vectorName ) )
		{
			product.Remove( vector, true );
			return true;
		}
	}
	return false;
}

//=========================================================================================
bool Blade::Bar( void )
{
	Scalar sign;
	Vector* nextVector = 0;
	for( Vector* vector = ( Vector* )product.LeftMost(); vector; vector = nextVector )
	{
		nextVector = ( Vector* )vector->Right();

		Vector* barVector = vector->MakeBar( sign );
		if( !barVector )
			return false;

		scalar = scalar * sign;

		product.Remove( vector, true );
		if( nextVector )
			product.InsertLeftOf( nextVector, barVector );
		else
			product.InsertRightOf( product.RightMost(), barVector );
	}

	return true;
}

//=========================================================================================
int Blade::Grade( void ) const
{
	return product.Count();
}

//=========================================================================================
bool Blade::Sort( int* adjacentSwapCount /*= 0*/ )
{
	if( sorted )
		return true;

	int swapCount = 0;
	if( !adjacentSwapCount )
		adjacentSwapCount = &swapCount;

	product.Sort( Utilities::List::SORT_ORDER_ASCENDING, adjacentSwapCount );
	if( *adjacentSwapCount % 2 == 1 )
		scalar = scalar * -1.0;
	
	sorted = true;
	return true;
}

//=========================================================================================
/*virtual*/ Utilities::List::SortComparison Blade::SortCompare( const Item* item ) const
{
	const Blade* blade = ( const Blade* )item;

	char leftString[ PRINT_BUFFER_SIZE_SMALL ];
	char rightString[ PRINT_BUFFER_SIZE_SMALL ];

	Print( leftString, sizeof( leftString ), ScalarAlgebra::PRINT_FOR_SORTING );
	blade->Print( rightString, sizeof( rightString ), ScalarAlgebra::PRINT_FOR_SORTING );

	int cmp = strcmp( leftString, rightString );
	if( cmp < 0 )
		return Utilities::List::SORT_COMPARE_LESS_THAN;
	else if( cmp > 0 )
		return Utilities::List::SORT_COMPARE_GREATER_THAN;
	return Utilities::List::SORT_COMPARE_EQUAL_TO;
}

//=========================================================================================
/*virtual*/ Utilities::List::Item* Blade::MakeCopy( void ) const
{
	Blade* blade = new Blade();
	blade->AssignBlade( *this );
	return blade;
}

// Blade.cpp