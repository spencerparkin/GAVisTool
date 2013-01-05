// CompositeVector.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

//=========================================================================================
// The space of all bivectors of a given geometric algebra forms a linear
// space, and I would like to explore the geometric algebra that is generated
// by such a linear space.  To that end, this class is designed to represent
// the vector we would get from a given 2-blade.
class GeometricAlgebra::CompositeVector : public Vector
{
public:

	CompositeVector( Vector* left, Vector* right );
	virtual ~CompositeVector( void );

	virtual Item* MakeCopy( void ) const override;
	virtual Vector* MakeBar( ScalarAlgebra::Scalar& sign ) const override;
	virtual double InnerProduct( const Vector& right ) const override;
	virtual const char* Name( void ) const override;
	virtual const char* LatexName( void ) const override;
	virtual bool IsComposite( void ) const override;

private:

	Vector* left, *right;
	char name[32];
	char latexName[128];
};

// CompositeVector.h