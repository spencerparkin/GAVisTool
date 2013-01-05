// Matrix.cpp

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "LinearAlgebra.h"

using namespace LinearAlgebra;

//=========================================================================================
Matrix::Matrix( void )
{
	table = 0;
	rows = 0;
	cols = 0;
}

// m = mtx(3,4)
// mtx_set(m,0,0,4)
// mtx_set_row(m,0,$a+3,3*8,mtx(2,3))
// mtx_set_col(m,2,-1,-2,-3,-4)
// mtx_get_row(m,2) -> 1x4 matrix

//=========================================================================================
Matrix::Matrix( int rows, int cols )
{
	table = 0;
	this->rows = 0;
	this->cols = 0;

	AssignDimensions( rows, cols );
}

//=========================================================================================
/*virtual*/ Matrix::~Matrix( void )
{
	DeallocateMemory();
}

//=========================================================================================
bool Matrix::Print( char* printBuffer, int printBufferSize, ScalarAlgebra::PrintPurpose printPurpose ) const
{
	printBuffer[0] = '\0';

	int lineCount = LinesNeededForPrinting();
	for( int line = 0; line < lineCount; line++ )
	{
		char lineBuffer[ PRINT_BUFFER_SIZE_LARGE ];
		if( !PrintLine( line, lineBuffer, sizeof( lineBuffer ) ) )
			return false;

		strcat_s( printBuffer, printBufferSize, lineBuffer );
		strcat_s( printBuffer, printBufferSize, "\n" );
	}

	return true;
}

//=========================================================================================
bool Matrix::PrintLine( int line, char* printBuffer, int printBufferSize ) const
{
	printBuffer[0] = '\0';

	int row = 0;
	while( true )
	{
		int linesForRow = LinesForRow( row );
		if( line >= linesForRow )
		{
			line -= linesForRow;
			row++;
		}
		else
			break;
	}

	// The columns won't be lined up at all, but I'm not concerned about that.
	// Latex formatted output will be much nicer when I can get that working.
	strcat_s( printBuffer, printBufferSize, "[ " );
	for( int col = 0; col < cols; col++ )
	{
		char lineBuffer[ PRINT_BUFFER_SIZE_LARGE ];
		if( !table[ row ][ col ].PrintLine( line, lineBuffer, sizeof( lineBuffer ) ) )
			return false;

		strcat_s( printBuffer, printBufferSize, lineBuffer );
		if( col + 1 < cols )
			strcat_s( printBuffer, printBufferSize, "  " );
	}
	strcat_s( printBuffer, printBufferSize, " ]" );

	return true;
}

//=========================================================================================
int Matrix::LinesNeededForPrinting( void ) const
{
	int lineCount = 0;
	if( table )
		for( int row = 0; row < rows; row++ )
			lineCount += LinesForRow( row );
	return lineCount;
}

//=========================================================================================
int Matrix::LinesForRow( int row ) const
{
	int maxLines = 0;
	for( int col = 0; col < cols; col++ )
	{
		int elementLineCount = table[ row ][ col ].LinesNeededForPrinting();
		if( elementLineCount > maxLines )
			maxLines = elementLineCount;
	}
	return maxLines;
}

//=========================================================================================
bool Matrix::AssignDimensions( int rows, int cols )
{
	if( !ReallocateMemory( rows, cols ) )
		return false;

	if( !AssignZero() )
		return false;

	return true;
}

//=========================================================================================
bool Matrix::AssignFrom( const Matrix& matrix )
{
	if( this == &matrix )
		return true;

	if( !ReallocateMemory( matrix.rows, matrix.cols ) )
		return false;

	for( int row = 0; row < rows; row++ )
		for( int col = 0; col < cols; col++ )
			if( !table[ row ][ col ].AssignFrom( matrix.table[ row ][ col ] ) )
				return false;

	return true;
}

//=========================================================================================
bool Matrix::AssignTo( Matrix& matrix ) const
{
	if( !matrix.AssignFrom( *this ) )
		return false;

	return true;
}

//=========================================================================================
bool Matrix::AssignZero( void )
{
	if( !table )
		return false;

	for( int row = 0; row < rows; row++ )
		for( int col = 0; col < cols; col++ )
			if( !table[ row ][ col ].AssignScalar( 0.0 ) )
				return false;

	return true;
}

//=========================================================================================
bool Matrix::AssignIdentity( void )
{
	if( !table )
		return false;

	if( rows != cols )
		return false;

	for( int row = 0; row < rows; row++ )
	{
		for( int col = 0; col < cols; cols++ )
		{
			if( row == col )
				table[ row ][ col ].AssignScalar( 1.0 );
			else
				table[ row ][ col ].AssignScalar( 0.0 );
		}
	}
	return true;
}

