// SumOfPseudoVersors.cpp

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
SumOfPseudoVersors::SumOfPseudoVersors( void )
{
}

//=========================================================================================
/*virtual*/ SumOfPseudoVersors::~SumOfPseudoVersors( void )
{
	sum.RemoveAll( true );
}

//=========================================================================================
bool SumOfPseudoVersors::Print( char* printBuffer, int printBufferSize, ScalarAlgebra::PrintPurpose printPurpose ) const
{
	return false;
}

//=========================================================================================
bool SumOfPseudoVersors::AssignZero( void )
{
	sum.RemoveAll( true );
	return true;
}

//=========================================================================================
bool SumOfPseudoVersors::AssignScalar( const Scalar& scalar )
{
	sum.RemoveAll( true );
	PseudoVersor* pseudoVersor = new PseudoVersor();
	sum.InsertLeftOf( 0, pseudoVersor );
	return pseudoVersor->AssignScalar( scalar );
}

//=========================================================================================
bool SumOfPseudoVersors::AssignVector( const Vector& vector )
{
	sum.RemoveAll( true );
	PseudoVersor* pseudoVersor = new PseudoVersor();
	sum.InsertLeftOf( 0, pseudoVersor );
	return pseudoVersor->AssignVector( vector, Scalar( 1.0 ) );
}

//=========================================================================================
bool SumOfPseudoVersors::AssignBlade( const Blade& blade )
{
	sum.RemoveAll( true );

	if( blade.Grade() <= 1 )
	{
		PseudoVersor* pseudoVersor = new PseudoVersor();
		sum.InsertLeftOf( 0, pseudoVersor );
		const Vector* vector = ( const Vector* )blade.product.LeftMost();
		if( vector )
		{
			if( !pseudoVersor->AssignVector( *vector, blade.scalar ) )
				return false;
		}
		else
		{
			if( !pseudoVersor->AssignScalar( blade.scalar ) )
				return false;
		}
		return true;
	}

	Blade subBlade;
	if( !subBlade.AssignBlade( blade ) )
		return false;

	Vector* vector = ( Vector* )subBlade.product.LeftMost();
	subBlade.product.Remove( vector, false );
	Utilities::ScopeDelete< Vector > scopeDelete( vector );

	SumOfPseudoVersors sumOfPseudoVersors;
	if( !sumOfPseudoVersors.AssignBlade( subBlade ) )
		return false;

	if( !AssignGeometricProduct( *vector, sumOfPseudoVersors ) )
		return false;

	SumOfBlades sumOfBlades;
	if( !sumOfBlades.AssignInnerProduct( *vector, subBlade ) )
		return false;

	for( Blade* term = ( Blade* )sumOfBlades.sum.LeftMost(); term; term = ( Blade* )term->Right() )
	{
		if( !sumOfPseudoVersors.AssignBlade( *term ) )
			return false;

		if( !sumOfPseudoVersors.Scale( -1.0 ) )
			return false;

		if( !Accumulate( sumOfPseudoVersors ) )
			return false;
	}

	return true;
}

//=========================================================================================
bool SumOfPseudoVersors::AssignVersor( const PseudoVersor& pseudoVersor )
{
	if( !AssignZero() )
		return false;

	if( !Accumulate( pseudoVersor ) )
		return false;

	return true;
}

//=========================================================================================
bool SumOfPseudoVersors::AssignSumOfBlades( const SumOfBlades& sumOfBlades )
{
	if( !AssignZero() )
		return false;

	SumOfPseudoVersors sumOfPseudoVersors;
	for( const Blade* blade = ( const Blade* )sumOfBlades.sum.LeftMost(); blade; blade = ( const Blade* )blade->Right() )
	{
		if( !sumOfPseudoVersors.AssignBlade( *blade ) )
			return false;

		if( !Accumulate( sumOfPseudoVersors ) )
			return false;
	}

	return true;
}

//=========================================================================================
bool SumOfPseudoVersors::AssignSumOfVersors( const SumOfPseudoVersors& sumOfPseudoVersors )
{
	if( !AssignZero() )
		return false;

	if( !Accumulate( sumOfPseudoVersors ) )
		return false;

	return true;
}

//=========================================================================================
bool SumOfPseudoVersors::AssignSum( const SumOfPseudoVersors& left, const SumOfPseudoVersors& right )
{
	if( !AssignZero() )
		return false;

	if( !Accumulate( left ) )
		return false;

	if( !Accumulate( right ) )
		return false;

	return true;
}

//=========================================================================================
bool SumOfPseudoVersors::AssignDifference( const SumOfPseudoVersors& left, const SumOfPseudoVersors& right )
{
	if( !AssignZero() )
		return false;

	if( !Accumulate( left ) )
		return false;

	SumOfPseudoVersors sumOfPseudoVersors;
	if( !sumOfPseudoVersors.AssignSumOfVersors( right ) )
		return false;

	if( !sumOfPseudoVersors.Scale( -1.0 ) )
		return false;

	if( !Accumulate( sumOfPseudoVersors ) )
		return false;

	return true;
}

