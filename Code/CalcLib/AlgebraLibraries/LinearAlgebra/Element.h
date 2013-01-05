// Element.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

//=========================================================================================
class LinearAlgebra::Element
{
public:

	enum Type
	{
		UNKNOWN_ELEMENT_TYPE,
		SCALAR_ELEMENT_TYPE,
		MATRIX_ELEMENT_TYPE,
		//MULTIVECTOR_ELEMENT_TYPE,		// It might be useful to also store multivectors in a matrix.
	};

	Element( void );
	virtual ~Element( void );

	bool Print( char* printBuffer, int printBufferSize, ScalarAlgebra::PrintPurpose printPurpose ) const;
	bool PrintLine( int line, char* printBuffer, int printBufferSize ) const;
	int LinesNeededForPrinting( void ) const;

	bool AssignIdentity( void );
	bool AssignScalar( double scalar );
	bool AssignScalarFrom( const Scalar& scalar );
	bool AssignScalarTo( Scalar& scalar ) const;
	bool AssignMatrixFrom( const Matrix& matrix );
	bool AssignMatrixTo( Matrix& matrix ) const;
	bool AssignFrom( const Element& element );
	bool AssignTo( Element& element ) const;
	bool AssignSum( const Element& left, const Element& right );
	bool AssignProduct( const Element& left, const Element& right );
	bool AssignInverse( const Element& element );

	bool Accumulate( const Scalar& scalar );
	bool Accumulate( const Element& element );
	bool Scale( const Scalar& scalar );

	Type ElementType( void ) const;
	Matrix* AccessMatrix( void );
	Scalar* AccessScalar( void );
	const Matrix* AccessMatrix( void ) const;
	const Scalar* AccessScalar( void ) const;

private:

	bool AllocateMemory( Type type );
	bool ReallocateMemory( Type type );
	bool DeallocateMemory( void );

	Type type;
	union
	{
		void* pointer;
		Scalar* scalar;
		Matrix* matrix;
	};
};

// Element.h