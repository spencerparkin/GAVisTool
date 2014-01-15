// Plane.cpp

/*
 * Copyright (C) 2013-2014 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "Plane.h"
#include <math.h>

//=============================================================================
void VectorMath::CopyPlane( Plane& dst, const Plane& src )
{
	Copy( dst.normal, src.normal );
	dst.dot = src.dot;
}

//=============================================================================
void VectorMath::MakePlane( Plane& plane, const Vector& pos, const Vector& vec )
{
	Normalize( plane.normal, vec );
	SetPlanePos( plane, pos );
}

//=============================================================================
void VectorMath::ProjectPointOntoPlane( const Plane& plane, Vector& dst, const Vector& src )
{
	double orthoDist = ShortestDistanceToPlane( plane, src );
	Vector delta;
	if( Plane::SIDE_FRONT == PlaneSide( plane, src ) )
		Scale( delta, plane.normal, -orthoDist );
	else
		Scale( delta, plane.normal, orthoDist );
	Add( dst, src, delta );
}

//=============================================================================
void VectorMath::ProjectVectorOntoPlane( const Plane& plane, Vector& dst, const Vector& src )
{
	Reject( dst, src, plane.normal );
}

//=============================================================================
double VectorMath::ShortestDistanceToPlane( const Plane& plane, const Vector& pos )
{
	// All solutions v to the plane equation is (v-p).n=0 are points
	// on the plane.  But even if (v-p).n is not zero, the value of
	// this expression gives a meaningful result.  It is the shortest
	// orthogonal signed distance to the plane, because |n|=1.  By signed,
	// we mean that the sign indicates which side of the plane the point
	// is on, while the magnitude represents the distance.
	double orthoDist = Dot( pos, plane.normal ) - plane.dot;
	return orthoDist;
}

//=============================================================================
VectorMath::Plane::Side VectorMath::PlaneSide( const Plane& plane, const Vector& pos, double epsilon /*= 1e-4*/ )
{
	double orthoDist = ShortestDistanceToPlane( plane, pos );

	// If we're within the given threshold of zero, then approximate the given
	// point as being on the plane.
	if( fabs( orthoDist ) <= epsilon )
		return Plane::SIDE_NEITHER;

	// If (v-p).n > 0, then v-p and n make an acute angle, showing that
	// v is on the front side of the plane.
	if( orthoDist > 0.0 )
		return Plane::SIDE_FRONT;

	// If (v-p).n < 0, then v-p and n make an obtuse angle, showing that
	// v is on the back side of the plane.
	return Plane::SIDE_BACK;
}

//=============================================================================
void VectorMath::SetPlanePos( Plane& plane, const Vector& pos )
{
	// The plane is the set of all vectors v such that (v-p).n=0,
	// where p is a point on the plane, and n is a normal to the plane.
	// This can be written as v.n - p.n = 0, showing that along with
	// n, we need only store p.n in the representation of a plane.
	plane.dot = Dot( pos, plane.normal );
}

//=============================================================================
void VectorMath::GetPlanePos( const Plane& plane, Vector& pos )
{
	// We want to find the scalar s such that
	// (sn-p).n = 0.  Expanding, we get s-p.n=0,
	// because |n|=1, which implies that s=p.n.
	Scale( pos, plane.normal, plane.dot );
}

//=============================================================================
bool VectorMath::PlaneLineIntersect( const Plane& plane, const Vector& pos0, const Vector& pos1, double& lerp )
{
	// (a+t(b-a)-p).n=0, solve for t.
	// (a-p).n + t(b-a).n=0
	// t = [(a-p).n]/[(a-b).n]
	// t = (a.n-p.n)/(a.n-b.n)
	// Notice that if (a-b).n=0, then clearly there can be no intersection as
	// the line is parallel to the plane.
	double a_dot_n = Dot( pos0, plane.normal );
	double b_dot_n = Dot( pos1, plane.normal );
	double numerator = a_dot_n - plane.dot;
	double denominator = a_dot_n - b_dot_n;
	if( denominator == 0.0 )
		return false;
	lerp = numerator / denominator;		// Could we still blow up if the denominator is close to zero?
	return true;
}

// Plane.cpp