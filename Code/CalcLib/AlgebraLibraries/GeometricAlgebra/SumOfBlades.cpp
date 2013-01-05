// SumOfBlades.cpp

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
SumOfBlades::SumOfBlades( void )
{
}

//=========================================================================================
/*virtual*/ SumOfBlades::~SumOfBlades( void )
{
	sum.RemoveAll( true );
}

//=========================================================================================
bool SumOfBlades::Print( char* printBuffer, int printBufferSize, ScalarAlgebra::PrintPurpose printPurpose ) const
{
	if( sum.Count() == 0 )
		strcpy_s( printBuffer, printBufferSize, "0" );
	else
	{
		printBuffer[0] = '\0';
		for( Blade* blade = ( Blade* )sum.LeftMost(); blade; blade = ( Blade* )blade->Right() )
		{
			char bladeString[ PRINT_BUFFER_SIZE_LARGE ];
			if( !blade->Print( bladeString, sizeof( bladeString ), printPurpose ) )
				return false;

			strcat_s( printBuffer, printBufferSize, bladeString );
			if( blade->Right() )
				strcat_s( printBuffer, printBufferSize, " + " );
		}
	}

	return true;
}

//=========================================================================================
bool SumOfBlades::AssignZero( void )
{
	sum.RemoveAll( true );
	return true;
}

//=========================================================================================
bool SumOfBlades::AssignScalar( const Scalar& scalar )
{
	sum.RemoveAll( true );
	Blade* blade = new Blade();
	sum.InsertLeftOf( 0, blade );
	return blade->AssignScalar( scalar );
}

//=========================================================================================
bool SumOfBlades::AssignScalar( const char* scalar )
{
	sum.RemoveAll( true );
	Blade* blade = new Blade();
	sum.InsertLeftOf( 0, blade );
	return blade->AssignScalar( scalar );
}

//=========================================================================================
bool SumOfBlades::AssignScalarFrom( double scalar )
{
	sum.RemoveAll( true );
	Blade* blade = new Blade();
	sum.InsertLeftOf( 0, blade );
	return blade->AssignScalar( Scalar( scalar ) );
}

//=========================================================================================
bool SumOfBlades::AssignScalarTo( double& scalar ) const
{
	if( sum.Count() > 1 )
		return false;

	scalar = 0.0;
	if( sum.Count() == 1 )
	{
		const Blade* blade = ( const Blade* )sum.LeftMost();
		if( blade->Grade() != 0 )
			return false;
		Scalar bladeScalarPart;
		if( !blade->AssignScalarPartTo( bladeScalarPart ) )
			return false;
		scalar = bladeScalarPart;	// The overload hides potential errors.
	}

	return true;
}

//=========================================================================================
bool SumOfBlades::AssignVector( const Vector& vector )
{
	sum.RemoveAll( true );
	Blade* blade = new Blade();
	sum.InsertLeftOf( 0, blade );
	return blade->AssignVector( vector, 1.0 );
}

//=========================================================================================
bool SumOfBlades::AssignBlade( const Blade& blade )
{
	if( !AssignZero() )
		return false;

	if( !Accumulate( blade ) )
		return false;

	return true;
}

//=========================================================================================
bool SumOfBlades::AssignVersor( const PseudoVersor& pseudoVersor )
{
	sum.RemoveAll( true );

	if( pseudoVersor.Grade() <= 1 )
	{	
		Blade* blade = new Blade();
		sum.InsertLeftOf( 0, blade );
		const Vector* vector = ( const Vector* )pseudoVersor.product.LeftMost();
		if( vector )
		{
			if( !blade->AssignVector( *vector, pseudoVersor.scalar ) )
				return false;
		}
		else
		{
			if( !blade->AssignScalar( pseudoVersor.scalar ) )
				return false;
		}
		return true;
	}
	
	PseudoVersor subVersor;
	if( !subVersor.AssignVersor( pseudoVersor ) )
		return false;

	Vector* vector = ( Vector* )subVersor.product.LeftMost();
	subVersor.product.Remove( vector, false );
	Utilities::ScopeDelete< Vector > scopeDelete( vector );

	SumOfBlades sumOfBlades;
	if( !sumOfBlades.AssignVersor( subVersor ) )
		return false;

	SumOfBlades innerProduct;
	if( !innerProduct.AssignInnerProduct( *vector, sumOfBlades ) )
		return false;

	// We have now accounted for the grade zero part in accumulating the inner product.
	// Don't account for it again when we accumulate the outer product.
	if( !sumOfBlades.RemoveGrade(0) )
		return false;

	if( !Accumulate( innerProduct ) )
		return false;

	SumOfBlades outerProduct;
	if( !outerProduct.AssignOuterProduct( *vector, sumOfBlades ) )
		return false;

	if( !Accumulate( outerProduct ) )
		return false;

	return true;
}