//=========================================================================================
bool Matrix::AssignScalarFrom( int row, int col, const Scalar& scalar )
{
	if( !table )
		return false;

	if( !CheckBounds( row, col ) )
		return false;

	if( !table[ row ][ col ].AssignScalarFrom( scalar ) )
		return false;

	return true;
}

//=========================================================================================
bool Matrix::AssignScalarTo( int row, int col, Scalar& scalar ) const
{
	if( !table )
		return false;

	if( !CheckBounds( row, col ) )
		return false;

	if( !table[ row ][ col ].AssignScalarTo( scalar ) )
		return false;

	return true;
}

//=========================================================================================
bool Matrix::AssignMatrixFrom( int row, int col, const Matrix& matrix )
{
	if( this == &matrix )
		return false;

	if( !table )
		return false;

	if( !CheckBounds( row, col ) )
		return false;

	if( !table[ row ][ col ].AssignMatrixFrom( matrix ) )
		return false;

	return true;
}

//=========================================================================================
bool Matrix::AssignMatrixTo( int row, int col, Matrix& matrix ) const
{
	if( !table )
		return false;

	if( !CheckBounds( row, col ) )
		return false;

	if( !table[ row ][ col ].AssignMatrixTo( matrix ) )
		return false;

	return true;
}

//=========================================================================================
bool Matrix::AssignElementFrom( int row, int col, const Element& element )
{
	if( !table )
		return false;

	if( !CheckBounds( row, col ) )
		return false;

	if( !table[ row ][ col ].AssignFrom( element ) )
		return false;

	return true;
}

//=========================================================================================
bool Matrix::AssignElementTo( int row, int col, Element& element ) const
{
	if( !table )
		return false;

	if( !CheckBounds( row, col ) )
		return false;

	if( !table[ row ][ col ].AssignTo( element ) )
		return false;

	return true;
}

//=========================================================================================
bool Matrix::AssignSum( const Matrix& left, const Matrix& right )
{
	if( !left.table || !right.table )
		return false;

	if( this == &left || this == &right )
		return false;

	if( left.rows != right.rows || left.cols != right.cols )
		return false;

	if( !( rows == left.rows && cols == right.cols ) || !ReallocateMemory( left.rows, left.cols ) )
		return false;

	for( int row = 0; row < rows; row++ )
		for( int col = 0; col < cols; col++ )
			if( !table[ row ][ col ].AssignSum( left.table[ row ][ col ], right.table[ row ][ col ] ) )
				return false;

	return true;
}

//=========================================================================================
bool Matrix::AssignProduct( const Matrix& left, const Matrix& right )
{
	if( !left.table || !right.table )
		return false;

	if( this == &left || this == &right )
		return false;

	if( left.cols != right.rows )
		return false;

	if( !( rows == left.rows && cols == right.cols ) && !ReallocateMemory( left.rows, right.cols ) )
		return false;

	Element element;
	for( int row = 0; row < rows; row++ )
	{
		for( int col = 0; col < cols; col++ )
		{
			table[ row ][ col ].AssignScalar( 0.0 );
			for( int index = 0; index < left.cols; index++ )
			{
				if( !element.AssignProduct( left.table[ row ][ index ], right.table[ index ][ col ] ) )
					return false;

				if( !table[ row ][ col ].Accumulate( element ) )
					return false;
			}
		}
	}

	return true;
}

//=========================================================================================
bool Matrix::AssignInverse( const Matrix& matrix, InverseType inverseType, InverseResult& inverseResult )
{
	// TODO: Calculate left and right inverse matrices for non-square matrices.

	inverseResult = NONSINGULAR_MATRIX;

	Scalar scalar;
	if( !matrix.AssignDeterminantTo( scalar ) )
		return false;

	if( scalar == 0.0 )
	{
		inverseResult = SINGULAR_MATRIX;
		return false;
	}

	if( !scalar.Invert() )
		return false;

	if( matrix.rows == 1 && matrix.cols == 1 )
	{
		if( !AssignDimensions( 1, 1 ) )
			return false;

		if( !AssignScalarFrom( 0, 0, scalar ) )
			return false;
	}
	else
	{
		if( !AssignAdjugate( matrix ) )
			return false;

		if( !Scale( scalar ) )
			return false;
	}

	return true;
}

//=========================================================================================
bool Matrix::AssignAdjugate( const Matrix& matrix )
{
	if( !ReallocateMemory( matrix.rows, matrix.cols ) )
		return false;

	Scalar cofactor;
	for( int row = 0; row < rows; row++ )
	{
		for( int col = 0; col < cols; col++ )
		{
			// Notice that we index first by column, then by row.
			if( !matrix.AssignCofactorTo( col, row, cofactor ) )
				return false;

			if( !AssignScalarFrom( row, col, cofactor ) )
				return false;
		}
	}

	return true;
}

