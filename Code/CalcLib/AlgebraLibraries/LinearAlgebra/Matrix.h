// Matrix.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

//=========================================================================================
class LinearAlgebra::Matrix
{
public:

	Matrix( void );
	Matrix( int rows, int cols );
	virtual ~Matrix( void );

	// For square matrices, this may not matter.
	enum InverseType
	{
		LEFT_INVERSE,
		RIGHT_INVERSE,
	};

	enum InverseResult
	{
		SINGULAR_MATRIX,
		NONSINGULAR_MATRIX,
	};

	bool Print( char* printBuffer, int printBufferSize, ScalarAlgebra::PrintPurpose printPurpose ) const;
	bool PrintLine( int line, char* printBuffer, int printBufferSize ) const;
	int LinesNeededForPrinting( void ) const;
	int LinesForRow( int row ) const;

	bool AssignDimensions( int rows, int cols );
	bool AssignFrom( const Matrix& matrix );
	bool AssignTo( Matrix& matrix ) const;
	bool AssignZero( void );
	bool AssignIdentity( void );
	bool AssignScalarFrom( int row, int col, const Scalar& scalar );
	bool AssignScalarTo( int row, int col, Scalar& scalar ) const;
	bool AssignMatrixFrom( int row, int col, const Matrix& matrix );
	bool AssignMatrixTo( int row, int col, Matrix& matrix ) const;
	bool AssignElementFrom( int row, int col, const Element& element );
	bool AssignElementTo( int row, int col, Element& element ) const;
	bool AssignSum( const Matrix& left, const Matrix& right );
	bool AssignProduct( const Matrix& left, const Matrix& right );
	bool AssignInverse( const Matrix& matrix, InverseType inverseType, InverseResult& inverseResult );
	bool AssignAdjugate( const Matrix& matrix );
	bool AssignDeterminantTo( Scalar& determinant ) const;
	bool AssignSubMatrix( int row, int col, const Matrix& matrix );
	bool AssignCofactorTo( int row, int col, Scalar& cofactor ) const;

	bool AccumulateScalarAt( int row, int col, const Scalar& scalar );
	bool Accumulate( const Matrix& matrix );
	bool Scale( const Scalar& scalar );

private:

	bool CheckBounds( int row, int col ) const;
	bool AllocateMemory( int rows, int cols );
	bool ReallocateMemory( int rows, int cols );
	bool DeallocateMemory( void );

	Element** table;
	int rows, cols;
};

// Matrix.h