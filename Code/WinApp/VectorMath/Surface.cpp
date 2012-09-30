// Surface.cpp

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "Surface.h"
#include "Spline.h"

// TODO: Trace planes cuting a double-cone exactly in half don't compute quite right.

//=============================================================================
VectorMath::Surface::Surface( void )
{
}

//=============================================================================
/*virtual*/ VectorMath::Surface::~Surface( void )
{
}

//=============================================================================
VectorMath::Surface::Point::Point( const VectorMath::Vector& point )
{
	VectorMath::Copy( this->point, point );
}

//=============================================================================
/*virtual*/ VectorMath::Surface::Point::~Point( void )
{
}

//=============================================================================
VectorMath::Surface::Trace::Trace( void )
{
	looped = false;
}

//=============================================================================
/*virtual*/ VectorMath::Surface::Trace::~Trace( void )
{
	pointList.RemoveAll( true );
}

//=============================================================================
bool VectorMath::Surface::Trace::IsPointOnTrace( const Vector& givenPoint, double epsilon ) const
{
	for( const Point* point = ( const Point* )pointList.LeftMost(); point; point = ( const Point* )point->Right() )
	{
		// Is the given point within the given distance of this point?
		Vector delta;
		Sub( delta, point->point, givenPoint );
		double dist = Length( delta );
		if( dist <= epsilon )
			return true;

		// Is the given point within the given distance of the line-segment between this point and the next point?
		const Point* nextPoint = ( const Point* )point->Right();
		if( nextPoint )
		{
			LinearSpline linearSpline;
			Copy( linearSpline.controlPoint[0], point->point );
			Copy( linearSpline.controlPoint[1], nextPoint->point );
			dist = linearSpline.ShortestDistanceToSpline( givenPoint );
			if( dist != -1.0 && dist <= epsilon )
				return true;

			// How about testing the point against a curve going through this and the next two points?
			const Point* nextNextPoint = ( const Point* )nextPoint->Right();
			if( nextNextPoint )
			{
				QuadraticSpline quadraticSpline;
				Copy( quadraticSpline.controlPoint[0], point->point );
				Copy( quadraticSpline.controlPoint[1], nextPoint->point );
				Copy( quadraticSpline.controlPoint[2], nextNextPoint->point );
				dist = quadraticSpline.ShortestDistanceToSpline( givenPoint );
				if( dist != -1.0 && dist <= epsilon )
					return true;
			}
		}
	}

	return false;
}

//=============================================================================
void VectorMath::Surface::GenerateTracesAlongAxis( const Vector& axis, double range, double planeCount, const Aabb& aabb, Utilities::List& traceList, bool resetList /*= false*/ )
{
	if( resetList )
		traceList.RemoveAll( true );

	// Build a right-handed coordinate frame with the Z-axis being the given axis.
	// The trace planes will be orthogonal to the given axis or planes parallel to
	// the XY-plane in this frame.
	CoordFrame coordFrame;
	Copy( coordFrame.zAxis, axis );
	Orthogonal( coordFrame.xAxis, axis );
	Cross( coordFrame.yAxis, axis, coordFrame.xAxis );

	// Generate traces in planes orthogonal to the given axis in the desired range.
	Vector pos[2];
	Scale( pos[0], axis, -range / 2.0 );
	Scale( pos[1], axis, range / 2.0 );
	double epsilon = 0.001;
	double dt = 1.0 / planeCount;
	for( double t = dt * 0.5; t <= 1.0; t += dt )
	{
		// Build a plane at this point along the axis.
		Vector planePos;
		Lerp( planePos, pos[0], pos[1], t );
		Plane plane;
		MakePlane( plane, planePos, axis );

		// There could be zero, one or two traces in the given plane
		// in the case that our surface is a quadric.  Our job here is
		// to try to find all of them.

		// Try to get a trace with the following seed.
		Vector seed;
		//Set( seed, -range / 2.0, -range / 2.0, 0.0 );
		Set( seed, -range / 4.0, -range / 4.0, 0.0 );
		Transform( seed, coordFrame, seed );
		Add( seed, seed, planePos );
		Trace* firstTrace = CalculateTraceInPlane( plane, seed, aabb );

		// Try to get another trace, but don't repeat the first trace, if any.
		//Set( seed, range / 2.0, range / 2.0, 0.0 );
		Set( seed, range / 4.0, range / 4.0, 0.0 );
		Transform( seed, coordFrame, seed );
		Add( seed, seed, planePos );
		Trace* secondTrace = 0;
		if( ConvergePointToSurfaceInPlane( plane, seed, epsilon ) )
			if( !( firstTrace && firstTrace->IsPointOnTrace( seed, 0.1 ) ) )
				secondTrace = CalculateTraceInPlane( plane, seed, aabb );

		// Add the traces, if any were found.
		if( firstTrace )
			traceList.InsertRightOf( traceList.RightMost(), firstTrace );
		if( secondTrace )
			traceList.InsertRightOf( traceList.RightMost(), secondTrace );
	}
}

