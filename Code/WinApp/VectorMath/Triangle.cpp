// Triangle.cpp

/*
 * Copyright (C) 2013-2014 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "Triangle.h"

//=============================================================================
void VectorMath::MakeTriangle( Triangle& triangle, const Vector& vertex0, const Vector& vertex1, const Vector& vertex2 )
{
	Copy( triangle.vertex[0], vertex0 );
	Copy( triangle.vertex[1], vertex1 );
	Copy( triangle.vertex[2], vertex2 );
}

//=============================================================================
void VectorMath::MakeTriangleNormals( TriangleNormals& triangleNormals, const Vector& normal0, const Vector& normal1, const Vector& normal2 )
{
	Copy( triangleNormals.normal[0], normal0 );
	Copy( triangleNormals.normal[1], normal1 );
	Copy( triangleNormals.normal[2], normal2 );
}

//=============================================================================
void VectorMath::CopyTriangle( Triangle& dst, const Triangle& src )
{
	Copy( dst.vertex[0], src.vertex[0] );
	Copy( dst.vertex[1], src.vertex[1] );
	Copy( dst.vertex[2], src.vertex[2] );
}

//=============================================================================
void VectorMath::CopyTriangleNormals( TriangleNormals& dst, const TriangleNormals& src )
{
	Copy( dst.normal[0], src.normal[0] );
	Copy( dst.normal[1], src.normal[1] );
	Copy( dst.normal[2], src.normal[2] );
}

//=============================================================================
// Viewing the triangle with the vertices in CCW order, the normal we will
// face towards you.
void VectorMath::CalcNormal( const Triangle& triangle, Vector& normal, bool normalize /*= false*/ )
{
	Vector vec01, vec02;
	Sub( vec01, triangle.vertex[1], triangle.vertex[0] );
	Sub( vec02, triangle.vertex[2], triangle.vertex[0] );
	Cross( normal, vec01, vec02 );
	if( normalize )
		Normalize( normal, normal );
}

//=============================================================================
// Here we just average the 3 vertices.
void VectorMath::CalcCenter( const Triangle& triangle, Vector& center )
{
	Add( center, triangle.vertex[0], triangle.vertex[1] );
	Add( center, center, triangle.vertex[2] );
	Scale( center, center, 1.0 / 3.0 );
}

//=============================================================================
// Here we arbitrarily use the first vertex of the triangle.
void VectorMath::MakePlane( const Triangle& triangle, Plane& plane )
{
	Vector normal;
	CalcNormal( triangle, normal, false );
	MakePlane( plane, triangle.vertex[0], normal );
}

//=============================================================================
void VectorMath::SideCount(
						const Triangle& triangle,
						const Plane& plane,
						SideCountData& sideCountData,
						double epsilon )
{
	sideCountData.side[0] = PlaneSide( plane, triangle.vertex[0], epsilon );
	sideCountData.side[1] = PlaneSide( plane, triangle.vertex[1], epsilon );
	sideCountData.side[2] = PlaneSide( plane, triangle.vertex[2], epsilon );

	sideCountData.countOnBack = 0;
	sideCountData.countOnFront = 0;
	sideCountData.countOnPlane = 0;

	for( int index = 0; index < 3; index++ )
	{
		switch( sideCountData.side[ index ] )
		{
			case Plane::SIDE_BACK:		sideCountData.countOnBack++;	break;
			case Plane::SIDE_FRONT:		sideCountData.countOnFront++;	break;
			case Plane::SIDE_NEITHER:	sideCountData.countOnPlane++;	break;
		}
	}
}

