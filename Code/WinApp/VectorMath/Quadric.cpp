// Quadric.cpp

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "Quadric.h"

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
VectorMath::Quadric::Point::Point( const VectorMath::Vector& point )
{
	VectorMath::Copy( this->point, point );
}

//=============================================================================
/*virtual*/ VectorMath::Quadric::Point::~Point( void )
{
}

//=============================================================================
VectorMath::Quadric::Trace::Trace( void )
{
}

//=============================================================================
/*virtual*/ VectorMath::Quadric::Trace::~Trace( void )
{
	pointList.RemoveAll( true );
}

//=============================================================================
bool VectorMath::Quadric::Trace::IsPointOnTrace( const Vector& givenPoint, double epsilon ) const
{
	for( const Point* point = ( const Point* )pointList.LeftMost(); point; point = ( const Point* )point->Right() )
	{
		// Is the given point within the given distance of this point?
		Vector delta;
		Sub( delta, point->point, givenPoint );
		double length = Length( delta );
		if( length <= epsilon )
			return true;

		// Is the given point within the given distance of the line-segment between this point and the next point?
		//...
	}

	return false;
}

//=============================================================================
void VectorMath::Quadric::GenerateTracesAlongAxis( const Vector& axis, double range, double density, Utilities::List& traceList, bool resetList /*= false*/ )
{
	if( resetList )
		traceList.RemoveAll( true );

	// All traces will be contained within this AABB.
	Aabb aabb;
	Vector zero;
	Zero( zero );
	Vector delta;
	Set( delta, 1.0, 1.0, 1.0 );
	Normalize( delta, delta );
	Scale( delta, delta, sqrt( 3.0 / 4.0 ) * range );
	MakeAabb( aabb, zero, delta );

	// Generate traces in planes orthogonal to the given axis in the desired range.
	Vector pos[2];
	Scale( pos[0], axis, -range / 2.0 );
	Scale( pos[1], axis, range / 2.0 );
	double epsilon = 0.001;
	double dt = 1.0 / density;
	for( double t = 0.0; t <= 1.0; t += dt )
	{
		// Build a plane at this point along the axis.
		Vector planePos;
		Lerp( planePos, pos[0], pos[1], t );
		Plane plane;
		MakePlane( plane, planePos, axis );

		// Build a frame in this plane.
		CoordFrame coordFrame;
		Copy( coordFrame.zAxis, axis );
		Orthogonal( coordFrame.xAxis, axis );
		Cross( coordFrame.yAxis, axis, coordFrame.xAxis );

		// There could be zero, one or two traces in the given plane.
		// Our job here is to try to find all of them.

		// Try to get a trace with the following seed.
		Vector seed;
		Set( seed, -range / 2.0, -range / 2.0, 0.0 );
		Transform( seed, coordFrame, seed );
		Add( seed, seed, planePos );
		Trace* firstTrace = CalculateTraceInPlane( plane, seed, aabb );

		// Try to get another trace, but don't repeat the first trace, if any.
		Set( seed, range / 2.0, range / 2.0, 0.0 );
		Transform( seed, coordFrame, seed );
		Add( seed, seed, planePos );
		Trace* secondTrace = 0;
		if( ConvergePointToQuadricInPlane( plane, seed, epsilon ) )
			if( !( firstTrace && firstTrace->IsPointOnTrace( seed, 0.2 ) ) )
				secondTrace = CalculateTraceInPlane( plane, seed, aabb );

		// Add the traces, if any were found.
		if( firstTrace )
			traceList.InsertRightOf( traceList.RightMost(), firstTrace );
		if( secondTrace )
			traceList.InsertRightOf( traceList.RightMost(), secondTrace );
	}
}

//=============================================================================
VectorMath::Quadric::Trace* VectorMath::Quadric::CalculateTraceInPlane( const Plane& plane, const Vector& seed, const Aabb& aabb )
{
	double epsilon = 0.01;
	Vector point;
	Copy( point, seed );
	if( !ConvergePointToQuadricInPlane( plane, point, epsilon ) )
		return 0;

	Trace* trace = new Trace();
	double traceDelta = 0.1;
	int direction = 0;

	while( direction < 2 )
	{
		if( direction == 0 )
			trace->pointList.InsertRightOf( trace->pointList.RightMost(), new Point( point ) );
		else
			trace->pointList.InsertLeftOf( trace->pointList.LeftMost(), new Point( point ) );
		if( !StepTraceInPlane( plane, direction, point, traceDelta ) || Aabb::IS_OUTSIDE_BOX == AabbSide( aabb, point ) )
		{
			direction++;
			if( direction < 2 )
			{
				Copy( point, seed );
				ConvergePointToQuadricInPlane( plane, point, epsilon );
				if( !StepTraceInPlane( plane, direction, point, traceDelta ) )
					break;
			}
		}
	}

	return trace;
}

//=============================================================================
bool VectorMath::Quadric::StepTraceInPlane( const Plane& plane, int direction, Vector& point, double traceDelta )
{
	Vector gradient, delta;
	EvaluateGradientAt( point, gradient );
	if( direction == 0 )
		Cross( delta, gradient, plane.normal );
	else
		Cross( delta, plane.normal, gradient );
	Normalize( delta, delta );
	Scale( delta, delta, traceDelta );
	Add( point, point, delta );
	double epsilon = 0.001;
	return ConvergePointToQuadricInPlane( plane, point, epsilon );
}

//=============================================================================
bool VectorMath::Quadric::ConvergePointToQuadricInPlane( const Plane& plane, Vector& point, double epsilon )
{
	int maxIters = 1000;
	for( int count = 0; count < maxIters; count++ )
	{
		// Project the point down onto the plane.
		ProjectOntoPlane( plane, point, point );

		// Is the point approximately on the quadric?
		double value = EvaluateAt( point );
		if( fabs( value ) <= epsilon )
			return true;

		// No, move it toward the quadric.
		Vector gradient;
		EvaluateGradientAt( point, gradient );
		Add( point, point, gradient );		// What's the significance of the length of the gradient?!  Figure it out!
	}

	return false;
}

//=============================================================================
double VectorMath::Quadric::EvaluateAt( const VectorMath::Vector& point )
{
	double x = point.x;
	double y = point.y;
	double z = point.z;
	return a00 + 2.0*( a01*x + a02*y + a03*z ) + a11*x*x + 2.0*( a12*x*y + a13*x*z ) + a22*y*y + 2.0*( a23*y*z ) + a33*z*z;
}

//=============================================================================
double VectorMath::Quadric::EvaluatePartialX( const VectorMath::Vector& point )
{
	double x = point.x;
	double y = point.y;
	double z = point.z;
	return 2.0*( a01 + a11*x + a12*y + a13*z );
}

//=============================================================================
double VectorMath::Quadric::EvaluatePartialY( const VectorMath::Vector& point )
{
	double x = point.x;
	double y = point.y;
	double z = point.z;
	return 2.0*( a02 + a12*x + a22*y + a23*z );
}

//=============================================================================
double VectorMath::Quadric::EvaluatePartialZ( const VectorMath::Vector& point )
{
	double x = point.x;
	double y = point.y;
	double z = point.z;
	return 2.0*( a03 + a13*x + a23*y + a33*z );
}

//=============================================================================
void VectorMath::Quadric::EvaluateGradientAt( const VectorMath::Vector& point, VectorMath::Vector& gradient )
{
	gradient.x = EvaluatePartialX( point );
	gradient.y = EvaluatePartialY( point );
	gradient.z = EvaluatePartialZ( point );
}

// Quadric.cpp