//=============================================================================
VectorMath::Surface::Trace* VectorMath::Surface::CalculateTraceInPlane( const Plane& plane, const Vector& seed, const Aabb& aabb )
{
	double epsilon = 1e-7;
	int tracePointLimit = 100;
	
	// The initial seed must converge to a point on the surface.
	Vector point;
	Copy( point, seed );
	if( !ConvergePointToSurfaceInPlane( plane, point, epsilon ) )
		return 0;

	// We have a point on the surface, so our trace will be non-empty.
	Trace* trace = new Trace();

	// Begin the tracing process in the first direction.
	double traceDelta = 0.5;		// This needs to be well above our epsilon.
	int direction = 0;
	while( direction < 2 )
	{
		bool stepMade = false;

		// Have we exited the box?
		bool exitedBox = Aabb::IS_OUTSIDE_BOX == AabbSide( aabb, point );

		// If we're in the box, then we can add the point and advance.
		if( !exitedBox )
		{
			// Add the traced point to the list.  Keep the order of the list
			// such that it can be used to draw a poly-line of the trace.
			if( direction == 0 )
				trace->pointList.InsertRightOf( trace->pointList.RightMost(), new Point( point ) );
			else
				trace->pointList.InsertLeftOf( trace->pointList.LeftMost(), new Point( point ) );

			// We shouldn't need this limit for our algorithm to always correctly terminate,
			// but the limit is put in place here to prevent us from creating too many lines
			// and thereby overloading the render system.
			if( trace->pointList.Count() >= tracePointLimit )
				break;

			// Attempt to trace the surface in the desired direction.
			stepMade = StepTraceInPlane( plane, direction, point, traceDelta, epsilon );

			// After each successful step, check to see if we have come full circle.
			if( stepMade )
			{
				// If we run into our own trace, we can always quit the algorithm,
				// because we know that we have come full circle.
				if( trace->IsPointOnTrace( point, 0.1 ) )
				{
					// Indicate that the caller should consider the trace a line-loop
					// in the case that the trace is more than one point.  If it is
					// just one point, then our trace plane is probably tangent to the surface.
					if( trace->pointList.Count() > 1 )
						trace->looped = true;
					break;
				}
			}
		}

		// Failure to step or exiting the box are grounds for direction change.
		if( !stepMade || exitedBox )
		{
			// Go in the other direction.
			direction++;
			if( direction < 2 )
			{
				// Reset to our original position on the surface.
				Copy( point, seed );
				ConvergePointToSurfaceInPlane( plane, point, epsilon );

				// Take the initial step now, because we have already considered the initial point on the surface.
				if( !StepTraceInPlane( plane, direction, point, traceDelta, epsilon ) )
					break;
			}
		}
	}

	// Finally, return the trace.
	return trace;
}

//=============================================================================
bool VectorMath::Surface::StepTraceInPlane( const Plane& plane, int direction, Vector& point, double traceDelta, double epsilon )
{
	// Calculate the direction of the translation vector along which we'll move the point in the given plane.
	Vector gradient, delta;
	EvaluateGradientAt( point, gradient );
	if( direction == 0 )
		Cross( delta, gradient, plane.normal );
	else
		Cross( delta, plane.normal, gradient );

	// If the delta ended up zero, then we don't know where to move the point.
	// This could happen, for example, if we were trying to trace a point on
	// a plane in that plane's plane.  Could this also be the case when we hit
	// the central vertex of double-cone?
	double length = Length( delta );
	if( length <= epsilon )
		return false;

	// Calculate the amount of desired translation.
	Scale( delta, delta, traceDelta / length );

	// Apply the translation vector.
	Add( point, point, delta );

	// Our step now succeeds if the translated point converges back onto the
	// surface, hopefully in a new position on the surface in the desired direction.
	return ConvergePointToSurfaceInPlane( plane, point, epsilon );
}