//=========================================================================================
bool SumOfBlades::AssignSumOfBlades( const SumOfBlades& sumOfBlades )
{
	if( !AssignZero() )
		return false;

	if( !Accumulate( sumOfBlades ) )
		return false;

	return true;
}

//=========================================================================================
bool SumOfBlades::AssignSumOfVersors( const SumOfPseudoVersors& sumOfPseudoVersors )
{
	if( !AssignZero() )
		return false;

	SumOfBlades sumOfBlades;
	for( const PseudoVersor* pseudoVersor = ( const PseudoVersor* )sumOfPseudoVersors.sum.LeftMost(); pseudoVersor; pseudoVersor = ( const PseudoVersor* )pseudoVersor->Right() )
	{
		if( !sumOfBlades.AssignVersor( *pseudoVersor ) )
			return false;

		if( !Accumulate( sumOfBlades ) )
			return false;
	}

	return true;
}

//=========================================================================================
bool SumOfBlades::AssignDerivative( const SumOfBlades& sumOfBlades, const char* variableName )
{
	if( !AssignZero() )
		return false;
	
	Blade derivative;
	for( const Blade* blade = ( const Blade* )sumOfBlades.sum.LeftMost(); blade; blade = ( const Blade* )blade->Right() )
	{
		if( !derivative.AssignDerivative( *blade, variableName ) )
			return false;

		if( !Accumulate( derivative ) )
			return false;
	}

	return true;
}

//=========================================================================================
bool SumOfBlades::AssignAntiDerivative( const SumOfBlades& sumOfBlades, const char* variableName )
{
	if( !AssignZero() )
		return false;
	
	Blade derivative;
	for( const Blade* blade = ( const Blade* )sumOfBlades.sum.LeftMost(); blade; blade = ( const Blade* )blade->Right() )
	{
		if( !derivative.AssignAntiDerivative( *blade, variableName ) )
			return false;

		if( !Accumulate( derivative ) )
			return false;
	}

	return true;
}

//=========================================================================================
bool SumOfBlades::AssignSum( const SumOfBlades& left, const SumOfBlades& right )
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
bool SumOfBlades::AssignDifference( const SumOfBlades& left, const SumOfBlades& right )
{
	if( !AssignZero() )
		return false;

	if( !Accumulate( left ) )
		return false;

	SumOfBlades sumOfBlades;
	if( !sumOfBlades.AssignSumOfBlades( right ) )
		return false;

	if( !sumOfBlades.Scale( -1.0 ) )
		return false;

	if( !Accumulate( sumOfBlades ) )
		return false;

	return true;
}

//=========================================================================================
bool SumOfBlades::AssignOuterProduct( const Vector& left, const SumOfBlades& right )
{
	if( !AssignZero() )
		return false;

	Blade leftBlade;
	if( !leftBlade.AssignVector( left, 1.0 ) )
		return false;

	Blade blade;
	for( const Blade* rightBlade = ( const Blade* )right.sum.LeftMost(); rightBlade; rightBlade = ( const Blade* )rightBlade->Right() )
	{
		if( !blade.AssignOuterProduct( leftBlade, *rightBlade ) )
			return false;

		if( !Accumulate( blade ) )
			return false;
	}

	return true;
}

