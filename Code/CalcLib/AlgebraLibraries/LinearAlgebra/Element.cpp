// Element.cpp

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
Element::Element( void )
{
	type = UNKNOWN_ELEMENT_TYPE;
	pointer = 0;
}

//=========================================================================================
/*virtual*/ Element::~Element( void )
{
	if( pointer != 0 )
		DeallocateMemory();
}

//=========================================================================================
bool Element::Print( char* printBuffer, int printBufferSize, ScalarAlgebra::PrintPurpose printPurpose ) const
{
	if( type == SCALAR_ELEMENT_TYPE )
	{
		if( !scalar->Print( printBuffer, printBufferSize, printPurpose ) )
			return false;
	}
	else if( type == MATRIX_ELEMENT_TYPE )
	{
		if( !matrix->Print( printBuffer, printBufferSize, printPurpose ) )
			return false;
	}
	else
		return false;

	return true;
}

//=========================================================================================
bool Element::PrintLine( int line, char* printBuffer, int printBufferSize ) const
{
	if( type == SCALAR_ELEMENT_TYPE )
	{
		if( line <= 0 )
			if( !scalar->Print( printBuffer, printBufferSize, ScalarAlgebra::PRINT_FOR_READING ) )
				return false;
	}
	else if( type == MATRIX_ELEMENT_TYPE )
	{
		if( !matrix->PrintLine( line, printBuffer, printBufferSize ) )
			return false;
	}
	else
		return false;

	return true;
}

//=========================================================================================
int Element::LinesNeededForPrinting( void ) const
{
	if( type == SCALAR_ELEMENT_TYPE )
	{
		return 1;
	}
	else if( type == MATRIX_ELEMENT_TYPE )
	{
		return matrix->LinesNeededForPrinting();
	}

	return 0;
}

//=========================================================================================
bool Element::AssignIdentity( void )
{
	if( type == SCALAR_ELEMENT_TYPE )
	{
		*scalar = 1.0;
	}
	else if( type == MATRIX_ELEMENT_TYPE )
	{
		if( !matrix->AssignIdentity() )
			return false;
	}
	else
		return false;

	return true;
}

//=========================================================================================
bool Element::AssignScalar( double scalar )
{
	return AssignScalarFrom( Scalar( scalar ) );
}

//=========================================================================================
bool Element::AssignScalarFrom( const Scalar& scalar )
{
	if( !ReallocateMemory( SCALAR_ELEMENT_TYPE ) )
		return false;

	*this->scalar = scalar;
	return true;
}

//=========================================================================================
bool Element::AssignScalarTo( Scalar& scalar ) const
{
	if( type != SCALAR_ELEMENT_TYPE )
		return false;

	scalar = *this->scalar;
	return true;
}

//=========================================================================================
bool Element::AssignMatrixFrom( const Matrix& matrix )
{
	if( !ReallocateMemory( MATRIX_ELEMENT_TYPE ) )
		return false;

	if( !this->matrix->AssignFrom( matrix ) )
		return false;

	return true;
}

//=========================================================================================
bool Element::AssignMatrixTo( Matrix& matrix ) const
{
	if( type != MATRIX_ELEMENT_TYPE )
		return false;

	if( !matrix.AssignFrom( *this->matrix ) )
		return false;

	return false;
}

//=========================================================================================
bool Element::AssignFrom( const Element& element )
{
	if( this == &element )
		return true;

	if( !ReallocateMemory( element.type ) )
		return false;

	switch( type )
	{
		case SCALAR_ELEMENT_TYPE:
		{
			*scalar = *element.scalar;
			break;
		}
		case MATRIX_ELEMENT_TYPE:
		{
			if( !matrix->AssignFrom( *element.matrix ) )
				return false;
			break;
		}
	}

	return true;
}

//=========================================================================================
bool Element::AssignTo( Element& element ) const
{
	if( !element.AssignFrom( *this ) )
		return false;

	return true;
}

//=========================================================================================
bool Element::AssignSum( const Element& left, const Element& right )
{
	if( left.type != right.type )
		return false;

	if( !ReallocateMemory( left.type ) )
		return false;

	switch( type )
	{
		case SCALAR_ELEMENT_TYPE:
		{
			*scalar = *left.scalar + *right.scalar;
			break;
		}
		case MATRIX_ELEMENT_TYPE:
		{
			if( !matrix->AssignSum( *left.matrix, *right.matrix ) )
				return false;
			break;
		}
	}

	return true;
}