//=============================================================================
bool VectorMath::Surface::ConvergePointToSurfaceInPlane( const Plane& plane, Vector& point, double epsilon )
{
	double deltaLength = 1.0;

	// Is there a way to prove or disprove that this algorithm converges?
	int maxIters = 1000;
	for( int count = 0; count < maxIters; count++ )
	{
		// Project the point down onto the plane.
		// This is mainly to account for accumulated round-off error.
		ProjectPointOntoPlane( plane, point, point );

		// Is the point approximately on the surface?
		double value = EvaluateAt( point );
		if( fabs( value ) <= epsilon )
			return true;

		// No, we need to move the point.
		// In what direction should we move?
		Vector gradient, delta;
		EvaluateGradientAt( point, gradient );
		ProjectVectorOntoPlane( plane, delta, gradient );
		Normalize( delta, delta );
		Scale( delta, delta, deltaLength );

		// Move in that direction.
		Vector movedPoint;
		bool isValuePositive = value > 0.0;
		if( isValuePositive )
			Sub( movedPoint, point, delta );
		else
			Add( movedPoint, point, delta );

		// Did we over-shoot our target?
		double movedValue = EvaluateAt( movedPoint );
		bool isMovedValuePositive = movedValue > 0.0;
		if( isMovedValuePositive != isValuePositive )
		{
			// Yes, reject the move and half our move amount.
			deltaLength /= 2.0;
		}
		else
		{
			// No, accept the move.
			Copy( point, movedPoint );

			// As a sanity check, did the value improve?
			if( fabs( movedValue ) >= fabs( value ) )
			{
				// Value will not improve in the case that
				// that there is no trace in the given plane.
				return false;
			}
		}
	}

	// Failure occures if our algorithm fails to converge the point
	// onto the surface in the alotted number of iterations.
	return false;
}

//=============================================================================
void VectorMath::Surface::EvaluateGradientAt( const VectorMath::Vector& point, VectorMath::Vector& gradient )
{
	gradient.x = EvaluatePartialX( point );
	gradient.y = EvaluatePartialY( point );
	gradient.z = EvaluatePartialZ( point );
}

//=============================================================================
VectorMath::Quadric::Quadric( void )
{
	a00 = a01 = a02 = a03 = 0.0;
	a11 = a12 = a13 = 0.0;
	a22 = a23 = 0.0;
	a33 = 0.0;
}

//=============================================================================
/*virtual*/ VectorMath::Quadric::~Quadric( void )
{
}

//=============================================================================
/*virtual*/ double VectorMath::Quadric::EvaluateAt( const VectorMath::Vector& point )
{
	double x = point.x;
	double y = point.y;
	double z = point.z;
	return a00 + 2.0*( a01*x + a02*y + a03*z ) + a11*x*x + 2.0*( a12*x*y + a13*x*z ) + a22*y*y + 2.0*( a23*y*z ) + a33*z*z;
}

//=============================================================================
/*virtual*/ double VectorMath::Quadric::EvaluatePartialX( const VectorMath::Vector& point )
{
	double x = point.x;
	double y = point.y;
	double z = point.z;
	return 2.0*( a01 + a11*x + a12*y + a13*z );
}

//=============================================================================
/*virtual*/ double VectorMath::Quadric::EvaluatePartialY( const VectorMath::Vector& point )
{
	double x = point.x;
	double y = point.y;
	double z = point.z;
	return 2.0*( a02 + a12*x + a22*y + a23*z );
}

//=============================================================================
/*virtual*/ double VectorMath::Quadric::EvaluatePartialZ( const VectorMath::Vector& point )
{
	double x = point.x;
	double y = point.y;
	double z = point.z;
	return 2.0*( a03 + a13*x + a23*y + a33*z );
}

// Surface.cpp