//=========================================================================================
bool SumOfBlades::AssignOuterProduct( const SumOfBlades& left, const SumOfBlades& right )
{
	if( !AssignZero() )
		return false;

	Blade blade;
	for( const Blade* leftBlade = ( const Blade* )left.sum.LeftMost(); leftBlade; leftBlade = ( const Blade* )leftBlade->Right() )
	{
		for( const Blade* rightBlade = ( const Blade* )right.sum.LeftMost(); rightBlade; rightBlade = ( const Blade* )rightBlade->Right() )
		{
			if( !blade.AssignOuterProduct( *leftBlade, *rightBlade ) )
				return false;

			if( !Accumulate( blade ) )
				return false;
		}
	}

	return true;
}

//=========================================================================================
bool SumOfBlades::AssignInnerProduct( const Vector& left, const Blade& right )
{
	if( !AssignZero() )
		return false;

	Blade blade;
	if( right.Grade() == 0 )
	{
		if( !blade.AssignVector( left, right.scalar ) )
			return false;

		if( !Accumulate( blade ) )
			return false;
	}
	else
	{
		int index = 0;
		for( const Vector* rightVector = ( const Vector* )right.product.LeftMost(); rightVector; rightVector = ( const Vector* )rightVector->Right() )
		{
			if( !blade.AssignBlade( right ) )
				return false;

			if( !blade.RemoveVector( rightVector->Name() ) )
				return false;

			if( index++ % 2 == 1 && !blade.Scale( -1.0 ) )
				return false;

			blade.scalar = blade.scalar * left.InnerProduct( *rightVector );
			if( !Accumulate( blade ) )
				return false;
		}
	}

	return true;
}

//=========================================================================================
bool SumOfBlades::AssignInnerProduct( const Blade& left, const Vector& right )
{
	if( !AssignInnerProduct( right, left ) )
		return false;

	if( left.Grade() % 2 == 0 && !Scale( -1.0 ) )
		return false;

	return true;
}

//=========================================================================================
bool SumOfBlades::AssignInnerProduct( const Vector& left, const SumOfBlades& right )
{
	if( !AssignZero() )
		return false;

	SumOfBlades sumOfBlades;
	for( const Blade* rightBlade = ( const Blade* )right.sum.LeftMost(); rightBlade; rightBlade = ( const Blade* )rightBlade->Right() )
	{
		if( !sumOfBlades.AssignInnerProduct( left, *rightBlade ) )
			return false;

		if( !Accumulate( sumOfBlades ) )
			return false;
	}

	return true;
}

//=========================================================================================
bool SumOfBlades::AssignInnerProduct( const SumOfBlades& left, const Vector& right )
{
	if( !AssignZero() )
		return false;

	SumOfBlades sumOfBlades;
	for( const Blade* leftBlade = ( const Blade* )left.sum.LeftMost(); leftBlade; leftBlade = ( const Blade* )leftBlade->Right() )
	{
		if( !sumOfBlades.AssignInnerProduct( *leftBlade, right ) )
			return false;

		if( !Accumulate( sumOfBlades ) )
			return false;
	}

	return true;
}

//=========================================================================================
bool SumOfBlades::AssignInnerProduct( const Blade& left, const Blade& right )
{
	int index = 0;
	int otherIndex = 1;
	SumOfBlades sumOfBlades[2];

	if( left.Grade() <= right.Grade() )
	{
		if( left.Grade() == 0 )
		{
			if( !sumOfBlades[ index ].AssignBlade( right ) )
				return false;
		}
		else
		{
			const Vector* vector = ( const Vector* )left.product.RightMost();
			if( !sumOfBlades[ index ].AssignInnerProduct( *vector, right ) )
				return false;
			
			vector = ( const Vector* )vector->Left();
			while( vector )
			{
				if( !sumOfBlades[ otherIndex ].AssignInnerProduct( *vector, sumOfBlades[ index ] ) )
					return false;

				vector = ( const Vector* )vector->Left();
				++index %= 2;
				++otherIndex %= 2;
			}
		}
	}
	else
	{
		if( right.Grade() == 0 )
		{
			if( !sumOfBlades[ index ].AssignBlade( left ) )
				return false;
		}
		else
		{
			const Vector* vector = ( const Vector* )right.product.LeftMost();
			if( !sumOfBlades[ index ].AssignInnerProduct( left, *vector ) )
				return false;

			vector = ( const Vector* )vector->Right();
			while( vector )
			{
				if( !sumOfBlades[ otherIndex ].AssignInnerProduct( sumOfBlades[ index ], *vector ) )
					return false;

				vector = ( const Vector* )vector->Right();
				++index %= 2;
				++otherIndex %= 2;
			}
		}
	}

	if( !AssignSumOfBlades( sumOfBlades[ index ] ) )
		return false;

	if( left.Grade() <= right.Grade() )
	{
		if( !Scale( left.scalar ) )
			return false;
	}
	else
	{
		if( !Scale( right.scalar ) )
			return false;
	}

	return true;
}

