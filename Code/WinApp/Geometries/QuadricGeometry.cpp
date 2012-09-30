// QuadricGeometry.cpp

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "QuadricGeometry.h"

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( QuadricGeometry, GAVisToolGeometry );

//=========================================================================================
QuadricGeometry::QuadricGeometry( BindType bindType ) : GAVisToolGeometry( bindType )
{
	// Make composition and decomposition code for the quadric...

	// Test code...
	// a00 + 2*( a01*x + a02*y + a03*z ) +
	// a11*x^2 + 2*( a12*x*y + a13*x*z ) +
	// a22*y^2 + 2*( a23*y*z ) +
	// a33*z^2 = 0
	quadric.a11 = 1.0;
	quadric.a02 = 1.0;

	traceListValid = false;
}

//=========================================================================================
/*virtual*/ QuadricGeometry::~QuadricGeometry( void )
{
}

//=========================================================================================
/*static*/ GAVisToolBindTarget* QuadricGeometry::Create( BindType bindType )
{
	return new QuadricGeometry( bindType );
}

//=========================================================================================
/*virtual*/ void QuadricGeometry::DecomposeFrom( const GeometricAlgebra::SumOfBlades& element )
{
	//...

	traceListValid = false;
}

//=========================================================================================
/*virtual*/ void QuadricGeometry::ComposeTo( GeometricAlgebra::SumOfBlades& element ) const
{
}

//=========================================================================================
/*virtual*/ void QuadricGeometry::DumpInfo( char* printBuffer, int printBufferSize ) const
{
	sprintf_s( printBuffer, printBufferSize, "The variable \"%s\" is being interpreted as a quadric.\n", name );
}

//=========================================================================================
/*virtual*/ void QuadricGeometry::AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const
{
	wxString itemName = wxString::Format( wxT( "Quadric: %s" ), name );
	treeCtrl->AppendItem( parentItem, itemName, -1, -1, new GAVisToolInventoryTree::Data( id ) );
}

//=========================================================================================
/*virtual*/ void QuadricGeometry::Draw( GAVisToolRender& render, bool selected )
{
	// If our trace-list is out of date, go up-date it.  Hopefully this won't take too long.
	if( !traceListValid )
	{
		// TODO: We need to deduce the center of the quadric for the AABB setup to be correct.
		VectorMath::CoordFrame coordFrame;
		VectorMath::Identity( coordFrame );
		double range = 10.0;
		double planeCount = 10.0;
		VectorMath::Vector center, delta;
		VectorMath::Zero( center );
		VectorMath::Set( delta, 5.0, 5.0, 5.0 );
		VectorMath::Aabb aabb;
		MakeAabb( aabb, center, delta );
		quadric.GenerateTracesAlongAxis( coordFrame.xAxis, range, planeCount, aabb, traceList );
		quadric.GenerateTracesAlongAxis( coordFrame.yAxis, range, planeCount, aabb, traceList );
		quadric.GenerateTracesAlongAxis( coordFrame.zAxis, range, planeCount, aabb, traceList );
		traceListValid = true;
	}

	if( selected )
		render.Highlight( GAVisToolRender::NORMAL_HIGHLIGHTING );
	else
		render.Highlight( GAVisToolRender::NO_HIGHLIGHTING );

	render.Color( color, alpha );

	VectorMath::Quadric::Trace* trace = ( VectorMath::Quadric::Trace* )traceList.LeftMost();
	while( trace )
	{
		DrawTrace( trace, render );
		trace = ( VectorMath::Quadric::Trace* )trace->Right();
	}
}

//=========================================================================================
void QuadricGeometry::DrawTrace( VectorMath::Quadric::Trace* trace, GAVisToolRender& render )
{
	VectorMath::Quadric::Point* point = 0, *nextPoint = 0;
	VectorMath::Quadric::Point* firstPoint = ( VectorMath::Quadric::Point* )trace->pointList.LeftMost();
	for( point = firstPoint; point; point = nextPoint )
	{
		nextPoint = ( VectorMath::Quadric::Point* )point->Right();
		if( nextPoint )
			render.DrawLine( point->point, nextPoint->point );
		else if( trace->looped )
			render.DrawLine( point->point, firstPoint->point );
	}
}

//=========================================================================================
/*virtual*/ void QuadricGeometry::CalcCenter( VectorMath::Vector& center ) const
{
	VectorMath::Zero( center );
}

//=========================================================================================
/*virtual*/ void QuadricGeometry::Translate( const VectorMath::Vector& delta )
{
	// It should be easy to figure out how to translate the quadric.
	// Rotating it, on the other hand, sounds much harder.  Can GA help at all?
	// How do you scale a quadric about it's center?
}

//=========================================================================================
/*virtual*/ void QuadricGeometry::Rotate( const VectorMath::Vector& unitAxis, float angle )
{
}

//=========================================================================================
/*virtual*/ void QuadricGeometry::Scale( float scale )
{
}

// QuadricGeoemtry.cpp