// PointCloudGeometry.cpp

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "PointCloudGeometry.h"

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( PointCloudGeometry, GAVisToolGeometry );

//=========================================================================================
PointCloudGeometry::PointCloudGeometry( void ) : GAVisToolGeometry( NORMAL_FORM )
{
	VectorMath::Set( center, 0.0, 0.0, 0.0 );
}

//=========================================================================================
/*virtual*/ PointCloudGeometry::~PointCloudGeometry( void )
{
}

//=========================================================================================
/*virtual*/ void PointCloudGeometry::DecomposeFrom( const GeometricAlgebra::SumOfBlades& element )
{
}

//=========================================================================================
/*virtual*/ void PointCloudGeometry::ComposeTo( GeometricAlgebra::SumOfBlades& element ) const
{
}

//=========================================================================================
/*virtual*/ void PointCloudGeometry::DumpInfo( char* printBuffer, int printBufferSize ) const
{
}

//=========================================================================================
/*virtual*/ void PointCloudGeometry::AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const
{
}

//=========================================================================================
/*virtual*/ void PointCloudGeometry::Draw( GAVisToolRender& render, bool selected )
{
	using namespace VectorMath;

	if( selected )
		render.Highlight( GAVisToolRender::NORMAL_HIGHLIGHTING );
	else
		render.Highlight( GAVisToolRender::NO_HIGHLIGHTING );
	render.Color( color, alpha );

	/*
	for( int index = 0; index < convexHull.CloudSize(); index++ )
	{
		Vector& point = convexHull[ index ];
		Vector normal;
		Sub( normal, point, center );
		render.DrawPoint( point, normal );
	}

	if( render.GetRenderMode() != GAVisToolRender::RENDER_MODE_SELECTION )
	{
		render.Color( color, alpha * 0.5 );

		Utilities::List& triangleList = convexHull.TriangleList();
		for( ConvexHull::Triangle* triangle = ( ConvexHull::Triangle* )triangleList.LeftMost(); triangle; triangle = ( ConvexHull::Triangle* )triangle->Right() )
		{
			Triangle triangleGeometry;
			Copy( triangleGeometry.vertex[0], convexHull[ triangle->vertex[0] ] );
			Copy( triangleGeometry.vertex[1], convexHull[ triangle->vertex[1] ] );
			Copy( triangleGeometry.vertex[2], convexHull[ triangle->vertex[2] ] );

			render.DrawTriangle( triangleGeometry );
		}
	}
	*/
}

//=========================================================================================
/*virtual*/ void PointCloudGeometry::CalcCenter( VectorMath::Vector& center ) const
{
	VectorMath::Copy( center, this->center );
}

//=========================================================================================
/*virtual*/ void PointCloudGeometry::Translate( const VectorMath::Vector& delta )
{
//	VectorMath::Add( center, center, delta );
//	for( int index = 0; index < convexHull.CloudSize(); index++ )
//		VectorMath::Add( convexHull[ index ], convexHull[ index ], delta );
}

//=========================================================================================
/*virtual*/ void PointCloudGeometry::Rotate( const VectorMath::Vector& unitAxis, float angle )
{
	/*for( int index = 0; index < convexHull.CloudSize(); index++ )
	{
		VectorMath::Vector vec;
		VectorMath::Sub( vec, convexHull[ index ], center );
		VectorMath::Rotate( vec, vec, unitAxis, angle );
		VectorMath::Add( convexHull[ index ], center, vec );
	}*/
}

//=========================================================================================
/*virtual*/ void PointCloudGeometry::Scale( float scale )
{
	/*for( int index = 0; index < convexHull.CloudSize(); index++ )
	{
		VectorMath::Vector vec;
		VectorMath::Sub( vec, convexHull[ index ], center );
		VectorMath::Scale( vec, vec, scale );
		VectorMath::Add( convexHull[ index ], center, vec );
	}*/
}

// PointCloudGeometry.cpp