//=========================================================================================
bool SumOfBlades::AssignInnerProduct( const SumOfBlades& left, const SumOfBlades& right )
{
	if( !AssignZero() )
		return false;

	SumOfBlades sumOfBlades;
	for( const Blade* leftBlade = ( const Blade* )left.sum.LeftMost(); leftBlade; leftBlade = ( const Blade* )leftBlade->Right() )
	{
		for( const Blade* rightBlade = ( const Blade* )right.sum.LeftMost(); rightBlade; rightBlade = ( const Blade* )rightBlade->Right() )
		{
			if( !sumOfBlades.AssignInnerProduct( *leftBlade, *rightBlade ) )
				return false;

			if( !Accumulate( sumOfBlades ) )
				return false;
		}
	}

	return true;
}

//=========================================================================================
bool SumOfBlades::AssignGeometricProduct( const SumOfBlades& left, const SumOfBlades& right )
{
	SumOfPseudoVersors leftSumOfVersors, rightSumOfVersors;

	if( !leftSumOfVersors.AssignSumOfBlades( left ) )
		return false;

	if( !rightSumOfVersors.AssignSumOfBlades( right ) )
		return false;

	SumOfPseudoVersors geometricProduct;
	if( !geometricProduct.AssignGeometricProduct( leftSumOfVersors, rightSumOfVersors ) )
		return false;

	if( !AssignSumOfVersors( geometricProduct ) )
		return false;

	return true;
}

//=========================================================================================
bool SumOfBlades::AssignGeometricProduct( const Blade& left, const Blade& right )
{
	SumOfPseudoVersors leftSumOfVersors, rightSumOfVersors;

	if( !leftSumOfVersors.AssignBlade( left ) )
		return false;

	if( !rightSumOfVersors.AssignBlade( right ) )
		return false;

	SumOfPseudoVersors geometricProduct;
	if( !geometricProduct.AssignGeometricProduct( leftSumOfVersors, rightSumOfVersors ) )
		return false;

	if( !AssignSumOfVersors( geometricProduct ) )
		return false;

	return true;
}

