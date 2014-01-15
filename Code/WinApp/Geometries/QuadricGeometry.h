// QuadricGeometry.h

/*
 * Copyright (C) 2013-2014 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#include "SurfaceGeometry.h"
#include "../VectorMath/Surface.h"
#include "../VectorMath/ConvexHull.h"

//=========================================================================================
// This bind target can interpret any quadric geometry.  It may be
// worth creating specific bind targets for specific quadric geometries,
// so that those geometries can be rendered using a triangle mesh.
// To render general quadrics using a triangle mesh would require
// a fairly sufisticated skinning algorithm.
class QuadricGeometry : public SurfaceGeometry
{
	DECLARE_CALCLIB_CLASS( QuadricGeometry );

public:

	QuadricGeometry( BindType bindType );
	virtual ~QuadricGeometry( void );

	static GAVisToolBindTarget* Create( BindType bindType );

	virtual void DecomposeFrom( const GeometricAlgebra::SumOfBlades& element ) override;

	virtual void DumpInfo( char* printBuffer, int printBufferSize ) const override;
	virtual void AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const override;

private:

	void DrawTrace( VectorMath::Quadric::Trace* trace, GAVisToolRender& render );

	VectorMath::Quadric quadric;
};

// QuadricGeometry.h