//=========================================================================================
// Here we just do a cofactor expansion along the top row.
bool Matrix::AssignDeterminantTo( Scalar& determinant ) const
{
	if( rows != cols )
		return false;

	if( rows == 1 && cols == 1 )
	{
		if( !table[0][0].AssignScalarTo( determinant ) )
			return false;
		return true;
	}

	determinant = 0.0;
	Scalar cofactor, scalar;
	int row = 0;
	for( int col = 0; col < cols; col++ )
	{
		if( !AssignCofactorTo( row, col, cofactor ) )
			return false;

		if( !AssignScalarTo( row, col, scalar ) )
			return false;

		determinant = determinant + scalar * cofactor;
	}

	return true;
}

//=========================================================================================
bool Matrix::AssignSubMatrix( int row, int col, const Matrix& matrix )
{
	if( !matrix.CheckBounds( row, col ) )
		return false;

	if( matrix.rows == 1 || matrix.cols == 1 )
		return false;

	if( !ReallocateMemory( matrix.rows - 1, matrix.cols - 1 ) )
		return false;

	int thisRow = 0;
	for( int givenRow = 0; givenRow < matrix.rows; givenRow++ )
	{
		if( givenRow != row )
		{
			int thisCol = 0;
			for( int givenCol = 0; givenCol < matrix.cols; givenCol++ )
			{
				if( givenCol != col )
				{
					if( !table[ thisRow ][ thisCol ].AssignFrom( matrix.table[ givenRow ][ givenCol ] ) )
						return false;

					thisCol++;
				}
			}

			thisRow++;
		}
	}

	return true;
}

//=========================================================================================
bool Matrix::AssignCofactorTo( int row, int col, Scalar& cofactor ) const
{
	Matrix subMatrix;
	if( !subMatrix.AssignSubMatrix( row, col, *this ) )
		return false;

	if( !subMatrix.AssignDeterminantTo( cofactor ) )
		return false;

	if( ( row + col ) % 2 == 1 )
		cofactor = cofactor * -1.0;

	return true;
}

//=========================================================================================
bool Matrix::AccumulateScalarAt( int row, int col, const Scalar& scalar )
{
	if( !table )
		return false;

	if( !CheckBounds( row, col ) )
		return false;

	if( !table[ row ][ col ].Accumulate( scalar ) )
		return false;

	return true;
}

//=========================================================================================
bool Matrix::Accumulate( const Matrix& matrix )
{
	if( !table )
		return false;

	if( rows != matrix.rows || cols != matrix.cols )
		return false;

	for( int row = 0; row < rows; row++ )
		for( int col = 0; col < cols; col++ )
			if( !table[ row ][ col ].Accumulate( matrix.table[ row ][ col ] ) )
				return false;

	return true;
}

//=========================================================================================
bool Matrix::Scale( const Scalar& scalar )
{
	if( !table )
		return false;

	for( int row = 0; row < rows; row++ )
		for( int col = 0; col < cols; col++ )
			table[ row ][ col ].Scale( scalar );

	return true;
}

//=========================================================================================
bool Matrix::AllocateMemory( int rows, int cols )
{
	if( table != 0 )
		return false;

	this->rows = rows;
	this->cols = cols;

	table = new Element*[ rows ];
	for( int row = 0; row < rows; row++ )
		table[ row ] = new Element[ cols ];
	
	if( !AssignZero() )
		return false;

	return true;
}

//=========================================================================================
bool Matrix::DeallocateMemory( void )
{
	if( table == 0 )
		return false;

	for( int row = 0; row < rows; row++ )
		delete[] table[ row ];
	delete[] table;

	table = 0;
	rows = 0;
	cols = 0;

	return true;
}

//=========================================================================================
bool Matrix::ReallocateMemory( int rows, int cols )
{
	// There is no need to re-allocate the matrix if it is already the desired dimensions.
	if( this->rows != rows || this->cols != cols )
	{
		if( table != 0 )
			if( !DeallocateMemory() )
				return false;
		
		if( !AllocateMemory( rows, cols ) )
			return false;
	}
	else
	{
		if( !AssignZero() )
			return false;
	}

	return true;
}

//=========================================================================================
bool Matrix::CheckBounds( int row, int col ) const
{
	if( row < 0 )
		return false;
	if( col < 0 )
		return false;
	if( row >= rows )
		return false;
	if( col >= cols )
		return false;
	return true;
}

// Matrix.cpp