//=========================================================================================
bool Element::AssignProduct( const Element& left, const Element& right )
{
	if( left.type == MATRIX_ELEMENT_TYPE || right.type == MATRIX_ELEMENT_TYPE )
	{
		if( !ReallocateMemory( MATRIX_ELEMENT_TYPE ) )
			return false;
	}
	else
	{
		if( !ReallocateMemory( SCALAR_ELEMENT_TYPE ) )
			return false;
	}

	if( left.type == SCALAR_ELEMENT_TYPE && right.type == SCALAR_ELEMENT_TYPE )
	{
		*scalar = *left.scalar * *right.scalar;
	}
	else if( left.type == SCALAR_ELEMENT_TYPE && right.type == MATRIX_ELEMENT_TYPE )
	{
		if( !matrix->AssignFrom( *right.matrix ) )
			return false;

		if( !matrix->Scale( *left.scalar ) )
			return false;
	}
	else if( left.type == MATRIX_ELEMENT_TYPE && right.type == SCALAR_ELEMENT_TYPE )
	{
		if( !matrix->AssignFrom( *left.matrix ) )
			return false;

		if( !matrix->Scale( *right.scalar ) )
			return false;
	}
	else if( left.type == MATRIX_ELEMENT_TYPE && right.type == MATRIX_ELEMENT_TYPE )
	{
		if( !matrix->AssignProduct( *left.matrix, *right.matrix ) )
			return false;
	}
	else
		return false;

	return true;
}

//=========================================================================================
bool Element::AssignInverse( const Element& element )
{
	if( !ReallocateMemory( element.type ) )
		return false;
	
	if( type == SCALAR_ELEMENT_TYPE )
	{
		if( *element.scalar == 0.0 )
			return false;
		*scalar = *element.scalar;
		if( !scalar->Invert() )
			return false;
	}
	else if( type == MATRIX_ELEMENT_TYPE )
	{
		LinearAlgebra::Matrix::InverseResult inverseResult;
		if( !matrix->AssignInverse( *element.matrix, Matrix::LEFT_INVERSE, inverseResult ) )
			return false;
	}

	return true;
}

//=========================================================================================
bool Element::Accumulate( const Scalar& scalar )
{
	if( type != SCALAR_ELEMENT_TYPE )
		return false;

	*this->scalar = *this->scalar + scalar;
	return true;
}

//=========================================================================================
bool Element::Accumulate( const Element& element )
{
	if( type == UNKNOWN_ELEMENT_TYPE )
	{
		if( !ReallocateMemory( element.type ) )
			return false;
	}

	if( type != element.type )
		return false;

	switch( type )
	{
		case SCALAR_ELEMENT_TYPE:
		{
			*scalar = *scalar + *element.scalar;
			break;
		}
		case MATRIX_ELEMENT_TYPE:
		{
			if( !matrix->Accumulate( *element.matrix ) )
				return false;
			break;
		}
	}

	return true;
}

//=========================================================================================
bool Element::Scale( const Scalar& scalar )
{
	if( type == UNKNOWN_ELEMENT_TYPE )
		return false;

	switch( type )
	{
		case SCALAR_ELEMENT_TYPE:
		{
			*this->scalar = *this->scalar * scalar;
			break;
		}
		case MATRIX_ELEMENT_TYPE:
		{
			if( !matrix->Scale( scalar ) )
				return false;
			break;
		}
	}

	return true;
}

//=========================================================================================
Element::Type Element::ElementType( void ) const
{
	return type;
}

//=========================================================================================
Matrix* Element::AccessMatrix( void )
{
	if( type != MATRIX_ELEMENT_TYPE )
		return 0;
	return matrix;
}

//=========================================================================================
Scalar* Element::AccessScalar( void )
{
	if( type != SCALAR_ELEMENT_TYPE )
		return 0;
	return scalar;
}

//=========================================================================================
const Matrix* Element::AccessMatrix( void ) const
{
	if( type != MATRIX_ELEMENT_TYPE )
		return 0;
	return matrix;
}

//=========================================================================================
const Scalar* Element::AccessScalar( void ) const
{
	if( type != SCALAR_ELEMENT_TYPE )
		return 0;
	return scalar;
}

//=========================================================================================
bool Element::AllocateMemory( Type type )
{
	if( pointer != 0 )
		return false;

	this->type = type;
	switch( type )
	{
		case SCALAR_ELEMENT_TYPE: scalar = new Scalar(); break;
		case MATRIX_ELEMENT_TYPE: matrix = new Matrix(); break;
	}

	return true;
}

//=========================================================================================
bool Element::ReallocateMemory( Type type )
{
	// There is no need to re-allocate the memory if we are already the desired type.
	if( this->type != type )
	{
		if( pointer != 0 && !DeallocateMemory() )
			return false;

		if( !AllocateMemory( type ) )
			return false;
	}
	else
	{
		switch( type )
		{
			case SCALAR_ELEMENT_TYPE: *scalar = 0.0; break;
			case MATRIX_ELEMENT_TYPE: matrix->AssignZero(); break;
		}
	}

	return true;
}

//=========================================================================================
bool Element::DeallocateMemory( void )
{
	if( pointer == 0 )
		return false;

	switch( type )
	{
		default: return false;
		case SCALAR_ELEMENT_TYPE: delete scalar; break;
		case MATRIX_ELEMENT_TYPE: delete matrix; break;
	}

	type = UNKNOWN_ELEMENT_TYPE;
	pointer = 0;
	return true;
}

// Element.cpp