//=========================================================================================
// This algorithm computes the geometric square twice.  Is there a way that does it just once?
bool SumOfBlades::AssignGeometricInverse( const SumOfBlades& sumOfBlades, InverseType inverseType, InverseResult& inverseResult )
{
	// The given multivector is non-singular until proven otherwise.
	inverseResult = NONSINGULAR_MULTIVECTOR;

	// Build a mapping from basis blades to matrix indices.
	int rowCount = 0, colCount = sumOfBlades.sum.Count();
	Utilities::Map< int > indexMap;
	char bladeString[ PRINT_BUFFER_SIZE_SMALL ];
	SumOfBlades geometricProduct;
	for( const Blade* leftBlade = ( const Blade* )sumOfBlades.sum.LeftMost(); leftBlade; leftBlade = ( const Blade* )leftBlade->Right() )
	{
		for( const Blade* rightBlade = ( const Blade* )sumOfBlades.sum.LeftMost(); rightBlade; rightBlade = ( const Blade* )rightBlade->Right() )
		{
			if( !geometricProduct.AssignGeometricProduct( *leftBlade, *rightBlade ) )
				return false;

			for( Blade* blade = ( Blade* )geometricProduct.sum.LeftMost(); blade; blade = ( Blade* )blade->Right() )
			{
				if( !blade->Print( bladeString, sizeof( bladeString ), ScalarAlgebra::PRINT_FOR_SORTING ) )
					return false;
				if( !indexMap.Lookup( bladeString ) )
					if( !indexMap.Insert( bladeString, rowCount++ ) )
						return false;
			}
		}
	}

	// If we have an undertermined system, then there isn't enough information to solve it.
	// Sometimes the system will be overdetermined, but it should be consistent.
	if( rowCount < colCount )
		return false;

	// Declare matrices we'll use to solve a linear system.  These initialize to zero throughout.
	LinearAlgebra::Matrix coeficientMatrix( rowCount, colCount );
	LinearAlgebra::Matrix constantMatrix( rowCount, 1 );

	// Now go populate the coeficient and constant matrices.
	LinearAlgebra::Scalar scalar;
	int row = 0, col = 0;
	int index = 0;
	for( const Blade* leftBlade = ( const Blade* )sumOfBlades.sum.LeftMost(); leftBlade; leftBlade = ( const Blade* )leftBlade->Right() )
	{
		if( inverseType == RIGHT_INVERSE )
			col = 0;

		for( const Blade* rightBlade = ( const Blade* )sumOfBlades.sum.LeftMost(); rightBlade; rightBlade = ( const Blade* )rightBlade->Right() )
		{
			if( !geometricProduct.AssignGeometricProduct( *leftBlade, *rightBlade ) )
				return false;

			for( Blade* blade = ( Blade* )geometricProduct.sum.LeftMost(); blade; blade = ( Blade* )blade->Right() )
			{
				if( !blade->Print( bladeString, sizeof( bladeString ), ScalarAlgebra::PRINT_FOR_SORTING ) )
					return false;
				if( !indexMap.Lookup( bladeString, &index ) )
					return false;
				row = index;

				if( !coeficientMatrix.AccumulateScalarAt( row, col, blade->scalar ) )
					return false;
				
				// This may do some redundant setting, but that's okay.
				if( blade->Grade() > 0 )
					scalar = 0.0;
				else
					scalar = 1.0;
				if( !constantMatrix.AssignScalarFrom( row, 0, scalar ) )
					return false;
			}

			if( inverseType == RIGHT_INVERSE )
				col++;
		}

		if( inverseType == LEFT_INVERSE )
			col++;
	}

	// We don't need this anymore.
	indexMap.RemoveAll();

	// Try to solve the linear system.
	LinearAlgebra::Matrix::InverseResult matrixInverseResult;
	LinearAlgebra::Matrix inverseCoeficientMatrix;
	if( !inverseCoeficientMatrix.AssignInverse( coeficientMatrix, LinearAlgebra::Matrix::LEFT_INVERSE, matrixInverseResult ) )
	{
		if( matrixInverseResult == LinearAlgebra::Matrix::SINGULAR_MATRIX )
			inverseResult = SINGULAR_MULTIVECTOR;
		return false;
	}

	// Calculate the solutions to the system.
	LinearAlgebra::Matrix solutionMatrix;
	if( !solutionMatrix.AssignProduct( inverseCoeficientMatrix, constantMatrix ) )
		return false;

	// Finally, apply the solution to calculate the inverse.
	if( !AssignZero() )
		return false;
	row = 0;
	for( const Blade* blade = ( const Blade* )sumOfBlades.sum.LeftMost(); blade; blade = ( const Blade* )blade->Right() )
	{
		if( !solutionMatrix.AssignScalarTo( row++, 0, scalar ) )
			return false;
		Blade accumulationBlade;
		if( !accumulationBlade.AssignBlade( *blade ) )
			return false;
		if( !accumulationBlade.Scale( scalar ) )
			return false;
		if( !Accumulate( accumulationBlade ) )
			return false;
	}

	return true;
}

//=========================================================================================
bool SumOfBlades::AssignSquareMagnitudeTo( Scalar& squareMagnitude ) const
{
	squareMagnitude = 0.0;
	Scalar bladeSquareMagnitude;
	for( const Blade* blade = ( const Blade* )sum.LeftMost(); blade; blade = ( const Blade* )blade->Right() )
	{
		if( !blade->AssignSquareMagnitudeTo( bladeSquareMagnitude ) )
			return false;
		squareMagnitude = squareMagnitude + bladeSquareMagnitude;
	}
	return true;
}