//=============================================================================
// Here we assume that the given triangle array has room for at least 3 triangles.
// We return true if and only if the given triangle was split into two or more triangles.
// The given "sideCountData" must be generated by a call to "SideCount" using the given
// triangle and plane.
bool VectorMath::SplitTriangle(
						const Triangle& triangle,
						const TriangleNormals* triangleNormals,
						const Plane& plane,
						const SideCountData& sideCountData,
						Triangle* triangleArray,
						TriangleNormals* triangleNormalsArray,
						Plane::Side* sideArray,
						int& triangleArraySize )
{
	if( ( sideCountData.countOnBack == 1 && sideCountData.countOnFront == 2 ) ||
		( sideCountData.countOnBack == 2 && sideCountData.countOnFront == 1 ) )
	{
		// Create a map so that we know the first two vertices on the same side, making
		// the third vertex the one on the other side.
		int map[3];
		for( int index = 0; index < 3; index++ )
		{
			int nextIndex = ( index + 1 ) % 3;
			if( sideCountData.side[ index ] == sideCountData.side[ nextIndex ] )
			{
				map[0] = index;
				map[1] = nextIndex;
				map[2] = ( nextIndex + 1 ) % 3;
				break;
			}
		}

		double lerp02, lerp12;
		PlaneLineIntersect( plane, triangle.vertex[ map[0] ], triangle.vertex[ map[2] ], lerp02 );
		PlaneLineIntersect( plane, triangle.vertex[ map[1] ], triangle.vertex[ map[2] ], lerp12 );
		Vector vertex02, vertex12;
		Lerp( vertex02, triangle.vertex[ map[0] ], triangle.vertex[ map[2] ], lerp02 );
		Lerp( vertex12, triangle.vertex[ map[1] ], triangle.vertex[ map[2] ], lerp12 );
		
		// Arbitrarily tesselate the triangle.
		triangleArraySize = 3;
		MakeTriangle( triangleArray[0], triangle.vertex[ map[0] ], triangle.vertex[ map[1] ], vertex12 );
		MakeTriangle( triangleArray[1], triangle.vertex[ map[0] ], vertex12, vertex02 );
		MakeTriangle( triangleArray[2], vertex02, vertex12, triangle.vertex[ map[2] ] );

		if( triangleNormals )
		{
			Vector normal02, normal12;

#if 1
			VectorMath::Slerp( normal02,
								triangleNormals->normal[ map[0] ],
								triangleNormals->normal[ map[2] ],
								lerp02 );
			VectorMath::Slerp( normal12,
								triangleNormals->normal[ map[1] ],
								triangleNormals->normal[ map[2] ],
								lerp12 );
#else
			InterpolateNormal( vertex02, normal02,
								triangle.vertex[ map[0] ], triangleNormals->normal[ map[0] ],
								triangle.vertex[ map[2] ], triangleNormals->normal[ map[2] ] );
			InterpolateNormal( vertex12, normal12,
								triangle.vertex[ map[1] ], triangleNormals->normal[ map[1] ],
								triangle.vertex[ map[2] ], triangleNormals->normal[ map[2] ] );
#endif

			MakeTriangleNormals( triangleNormalsArray[0], triangleNormals->normal[ map[0] ], triangleNormals->normal[ map[1] ], normal12 );
			MakeTriangleNormals( triangleNormalsArray[1], triangleNormals->normal[ map[0] ], normal12, normal02 );
			MakeTriangleNormals( triangleNormalsArray[2], normal02, normal12, triangleNormals->normal[ map[2] ] );
		}

		sideArray[0] = sideCountData.side[ map[0] ];
		sideArray[1] = sideCountData.side[ map[1] ];
		sideArray[2] = sideCountData.side[ map[2] ];

		return true;
	}
	else if( sideCountData.countOnPlane == 1 && sideCountData.countOnBack == 1 && sideCountData.countOnFront == 1 )
	{
		// Create a map so that we know that the first index is on the plane,
		// the second is one one side and the third on the other.
		int map[3];
		for( int index = 0; index < 3; index++ )
		{
			if( sideCountData.side[ index ] == Plane::SIDE_NEITHER )
			{
				map[0] = index;
				map[1] = ( index + 1 ) % 3;
				map[2] = ( index + 2 ) % 3;
				break;
			}
		}

		double lerp12;
		PlaneLineIntersect( plane, triangle.vertex[ map[1] ], triangle.vertex[ map[2] ], lerp12 );
		Vector vertex12;
		Lerp( vertex12, triangle.vertex[ map[1] ], triangle.vertex[ map[2] ], lerp12 );
		
		triangleArraySize = 2;
		MakeTriangle( triangleArray[0], triangle.vertex[ map[0] ], triangle.vertex[ map[1] ], vertex12 );
		MakeTriangle( triangleArray[1], triangle.vertex[ map[0] ], vertex12, triangle.vertex[ map[2] ] );

		if( triangleNormals )
		{
			Vector normal12;

#if 1
			VectorMath::Slerp( normal12,
								triangleNormals->normal[ map[1] ],
								triangleNormals->normal[ map[2] ],
								lerp12 );
#else
			InterpolateNormal( vertex12, normal12,
								triangle.vertex[ map[1] ], triangleNormals->normal[ map[1] ],
								triangle.vertex[ map[2] ], triangleNormals->normal[ map[2] ] );
#endif

			MakeTriangleNormals( triangleNormalsArray[0], triangleNormals->normal[ map[0] ], triangleNormals->normal[ map[1] ], normal12 );
			MakeTriangleNormals( triangleNormalsArray[1], triangleNormals->normal[ map[0] ], normal12, triangleNormals->normal[ map[2] ] );
		}

		sideArray[0] = sideCountData.side[ map[1] ];
		sideArray[1] = sideCountData.side[ map[2] ];

		return true;
	}
	
	return false;
}

