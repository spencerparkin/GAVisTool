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
	renderAs = RENDER_AS_SET_OF_TRACES;
	surfaceGeometryValid = false;
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
	surfaceGeometryValid = false;
}

//=========================================================================================
/*virtual*/ void SurfaceGeometry::ComposeTo( GeometricAlgebra::SumOfBlades& element ) const
{
	element.AssignSumOfBlades( this->element );
}

//=========================================================================================
void SurfaceGeometry::RegenerateSurfaceGeometry( void )
{
	if( renderAs == RENDER_AS_SET_OF_TRACES )
	{
		// Kill the old trace list.
		traceList.RemoveAll( true );

		VectorMath::Surface::TraceParameters traceParameters;

		// A better center would be that of the surface, if we knew how to calculate that.
		VectorMath::Zero( traceParameters.center );

		traceParameters.range = 15.0;
		traceParameters.extent = 8.0;
		traceParameters.planeCount = 14;

		VectorMath::Vector axis[3];
		int axisCount = 3;
		VectorMath::Set( axis[0], 1.0, 0.0, 0.0 );
		VectorMath::Set( axis[1], 0.0, 1.0, 0.0 );
		VectorMath::Set( axis[2], 0.0, 0.0, 1.0 );

		// Create the new trace list.
		for( int index = 0; index < axisCount; index++ )
		{
			VectorMath::Copy( traceParameters.axis, axis[ index ] );

			// Append to the trace list all traces along this axis.
			surface->GenerateTracesAlongAxis( traceParameters, traceList );
		}
	}
	else if( renderAs == RENDER_AS_TRIANGLE_MESH )
	{
		//...
	}
}

//=========================================================================================
/*virtual*/ void SurfaceGeometry::Draw( GAVisToolRender& render, bool selected )
{
	// If our trace-list is out of date, go up-date it.  Hopefully this won't take too long.
	if( !surfaceGeometryValid )
	{
		RegenerateSurfaceGeometry();
		surfaceGeometryValid = true;
	}

	if( selected )
		render.Highlight( GAVisToolRender::NORMAL_HIGHLIGHTING );
	else
		render.Highlight( GAVisToolRender::NO_HIGHLIGHTING );

	render.Color( color, alpha );

	if( renderAs == RENDER_AS_SET_OF_TRACES )
	{
		VectorMath::Surface::Trace* trace = ( VectorMath::Surface::Trace* )traceList.LeftMost();
		while( trace )
		{
			DrawTrace( trace, render );
			trace = ( VectorMath::Surface::Trace* )trace->Right();
		}
	}
	else if( renderAs == RENDER_AS_TRIANGLE_MESH )
	{
		//...
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