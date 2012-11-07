// SurfaceGeometry.cpp

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "SurfaceGeometry.h"

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( SurfaceGeometry, GAVisToolGeometry );

//=========================================================================================
SurfaceGeometry::SurfaceGeometry( BindType bindType, VectorMath::Surface* surface ) : GAVisToolGeometry( bindType )
{
	traceListValid = false;
	this->surface = surface;
}

//=========================================================================================
/*virtual*/ SurfaceGeometry::~SurfaceGeometry( void )
{
	traceList.RemoveAll( true );
}

//=========================================================================================
/*virtual*/ void SurfaceGeometry::DecomposeFrom( const GeometricAlgebra::SumOfBlades& element )
{
	this->element.AssignSumOfBlades( element );
	traceListValid = false;
}

//=========================================================================================
/*virtual*/ void SurfaceGeometry::ComposeTo( GeometricAlgebra::SumOfBlades& element ) const
{
	element.AssignSumOfBlades( this->element );
}

//=========================================================================================
/*virtual*/ void SurfaceGeometry::Draw( GAVisToolRender& render, bool selected )
{
	// If our trace-list is out of date, go up-date it.  Hopefully this won't take too long.
	if( !traceListValid )
	{
		// TODO: We need to deduce the center of the surface for the AABB setup to be correct.
		VectorMath::CoordFrame coordFrame;
		VectorMath::Identity( coordFrame );
		double range = 20.0;
		double planeCount = 40.0;
		VectorMath::Vector center, delta;
		VectorMath::Zero( center );
		VectorMath::Set( delta, 10.0, 10.0, 10.0 );
		VectorMath::Aabb aabb;
		MakeAabb( aabb, center, delta );
		traceList.RemoveAll( true );
		surface->GenerateTracesAlongAxis( coordFrame.xAxis, range, planeCount, aabb, traceList );
		surface->GenerateTracesAlongAxis( coordFrame.yAxis, range, planeCount, aabb, traceList );
		surface->GenerateTracesAlongAxis( coordFrame.zAxis, range, planeCount, aabb, traceList );
		traceListValid = true;
	}

	if( selected )
		render.Highlight( GAVisToolRender::NORMAL_HIGHLIGHTING );
	else
		render.Highlight( GAVisToolRender::NO_HIGHLIGHTING );

	render.Color( color, alpha );

	VectorMath::Surface::Trace* trace = ( VectorMath::Surface::Trace* )traceList.LeftMost();
	while( trace )
	{
		DrawTrace( trace, render );
		trace = ( VectorMath::Surface::Trace* )trace->Right();
	}
}

//=========================================================================================
void SurfaceGeometry::DrawTrace( VectorMath::Surface::Trace* trace, GAVisToolRender& render )
{
	VectorMath::Surface::Point* point = 0, *nextPoint = 0;
	VectorMath::Surface::Point* firstPoint = ( VectorMath::Surface::Point* )trace->pointList.LeftMost();
	for( point = firstPoint; point; point = nextPoint )
	{
		nextPoint = ( VectorMath::Surface::Point* )point->Right();
		if( nextPoint )
			render.DrawLine( point->point, nextPoint->point );
		else if( trace->looped )
			render.DrawLine( point->point, firstPoint->point );
	}
}

//=========================================================================================
/*virtual*/ void SurfaceGeometry::CalcCenter( VectorMath::Vector& center ) const
{
	VectorMath::Zero( center );
}

//=========================================================================================
/*virtual*/ void SurfaceGeometry::Translate( const VectorMath::Vector& delta )
{
}

//=========================================================================================
/*virtual*/ void SurfaceGeometry::Rotate( const VectorMath::Vector& unitAxis, float angle )
{
}

//=========================================================================================
/*virtual*/ void SurfaceGeometry::Scale( float scale )
{
}

// SurfaceGeometry.cpp