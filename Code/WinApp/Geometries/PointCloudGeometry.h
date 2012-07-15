// PointCloudGeometry.h

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#include "../VectorMath/ConvexHull.h"
#include "../Geometry.h"

//=========================================================================================
class PointCloudGeometry : public GAVisToolGeometry
{
	DECLARE_CALCLIB_CLASS( PointCloudGeometry );

public:

	PointCloudGeometry( void );
	virtual ~PointCloudGeometry( void );

	virtual void DecomposeFrom( const GeometricAlgebra::SumOfBlades& element );
	virtual void ComposeTo( GeometricAlgebra::SumOfBlades& element ) const;

	virtual void DumpInfo( char* printBuffer, int printBufferSize ) const override;

	virtual void AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const override;

	virtual void Draw( GAVisToolRender& render, bool selected ) override;
	virtual void CalcCenter( VectorMath::Vector& center ) const override;

	// Notice that the convex hull does not need to be regenerated under any of these transformations.
	virtual void Translate( const VectorMath::Vector& delta ) override;
	virtual void Rotate( const VectorMath::Vector& unitAxis, float angle ) override;
	virtual void Scale( float scale ) override;

	VectorMath::Vector center;
	VectorMath::ConvexHull convexHull;
};

// PointCloudGeometry.h