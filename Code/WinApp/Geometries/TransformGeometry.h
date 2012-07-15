// ConformalTransformGeometry.h

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#include "../Geometry.h"

//=========================================================================================
// What I'm calling here a conformal transform is the geometric product of a translation
// versor and a rotation versor (rotor), with possible scaling.  Without the scaling, this
// would be a rigid body transform.  There is not yet an element of geometric algebra, to
// my knowledge, that represents a transform like this that has non-uniform scale.  Such
// transforms would therefore have shear also.  We represent transforms in this class using
// a translation vector and a coordinate frame.  Non-uniform scale can be represented with
// these things, but we will not be taking advantage of that.
class ConformalTransformGeometry : public GAVisToolGeometry
{
	DECLARE_CALCLIB_CLASS( ConformalTransformGeometry );

public:

	ConformalTransformGeometry( BindType bindType );
	virtual ~ConformalTransformGeometry( void );

	static GAVisToolBindTarget* Create( BindType bindType );

	virtual void DecomposeFrom( const GeometricAlgebra::SumOfBlades& element ) override;
	virtual void ComposeTo( GeometricAlgebra::SumOfBlades& element ) const override;

	virtual void DumpInfo( char* printBuffer, int printBufferSize ) const override;

	virtual void AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const override;

	virtual void Draw( GAVisToolRender& render, bool selected );
	virtual void CalcCenter( VectorMath::Vector& center ) const;

	virtual void Translate( const VectorMath::Vector& delta );
	virtual void Rotate( const VectorMath::Vector& unitAxis, float angle );
	virtual void Scale( float scale );

	virtual void NameCenterOffset( VectorMath::Vector& offsetDelta );

private:

	VectorMath::Vector translation;
	VectorMath::Vector unitAxis;
	double angle;
	double scale;
};

// ConformalTransformGeometry.h