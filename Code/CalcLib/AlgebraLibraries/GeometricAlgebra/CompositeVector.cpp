// CompositeVector.cpp

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
CompositeVector::CompositeVector( Vector* left, Vector* right )
{
	this->left = left;
	this->right = right;

	sprintf_s( name, sizeof( name ), "%s_%s", left->Name(), right->Name() );
	sprintf_s( latexName, sizeof( latexName ), "[%s\\wedge %s]", left->LatexName(), right->LatexName() );
}

//=========================================================================================
/*virtual*/ CompositeVector::~CompositeVector( void )
{
	delete left;
	delete right;
}

//=========================================================================================
/*virtual*/ Utilities::List::Item* CompositeVector::MakeCopy( void ) const
{
	Vector* leftCopy = ( Vector* )left->MakeCopy();
	Vector* rightCopy = ( Vector* )right->MakeCopy();
	CompositeVector* copy = new CompositeVector( leftCopy, rightCopy );
	return copy;
}

//=========================================================================================
// We do not support the bar function.
/*virtual*/ Vector* CompositeVector::MakeBar( ScalarAlgebra::Scalar& sign ) const
{
	return 0;
}

//=========================================================================================
/*virtual*/ double CompositeVector::InnerProduct( const Vector& right ) const
{
	// It really only makes sense to take the inner product between composite vectors.
	if( !right.IsComposite() )
		return 0.0;

	const CompositeVector* compositeVectorRight = ( const CompositeVector* )&right;

	// (a^b).(c^d) = a.(b.(c^d) = a.((b.c)d - (b.d)c) = (a.d)(b.c) - (a.c)(b.d)
	
	const Vector* a = this->left;
	const Vector* b = this->right;
	const Vector* c = compositeVectorRight->left;
	const Vector* d = compositeVectorRight->right;

	double result = a->InnerProduct( *d ) * b->InnerProduct( *c ) - a->InnerProduct( *c ) * b->InnerProduct( *d );
	return result;
}

//=========================================================================================
/*virtual*/ const char* CompositeVector::Name( void ) const
{
	return name;
}

//=========================================================================================
/*virtual*/ const char* CompositeVector::LatexName( void ) const
{
	return latexName;
}

//=========================================================================================
/*virtual*/ bool CompositeVector::IsComposite( void ) const
{
	return true;
}

// CompositeVector.cpp