//=========================================================================================
bool SumOfBlades::Scale( const Scalar& scalar )
{
	for( Blade* blade = ( Blade* )sum.LeftMost(); blade; blade = ( Blade* )blade->Right() )
		if( !blade->Scale( scalar ) )
			return false;

	return true;
}

//=========================================================================================
bool SumOfBlades::Reverse( void )
{
	SumOfPseudoVersors sumOfPseudoVersors;

	if( !sumOfPseudoVersors.AssignSumOfBlades( *this ) )
		return false;

	if( !sumOfPseudoVersors.Reverse() )
		return false;

	if( !AssignSumOfVersors( sumOfPseudoVersors ) )
		return false;

	return true;
}

//=========================================================================================
bool SumOfBlades::Bar( void )
{
	for( Blade* blade = ( Blade* )sum.LeftMost(); blade; blade = ( Blade* )blade->Right() )
		if( !blade->Bar() )
			return false;

	return true;
}

//=========================================================================================
bool SumOfBlades::Accumulate( const Blade& blade )
{
	if( blade.scalar == 0.0 )
		return true;

	Scalar relativeHandedness;
	Blade* likeTerm = FindLikeTerm( blade, relativeHandedness );
	if( !likeTerm )
		sum.InsertSorted( blade.MakeCopy(), Utilities::List::SORT_ORDER_ASCENDING );
	else
	{
		likeTerm->scalar = likeTerm->scalar + blade.scalar * relativeHandedness;
		if( likeTerm->scalar == 0.0 )
			sum.Remove( likeTerm, true );
	}
	return true;
}

//=========================================================================================
bool SumOfBlades::Accumulate( const SumOfBlades& sumOfBlades )
{
	for( const Blade* blade = ( const Blade* )sumOfBlades.sum.LeftMost(); blade; blade = ( const Blade* )blade->Right() )
		if( !Accumulate( *blade ) )
			return false;
	return true;
}

//=========================================================================================
bool SumOfBlades::IsHomogeneousOfGrade( int grade ) const
{
	for( const Blade* blade = ( const Blade* )sum.LeftMost(); blade; blade = ( const Blade* )blade->Right() )
		if( blade->Grade() != grade )
			return false;
	return true;
}

//=========================================================================================
bool SumOfBlades::RemoveGrade( int grade )
{
	Blade* blade = ( Blade* )sum.LeftMost();
	Blade* nextBlade = 0;
	while( blade )
	{
		nextBlade = ( Blade* )blade->Right();
		if( blade->Grade() == grade )
			sum.Remove( blade, true );
		blade = nextBlade;
	}
	return true;
}

//=========================================================================================
Blade* SumOfBlades::FindLikeTerm( const Blade& blade, Scalar& relativeHandedness ) const
{
	Blade* foundTerm = 0;
	for( Blade* term = ( Blade* )sum.LeftMost(); term && !foundTerm; term = ( Blade* )term->Right() )
		if( term->IsLikeTerm( blade, relativeHandedness ) )
			foundTerm = term;
	return foundTerm;
}

//=========================================================================================
int SumOfBlades::BladeCount( void ) const
{
	return sum.Count();
}

//=========================================================================================
bool SumOfBlades::AssignBladeTo( Blade& blade, int index ) const
{
	if( index < 0 || index >= sum.Count() )
		return false;
	return blade.AssignBlade( *( Blade* )sum[ index ] );
}

//=========================================================================================
bool SumOfBlades::ScalarPart( const Blade& blade, Scalar& scalar ) const
{
	scalar = 0.0;

	Scalar relativeHandedness;
	Blade* likeTerm = FindLikeTerm( blade, relativeHandedness );
	if( !likeTerm )
		return true;

	scalar = likeTerm->scalar * relativeHandedness / blade.scalar;
	return true;
}

//=========================================================================================
bool SumOfBlades::GradePart( int grade, SumOfBlades& homogeneousPart ) const
{
	if( !homogeneousPart.AssignZero() )
		return false;

	for( const Blade* blade = ( const Blade* )sum.LeftMost(); blade; blade = ( const Blade* )blade->Right() )
		if( blade->Grade() == grade )
			if( !homogeneousPart.Accumulate( *blade ) )
				return false;

	return true;
}

// SumOfBlades.cpp