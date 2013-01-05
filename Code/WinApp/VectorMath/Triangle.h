// Triangle.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#include "Plane.h"
#include "Vector.h"

namespace VectorMath
{
	struct Triangle
	{
		Vector vertex[3];
	};

	// We keep these seperate from the triangle geometry, because
	// users of the library may not always want to also toat around
	// triangle normals if they're not using them.
	struct TriangleNormals
	{
		Vector normal[3];
	};

	struct SideCountData
	{
		Plane::Side side[3];
		int countOnBack;
		int countOnFront;
		int countOnPlane;
	};

	void MakeTriangle( Triangle& triangle, const Vector& vertex0, const Vector& vertex1, const Vector& vertex2 );
	void MakeTriangleNormals( TriangleNormals& triangleNormals, const Vector& normal0, const Vector& normal1, const Vector& normal2 );
	void CopyTriangle( Triangle& dst, const Triangle& src );
	void CopyTriangleNormals( TriangleNormals& dst, const TriangleNormals& src );
	void CalcNormal( const Triangle& triangle, Vector& normal, bool normalize = false );
	void CalcCenter( const Triangle& triangle, Vector& center );
	void MakePlane( const Triangle& triangle, Plane& plane );
	void SideCount( const Triangle& triangle, const Plane& plane, SideCountData& sideCountData, double epsilon );
	bool SplitTriangle( const Triangle& triangle, const TriangleNormals* triangleNormals, const Plane& plane, const SideCountData& sideCountData, Triangle* triangleArray, TriangleNormals* triangleNormalsArray, Plane::Side* sideArray, int& triangleArraySize );
	bool IsPointInsideTriangle( const Triangle& triangle, const Plane& trianglePlane, const Vector& point, double epsilon = 1e-4 );
	bool IsPointInsideTriangle( const Triangle& triangle, const Vector& point, double epsilon = 1e-4 );
	bool LineSegmentIntersectsTriangle( const Triangle& triangle, const Plane& trianglePlane, const Vector& point0, const Vector& point1, double& lerp );
	bool TrianglesIntersect( const Triangle& triangle0, const Triangle& triangle1 );
	void InterpolateNormal( const Vector& vertex, Vector& normal, const Vector& vertex0, const Vector& normal0, const Vector& vertex1, const Vector& normal1 );
}

// Triangle.h