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
		if( ConvergePointToSurfaceInPlane( &plane, seed, epsilon ) )
			if( !( firstTrace && firstTrace->IsPointOnTrace( seed, 0.005 ) ) )
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
	double epsilon = 1e-8;
	int tracePointLimit = 100;
	
	// The initial seed must converge to a point on the surface.
	Vector point;
	Copy( point, seed );
	if( !ConvergePointToSurfaceInPlane( &plane, point, epsilon ) )
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
				if( trace->IsPointOnTrace( point, 0.005 ) )
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
				ConvergePointToSurfaceInPlane( &plane, point, epsilon );

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
	return ConvergePointToSurfaceInPlane( &plane, point, epsilon );
}

//=============================================================================
bool VectorMath::Surface::ConvergePointToSurfaceInPlane( const Plane* plane, Vector& point, double epsilon )
{
	// This must be small, but bigger than epsilons used to detect trace loops.
	double deltaLength = 0.1;

	// Is there a way to prove or disprove that this algorithm converges?
	int maxIters = 1000;
	for( int count = 0; count < maxIters; count++ )
	{
		// Project the point down onto the plane.
		// This is mainly to account for accumulated round-off error.
		if( plane )
			ProjectPointOntoPlane( *plane, point, point );

		// Is the point approximately on the surface?
		double value = EvaluateAt( point );
		if( fabs( value ) <= epsilon )
			return true;

		// No, we need to move the point.
		// In what direction should we move?
		Vector gradient, delta;
		EvaluateGradientAt( point, gradient );
		if( plane )
			ProjectVectorOntoPlane( *plane, delta, gradient );
		else
			Copy( delta, gradient );
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
				// The value will not improve in the case that
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
VectorMath::Surface::ManifoldPoint::ManifoldPoint( const Vector& point )
{
	Copy( this->point, point );

	for( int index = 0; index < MAX_DEGREE; index++ )
		adjacentPoint[ index ] = 0;

	degree = 0;
}

//=============================================================================
/*virtual*/ VectorMath::Surface::ManifoldPoint::~ManifoldPoint( void )
{
}

//=============================================================================
// I don't know if this will ever work.
bool VectorMath::Surface::GenerateManifold( const Aabb& aabb, const Vector& seedPoint, Utilities::List& manifoldPointList )
{
	// Make sure we're starting with a new empty list.
	manifoldPointList.RemoveAll( true );

	// If the seed point doesn't converge to the surface, then we're done.
	double epsilon = 1e-6;
	Vector convergedPoint;
	Copy( convergedPoint, seedPoint );
	if( !ConvergePointToSurfaceInPlane( 0, convergedPoint, epsilon ) )
		return false;

	// Queue the initial point on the surface.
	Utilities::List pointQueue;
	pointQueue.InsertRightOf( pointQueue.RightMost(), new ManifoldPoint( convergedPoint ) );

	// Process the queue until it is empty.
	while( pointQueue.Count() > 0 )
	{
		// Grabe the next point off the queue and process it.
		ManifoldPoint* nextPoint = ( ManifoldPoint* )pointQueue.LeftMost();
		pointQueue.Remove( nextPoint, false );
		if( !MakeManifoldAboutPoint( aabb, nextPoint, manifoldPointList, pointQueue, epsilon ) )
			break;
	}

	// Remove any points remaining in the queue due to possible early termination.
	pointQueue.RemoveAll( true );
	return true;
}

//=============================================================================
bool VectorMath::Surface::MakeManifoldAboutPoint( const Aabb& aabb, ManifoldPoint* centralManifoldPoint, Utilities::List& manifoldPointList, Utilities::List& pointQueue, double epsilon )
{
	Vector gradient, axis;
	EvaluateGradientAt( centralManifoldPoint->point, gradient );
	Normalize( axis, gradient );

	Vector firstPeripheralSurfacePoint;
	double manifoldRadius = 1.0;
	double manifoldAngle = 2.0 * PI / double( ManifoldPoint::MAX_DEGREE );
	double squareDistance = 0.0;

	for( int index = 0; index < ManifoldPoint::MAX_DEGREE; index++ )
	{
		ManifoldPoint* manifoldPoint = 0;

		if( !index )
		{
			manifoldPoint = FindNearestManifoldPoint( manifoldPointList, centralManifoldPoint->point, squareDistance );
			if( manifoldPoint && squareDistance <= manifoldRadius * manifoldRadius + epsilon )
				Copy( firstPeripheralSurfacePoint, manifoldPoint->point );
			else
			{
				Orthogonal( firstPeripheralSurfacePoint, axis );
				Scale( firstPeripheralSurfacePoint, firstPeripheralSurfacePoint, manifoldRadius );
				for( int subIndex = 0; subIndex < ManifoldPoint::MAX_DEGREE; subIndex++ )
				{
					if( ConvergePointToSurfaceInPlane( 0, firstPeripheralSurfacePoint, epsilon ) )
					{
						if( AabbSide( aabb, firstPeripheralSurfacePoint ) != Aabb::IS_OUTSIDE_BOX )
						{
							manifoldPoint = new ManifoldPoint( firstPeripheralSurfacePoint );
							pointQueue.InsertRightOf( pointQueue.RightMost(), manifoldPoint );
							break;
						}
					}
					double angle = manifoldAngle * double( subIndex );
					Rotate( firstPeripheralSurfacePoint, firstPeripheralSurfacePoint, axis, angle );
				}
			}
		}
		else
		{
			double angle = manifoldAngle * double( index );
			Vector rotatedPeripheralSurfacePoint;
			Rotate( rotatedPeripheralSurfacePoint, firstPeripheralSurfacePoint, axis, angle );
			if( ConvergePointToSurfaceInPlane( 0, rotatedPeripheralSurfacePoint, epsilon ) )
			{
				manifoldPoint = FindNearestManifoldPoint( manifoldPointList, rotatedPeripheralSurfacePoint, squareDistance );
				if( !manifoldPoint )
				{
					if( AabbSide( aabb, rotatedPeripheralSurfacePoint ) != Aabb::IS_OUTSIDE_BOX )
					{
						manifoldPoint = new ManifoldPoint( rotatedPeripheralSurfacePoint );
						pointQueue.InsertRightOf( pointQueue.RightMost(), manifoldPoint );
					}
				}
			}
		}

		// Null entries are valid.  Such entries mark boundries of the surface due to being cut by the AABB.
		centralManifoldPoint->adjacentPoint[ index ] = manifoldPoint;
	}

	manifoldPointList.InsertRightOf( manifoldPointList.RightMost(), centralManifoldPoint );
	return true;
}

//=============================================================================
// This is definitely going to be the bottle-neck of the algorithm.  Currently
// this does a linear search, but it may be worthy trying to speed this up with
// an algorithm based upon some sort of spacial sorting/partitioning data structure.
VectorMath::Surface::ManifoldPoint* VectorMath::Surface::FindNearestManifoldPoint( const Utilities::List& manifoldPointList, const Vector& point, double& squareDistance )
{
	VectorMath::Surface::ManifoldPoint* foundPoint = 0;
	squareDistance = -1.0;
	for( const ManifoldPoint* manifoldPoint = ( const ManifoldPoint* )manifoldPointList.LeftMost(); manifoldPoint; manifoldPoint = ( const ManifoldPoint* )manifoldPoint->Right() )
	{
		double thisSquareDistance = Dot( point, manifoldPoint->point );
		if( thisSquareDistance < squareDistance || squareDistance == -1.0 )
		{
			squareDistance = thisSquareDistance;
			foundPoint = const_cast< ManifoldPoint* >( manifoldPoint );
		}
	}
	return foundPoint;
}

//=============================================================================
/*virtual*/ void VectorMath::Surface::EvaluateGradientAt( const VectorMath::Vector& point, VectorMath::Vector& gradient )
{
	gradient.x = EvaluatePartialX( point );
	gradient.y = EvaluatePartialY( point );
	gradient.z = EvaluatePartialZ( point );
}

//=============================================================================
VectorMath::Quadric::Quadric( void )
{
	A = B = C = D = E = F = G = H = I = J = 0.0;
}

//=============================================================================
/*virtual*/ VectorMath::Quadric::~Quadric( void )
{
}

//=============================================================================
/*virtual*/ double VectorMath::Quadric::EvaluateAt( const VectorMath::Vector& point )
{
	// f(x,y,z) = Ax^2 + By^2 + Cz^2 + Dxy + Exz + Fyz + Gx + Hy + Iz + J
	double x = point.x;
	double y = point.y;
	double z = point.z;
	return A*x*x + B*y*y + C*z*z + D*x*y + E*x*z + F*y*z + G*x + H*y + I*z + J;
}

//=============================================================================
/*virtual*/ double VectorMath::Quadric::EvaluatePartialX( const VectorMath::Vector& point )
{
	// f_x(x,y,z) = 2Ax + Dy + Ez + G
	double x = point.x;
	double y = point.y;
	double z = point.z;
	return 2.0*A*x + D*y + E*z + G;
}

//=============================================================================
/*virtual*/ double VectorMath::Quadric::EvaluatePartialY( const VectorMath::Vector& point )
{
	// f_y(x,y,z) = 2By + Dx + Fz + H
	double x = point.x;
	double y = point.y;
	double z = point.z;
	return 2.0*B*y + D*x + F*z + H;
}

//=============================================================================
/*virtual*/ double VectorMath::Quadric::EvaluatePartialZ( const VectorMath::Vector& point )
{
	// f_z(x,y,z) = 2Cz + Ex + Fy + I
	double x = point.x;
	double y = point.y;
	double z = point.z;
	return 2.0*C*z + E*x + F*y + I;
}

//=============================================================================
VectorMath::ConformalQuartic::ConformalQuartic( void )
{
	c0 = c1 = c2 = c3 = c4 = 0.0;
	c5 = c6 = c7 = c8 = c9 = 0.0;
	c10 = c11 = c12 = c13 = c14 = 0.0;
	c15 = c16 = c17 = c18 = c19 = 0.0;
	c20 = c21 = c22 = c23 = c24 = 0.0;
}

//=============================================================================
/*virtual*/ VectorMath::ConformalQuartic::~ConformalQuartic( void )
{
}

//=============================================================================
/*virtual*/ double VectorMath::ConformalQuartic::EvaluateAt( const VectorMath::Vector& point )
{
	double x = point.x;
	double y = point.y;
	double z = point.z;
	double value =
		-0.25*c0*x*x*x*x +
		-0.50*c0*x*x*y*y +
		-0.50*c0*x*x*z*z +
		-0.25*c0*y*y*y*y +
		-0.50*c0*y*y*z*z +
		-0.25*c0*z*z*z*z +
		0.50*c10*x*x*y +
		0.50*c10*y*y*y +
		0.50*c10*y*z*z +
		-c11*x*y +
		-c12*y*y +
		-c13*y*z +
		c14*y +
		0.50*c15*x*x*z +
		0.50*c15*y*y*z +
		0.50*c15*z*z*z +
		-c16*x*z +
		-c17*y*z +
		-c18*z*z +
		c19*z +
		0.50*c1*x*x*x +
		0.50*c1*x*y*y +
		0.50*c1*x*z*z +
		-0.50*c20*x*x +
		-0.50*c20*y*y +
		-0.50*c20*z*z +
		c21*x +
		c22*y +
		c23*z +
		-c24 +
		0.50*c2*x*x*y +
		0.50*c2*y*y*y +
		0.50*c2*y*z*z +
		0.50*c3*x*x*z +
		0.50*c3*y*y*z +
		0.50*c3*z*z*z +
		-0.50*c4*x*x +
		-0.50*c4*y*y +
		-0.50*c4*z*z +
		0.50*c5*x*x*x +
		0.50*c5*x*y*y +
		0.50*c5*x*z*z +
		-c6*x*x +
		-c7*x*y +
		-c8*x*z +
		c9*x;
	return value;
}

//=============================================================================
/*virtual*/ double VectorMath::ConformalQuartic::EvaluatePartialX( const VectorMath::Vector& point )
{
	double x = point.x;
	double y = point.y;
	double z = point.z;
	double value =
		-c0*x*x*x +
		-c0*x*y*y +
		-c0*x*z*z +
		c10*x*y +
		-c11*y +
		c15*x*z +
		-c16*z +
		1.50*c1*x*x +
		0.50*c1*y*y +
		0.50*c1*z*z +
		-c20*x +
		c21 +
		c2*x*y +
		c3*x*z +
		-c4*x +
		1.50*c5*x*x +
		0.50*c5*y*y +
		0.50*c5*z*z +
		-2.00*c6*x +
		-c7*y +
		-c8*z +
		c9;
	return value;
}

//=============================================================================
/*virtual*/ double VectorMath::ConformalQuartic::EvaluatePartialY( const VectorMath::Vector& point )
{
	double x = point.x;
	double y = point.y;
	double z = point.z;
	double value =
		-c0*x*x*y +
		-c0*y*y*y +
		-c0*y*z*z +
		0.50*c10*x*x +
		1.50*c10*y*y +
		0.50*c10*z*z +
		-c11*x +
		-2.00*c12*y +
		-c13*z +
		c14 +
		c15*y*z +
		-c17*z +
		c1*x*y +
		-c20*y +
		c22 +
		0.50*c2*x*x +
		1.50*c2*y*y +
		0.50*c2*z*z +
		c3*y*z +
		-c4*y +
		c5*x*y +
		-c7*x;
	return value;
}

//=============================================================================
/*virtual*/ double VectorMath::ConformalQuartic::EvaluatePartialZ( const VectorMath::Vector& point )
{
	double x = point.x;
	double y = point.y;
	double z = point.z;
	double value =
		-c0*x*x*z +
		-c0*y*y*z +
		-c0*z*z*z +
		c10*y*z +
		-c13*y +
		0.50*c15*x*x +
		0.50*c15*y*y +
		1.50*c15*z*z +
		-c16*x +
		-c17*y +
		-2.00*c18*z +
		c19 +
		c1*x*z +
		-c20*z +
		c23 +
		c2*y*z +
		0.50*c3*x*x +
		0.50*c3*y*y +
		1.50*c3*z*z +
		-c4*z +
		c5*x*z +
		-c8*x;
	return value;
}

// Surface.cpp