//=============================================================================
// Here we're given two vertices and their normals along with a third vertex
// with an unknown normal for that vertex, and what we want to do here is
// calculate the unknown normal for that vertex.
void VectorMath::InterpolateNormal( const Vector& vertex, Vector& normal,
									const Vector& vertex0, const Vector& normal0,
									const Vector& vertex1, const Vector& normal1 )
{
	Plane plane;
	Vector delta, vec;
	double lerp;
	Vector point0, point1;

	// Calculate point0.
	Sub( delta, vertex1, vertex0 );
	Cross( vec, delta, normal0 );
	Cross( plane.normal, normal0, vec );
	Normalize( plane.normal, plane.normal );
	SetPlanePos( plane, vertex0 );
	Add( vec, vertex1, normal1 );
	PlaneLineIntersect( plane, vertex1, vec, lerp );
	Lerp( point0, vertex1, vec, lerp );

	// Calculate point1.
	Cross( vec, normal1, delta );
	Cross( plane.normal, normal1, vec );
	Normalize( plane.normal, plane.normal );
	SetPlanePos( plane, vertex1 );
	Add( vec, vertex0, normal0 );
	PlaneLineIntersect( plane, vertex0, vec, lerp );
	Lerp( point1, vertex0, vec, lerp );

	// Finally, calculate the interpolated normal.
	Lerp( vec, point0, point1, 0.5 );
	Sub( normal, vertex, vec );
	Normalize( normal, normal );
}

//=============================================================================
// Here we assume that the given point is on the plane determined by the
// given triangle, (assuming the given triangle is not degenerate, it does
// determine a plane.)  Also, we assume here that the given plane contains
// the given triangle!
bool VectorMath::IsPointInsideTriangle( const Triangle& triangle, const Plane& trianglePlane, const Vector& point, double epsilon /*= 1e-4*/ )
{
	// For this algorithm to be correct, the function that makes a plane
	// out of the triangle must choose the front-side of that plane based
	// on a CCW ordering of the vertices in the triangle.
	Vector crossPoint, vec, edge;
	for( int index = 0; index < 3; index++ )
	{
		int nextIndex = ( index + 1 ) % 3;
		Sub( vec, point, triangle.vertex[ index ] );
		Sub( edge, triangle.vertex[ nextIndex ], triangle.vertex[ index ] );
		Cross( crossPoint, edge, vec );
		Add( crossPoint, crossPoint, triangle.vertex[ index ] );
		Plane::Side side = PlaneSide( trianglePlane, crossPoint );
		if( side == Plane::SIDE_BACK || side == Plane::SIDE_NEITHER )
			return false;
	}

	return true;
}

//=============================================================================
bool VectorMath::IsPointInsideTriangle( const Triangle& triangle, const Vector& point, double epsilon /*= 1e-4*/ )
{
	Plane trianglePlane;
	MakePlane( triangle, trianglePlane );
	return IsPointInsideTriangle( triangle, trianglePlane, point );
}

//=============================================================================
// Here we assume that the given plane contains the given triangle!
bool VectorMath::LineSegmentIntersectsTriangle( const Triangle& triangle, const Plane& trianglePlane, const Vector& point0, const Vector& point1, double& lerp )
{
	if( !PlaneLineIntersect( trianglePlane, point0, point1, lerp ) )
		return false;
	if( lerp <= 0.0 || lerp >= 1.0 )
		return false;

	Vector intersectionPoint;
	Lerp( intersectionPoint, point0, point1, lerp );
	return IsPointInsideTriangle( triangle, trianglePlane, intersectionPoint );
}

//=============================================================================
// By definition, we don't want triangles that merely touch one another
// to be counted as intersecting.
bool VectorMath::TrianglesIntersect( const Triangle& triangle0, const Triangle& triangle1 )
{
	VectorMath::Plane plane0, plane1;
	MakePlane( triangle0, plane0 );
	MakePlane( triangle1, plane1 );

	VectorMath::Vector midEdge;
	double lerp;
	for( int index = 0; index < 3; index++ )
	{
		int nextIndex = ( index + 1 ) % 3;
		int remainingIndex = ( index + 2 ) % 3;

		if( LineSegmentIntersectsTriangle( triangle1, plane1, triangle0.vertex[ index ], triangle0.vertex[ nextIndex ], lerp ) )
			return true;

		if( LineSegmentIntersectsTriangle( triangle0, plane0, triangle1.vertex[ index ], triangle1.vertex[ nextIndex ], lerp ) )
			return true;

		VectorMath::Lerp( midEdge, triangle0.vertex[ nextIndex ], triangle0.vertex[ remainingIndex ], 0.5 );
		if( LineSegmentIntersectsTriangle( triangle1, plane1, triangle0.vertex[ index ], midEdge, lerp ) )
			return true;

		VectorMath::Lerp( midEdge, triangle1.vertex[ nextIndex ], triangle1.vertex[ remainingIndex ], 0.5 );
		if( LineSegmentIntersectsTriangle( triangle0, plane0, triangle1.vertex[ index ], midEdge, lerp ) )
			return true;
	}

	return false;
}

// Triangle.cpp