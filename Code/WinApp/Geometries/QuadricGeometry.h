// QuadricGeometry.h

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
#include "../VectorMath/ConvexHull.h"

//=========================================================================================
class QuadricGeometry : public GAVisToolGeometry
{
	DECLARE_CALCLIB_CLASS( QuadricGeometry );

public:

	QuadricGeometry( BindType bindType );
	virtual ~QuadricGeometry( void );

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

private:

	GeometricAlgebra::SumOfBlades element;

	// A quadric is not always a convex hull, but we
	// can still use this object to store the cloud
	// of points that helps us visualize the quadric.
	VectorMath::ConvexHull convexHull;
	bool convexHullValid;

	void RegenerateConvexHull( void );

public:

	static VectorMath::Aabb samplingRegion;
	static VectorMath::Vector samplingResolution;
};

// QuadricGeometry.h