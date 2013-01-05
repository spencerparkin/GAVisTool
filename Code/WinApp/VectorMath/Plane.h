// Plane.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#include "Vector.h"

namespace VectorMath
{
	struct Plane
	{
		enum Side
		{
			SIDE_NEITHER,		// This means it is on the plane.
			SIDE_FRONT,			// This means it is on the side of the plane pointed to by the normal.
			SIDE_BACK,			// This means it is not on the front side of the plane or on the plane.
		};

		Vector normal;		// We assume this is of unit-length.
		double dot;
	};

	void CopyPlane( Plane& dst, const Plane& src );
	double ShortestDistanceToPlane( const Plane& plane, const Vector& pos );
	void ProjectPointOntoPlane( const Plane& plane, Vector& dst, const Vector& src );	// This is the orthogonal projection of the given point onto the given plane.
	void ProjectVectorOntoPlane( const Plane& plane, Vector& dst, const Vector& src );	// This is the orthogonal projection of the given vector onto the given plane.  (A rejection of the vector from the norm of the plane.)
	void MakePlane( Plane& plane, const Vector& pos, const Vector& vec );	// Initialize a plane structure to represent the plane at the given location with the given normal.
	Plane::Side PlaneSide( const Plane& plane, const Vector& pos, double epsilon = 1e-4 );	// Tell us which side of the plane the given point is on.  The normal of the plane points to its front.
	void SetPlanePos( Plane& plane, const Vector& pos );	// We assume here that the given plane has already been initialized.
	void GetPlanePos( const Plane& plane, Vector& pos );	// Give us the point on the plane closest to the origin.
	bool PlaneLineIntersect( const Plane& plane, const Vector& pos0, const Vector& pos1, double& lerp );	// The caller can check to see if the returned value is in [0,1].
}

// Plane.h