//=========================================================================================
bool SumOfPseudoVersors::AssignOuterProduct( const SumOfPseudoVersors& left, const SumOfPseudoVersors& right )
{
	SumOfBlades leftSumOfBlades, rightSumOfBlades;

	if( !leftSumOfBlades.AssignSumOfVersors( left ) )
		return false;

	if( !rightSumOfBlades.AssignSumOfVersors( right ) )
		return false;

	SumOfBlades outerProduct;
	if( !outerProduct.AssignOuterProduct( leftSumOfBlades, rightSumOfBlades ) )
		return false;

	if( !AssignSumOfBlades( outerProduct ) )
		return false;

	return true;
}

//=========================================================================================
bool SumOfPseudoVersors::AssignInnerProduct( const SumOfPseudoVersors& left, const SumOfPseudoVersors& right )
{
	SumOfBlades leftSumOfBlades, rightSumOfBlades;

	if( !leftSumOfBlades.AssignSumOfVersors( left ) )
		return false;

	if( !rightSumOfBlades.AssignSumOfVersors( right ) )
		return false;

	SumOfBlades innerProduct;
	if( !innerProduct.AssignInnerProduct( leftSumOfBlades, rightSumOfBlades ) )
		return false;

	if( !AssignSumOfBlades( innerProduct ) )
		return false;

	return true;
}

//=========================================================================================
bool SumOfPseudoVersors::AssignGeometricProduct( const Vector& left, const SumOfPseudoVersors& right )
{
	PseudoVersor pseudoVersor;
	if( !pseudoVersor.AssignVector( left, 1.0 ) )
		return false;

	SumOfPseudoVersors sumOfPseudoVersors;
	if( !sumOfPseudoVersors.AssignVersor( pseudoVersor ) )
		return false;

	if( !AssignGeometricProduct( sumOfPseudoVersors, right ) )
		return false;

	return true;
}

//=========================================================================================
bool SumOfPseudoVersors::AssignGeometricProduct( const SumOfPseudoVersors& left, const SumOfPseudoVersors& right )
{
	if( !AssignZero() )
		return false;

	PseudoVersor pseudoVersor;
	for( const PseudoVersor* leftVersor = ( const PseudoVersor* )left.sum.LeftMost(); leftVersor; leftVersor = ( const PseudoVersor* )leftVersor->Right() )
	{
		for( const PseudoVersor* rightVersor = ( const PseudoVersor* )right.sum.LeftMost(); rightVersor; rightVersor = ( const PseudoVersor* )rightVersor->Right() )
		{
			if( !pseudoVersor.AssignGeometricProduct( *leftVersor, *rightVersor ) )
				return false;

			if( !Accumulate( pseudoVersor ) )
				return false;
		}
	}

	return true;
}

//=========================================================================================
bool SumOfPseudoVersors::Scale( const Scalar& scalar )
{
	for( PseudoVersor* pseudoVersor = ( PseudoVersor* )sum.LeftMost(); pseudoVersor; pseudoVersor = ( PseudoVersor* )pseudoVersor->Right() )
		if( !pseudoVersor->Scale( scalar ) )
			return false;
	return true;
}

//=========================================================================================
bool SumOfPseudoVersors::Reverse( void )
{
	for( PseudoVersor* pseudoVersor = ( PseudoVersor* )sum.LeftMost(); pseudoVersor; pseudoVersor = ( PseudoVersor* )pseudoVersor->Right() )
		if( !pseudoVersor->Reverse() )
			return false;
	return true;
}

//=========================================================================================
bool SumOfPseudoVersors::Accumulate( const PseudoVersor& pseudoVersor )
{
	if( pseudoVersor.scalar == 0.0 )
		return true;

	PseudoVersor* likeTerm = FindLikeTerm( pseudoVersor );
	if( !likeTerm )
		sum.InsertSorted( ( PseudoVersor* )pseudoVersor.MakeCopy(), Utilities::List::SORT_ORDER_ASCENDING );
	else
	{
		likeTerm->scalar = likeTerm->scalar + pseudoVersor.scalar;
		if( likeTerm->scalar == 0.0 )
			sum.Remove( likeTerm, true );
	}
	return true;
}

//=========================================================================================
bool SumOfPseudoVersors::Accumulate( const SumOfPseudoVersors& sumOfPseudoVersors )
{
	for( const PseudoVersor* pseudoVersor = ( const PseudoVersor* )sumOfPseudoVersors.sum.LeftMost(); pseudoVersor; pseudoVersor = ( const PseudoVersor* )pseudoVersor->Right() )
		if( !Accumulate( *pseudoVersor ) )
			return false;

	return true;
}

//=========================================================================================
PseudoVersor* SumOfPseudoVersors::FindLikeTerm( const PseudoVersor& pseudoVersor )
{
	PseudoVersor* foundTerm = 0;
	for( PseudoVersor* term = ( PseudoVersor* )sum.LeftMost(); term; term = ( PseudoVersor* )term->Right() )
		if( term->IsLikeTerm( pseudoVersor ) )
			foundTerm = term;
	return foundTerm;
}

// SumOfPseudoVersors.cpp