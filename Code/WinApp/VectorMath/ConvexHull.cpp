// ConvexHull.cpp

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

// TODO: This code has not yet been tested.  Test it.

#include "ConvexHull.h"

using namespace VectorMath;

//=============================================================================
ConvexHull::ConvexHull( void )
{
	pointCloud = 0;
	pointCloudSize = 0;
}

//=============================================================================
/*virtual*/ ConvexHull::~ConvexHull( void )
{
	triangleList.RemoveAll( true );
	pointList.RemoveAll( true );

	Deallocate();
}

//=============================================================================
void ConvexHull::Reallocate( int pointCloudSize )
{
	Deallocate();

	this->pointCloudSize = pointCloudSize;
	pointCloud = new Vector[ pointCloudSize ];
}

//=============================================================================
void ConvexHull::Deallocate( void )
{
	if( pointCloud )
		delete[] pointCloud;

	pointCloud = 0;
	pointCloudSize = 0;
}

//=============================================================================
Vector& ConvexHull::operator[]( int index )
{
	return pointCloud[ index ];
}

//=============================================================================
int ConvexHull::CloudSize( void )
{
	return pointCloudSize;
}

//=============================================================================
Utilities::List& ConvexHull::TriangleList( void )
{
	return triangleList;
}

//=============================================================================
ConvexHull::Triangle::Triangle( int vertex0, int vertex1, int vertex2 )
{
	vertex[0] = vertex0;
	vertex[1] = vertex1;
	vertex[2] = vertex2;
}

//=============================================================================
/*virtual*/ ConvexHull::Triangle::~Triangle( void )
{
}

//=============================================================================
ConvexHull::Point::Point( int vertex )
{
	this->vertex = vertex;
}

//=============================================================================
/*virtual*/ ConvexHull::Point::~Point( void )
{
}

//=============================================================================
bool ConvexHull::Regenerate( void )
{
	triangleList.RemoveAll( true );
	pointList.RemoveAll( true );

	for( int index = 0; index < pointCloudSize; index++ )
		pointList.InsertRightOf( pointList.RightMost(), new Point( index ) );

	Point* tetrahedron[4];
	if( !FindFourNonCoplanarPoints( tetrahedron ) )
	{
		pointList.RemoveAll( true );
		return false;
	}

	IntegrateTetrahedron( tetrahedron );
	CullPoints();

	while( pointList.Count() > 0 )
	{
		Point* point = ChoosePoint();
		ExpandHull( point );
		CullPoints();
	}

	return true;
}

//=============================================================================
void ConvexHull::CullPoints( void )
{
	Point* nextPoint = 0;
	for( Point* point = ( Point* )pointList.LeftMost(); point; point = nextPoint )
	{
		nextPoint = ( Point* )point->Right();
		if( PointIsOnOrInsideHull( point ) )
			pointList.Remove( point, true );
	}
}

//=============================================================================
bool ConvexHull::PointIsOnOrInsideHull( Point* point )
{
	Vector vertex;
	Copy( vertex, pointCloud[ point->vertex ] );
	for( Triangle* triangle = ( Triangle* )triangleList.LeftMost(); triangle; triangle = ( Triangle* )triangle->Right() )
		if( Plane::SIDE_FRONT == PlaneSide( triangle->plane, vertex ) )
			return false;
	return true;
}

//=============================================================================
void ConvexHull::IntegrateTetrahedron( Point** tetrahedron )
{
	for( int index = 0; index < 4; index++ )
	{
		Vector vertex;
		Copy( vertex, pointCloud[ tetrahedron[ index ]->vertex ] );

		Triangle* triangleFace = new Triangle(
						tetrahedron[ ( index + 1 ) % 4 ]->vertex,
						tetrahedron[ ( index + 2 ) % 4 ]->vertex,
						tetrahedron[ ( index + 3 ) % 4 ]->vertex );

		VectorMath::Triangle triangle;
		Copy( triangle.vertex[0], pointCloud[ triangleFace->vertex[0] ] );
		Copy( triangle.vertex[1], pointCloud[ triangleFace->vertex[1] ] );
		Copy( triangle.vertex[2], pointCloud[ triangleFace->vertex[2] ] );
		MakePlane( triangle, triangleFace->plane );
		if( Plane::SIDE_FRONT == PlaneSide( triangleFace->plane, vertex ) )
		{
			int vertex = triangleFace->vertex[0];
			triangleFace->vertex[0] = triangleFace->vertex[1];
			triangleFace->vertex[1] = vertex;
			Scale( triangleFace->plane.normal, triangleFace->plane.normal, -1.0 );
		}
		
		Triangle* existingFace = FindTriangle( triangleFace->vertex[0], triangleFace->vertex[1], triangleFace->vertex[2] );
		if( !existingFace )
			triangleList.InsertRightOf( triangleList.RightMost(), triangleFace );
		else
		{
			triangleList.Remove( existingFace, true );
			delete triangleFace;
		}
	}
}

//=============================================================================
void ConvexHull::ExpandHull( Point* point )
{
	Vector vertex;
	Copy( vertex, pointCloud[ point->vertex ] );

	Utilities::List processList;
	triangleList.EmptyIntoOnRight( processList );

	while( processList.Count() > 0 )
	{
		Triangle* triangle = ( Triangle* )processList.LeftMost();
		processList.Remove( triangle, false );
		triangleList.InsertRightOf( triangleList.RightMost(), triangle );

		if( Plane::SIDE_FRONT == PlaneSide( triangle->plane, vertex, 0.0 ) )
		{
			Point* tetrahedron[4];
			tetrahedron[0] = point;
			tetrahedron[1] = FindPoint( triangle->vertex[0] );
			tetrahedron[2] = FindPoint( triangle->vertex[1] );
			tetrahedron[3] = FindPoint( triangle->vertex[2] );
			IntegrateTetrahedron( tetrahedron );
		}
	}
}

//=============================================================================
ConvexHull::Point* ConvexHull::ChoosePoint( void )
{
	Point* point = ( Point* )pointList.LeftMost();
	return point;
}

//=============================================================================
ConvexHull::Triangle* ConvexHull::FindTriangle( int vertex0, int vertex1, int vertex2 )
{
	Triangle* foundTriangle = 0;
	for( Triangle* triangle = ( Triangle* )triangleList.LeftMost(); triangle && !foundTriangle; triangle = ( Triangle* )triangle->Right() )
	{
		for( int index = 0; index < 3; index++ )
		{
			if( ( triangle->vertex[ index ] == vertex0 &&
				triangle->vertex[ ( index + 1 ) % 3 ] == vertex1 &&
				triangle->vertex[ ( index + 2 ) % 3 ] == vertex2 ) ||
				( triangle->vertex[ index ] == vertex0 &&
				triangle->vertex[ ( index + 2 ) % 3 ] == vertex1 &&
				triangle->vertex[ ( index + 1 ) % 3 ] == vertex2 ) )
			{
				foundTriangle = triangle;
			}
		}
	}
	return foundTriangle;
}

//=============================================================================
ConvexHull::Point* ConvexHull::FindPoint( int vertex )
{
	Point* foundPoint = 0;
	for( Point* point = ( Point* )pointList.LeftMost(); point && !foundPoint; point = ( Point* )point->Right() )
		if( point->vertex == vertex )
			foundPoint = point;
	return foundPoint;
}

//=============================================================================
bool ConvexHull::FindFourNonCoplanarPoints( Point** tetrahedron )
{
	CoordFrame coordFrame;
	for( Point* point0 = ( Point* )pointList.LeftMost(); point0; point0 = ( Point* )point0->Right() )
	{
		for( Point* point1 = ( Point* )point0->Right(); point1; point1 = ( Point* )point1->Right() )
		{
			// Make sure that point0 and point1 are unique.
			Sub( coordFrame.yAxis, pointCloud[ point1->vertex ], pointCloud[ point0->vertex ] );
			if( 0.0 != Dot( coordFrame.xAxis, coordFrame.xAxis ) )
			{
				for( Point* point2 = ( Point* )point1->Right(); point2; point2 = ( Point* )point2->Right() )
				{
					// Make sure that point0, point1 and point2 are non-colinear.
					Sub( coordFrame.zAxis, pointCloud[ point2->vertex ], pointCloud[ point0->vertex ] );
					Vector vec;
					Cross( vec, coordFrame.yAxis, coordFrame.zAxis );
					if( 0.0 != Dot( vec, vec ) )
					{
						for( Point* point3 = ( Point* )point2->Right(); point3; point3 = ( Point* )point3->Right() )
						{
							// Make sure that point0, point1, point2 and point3 are non-coplanar.
							Sub( coordFrame.xAxis, pointCloud[ point3->vertex ], pointCloud[ point0->vertex ] );
							double det = 0.0;
							det += coordFrame.xAxis.x * vec.x;
							det -= coordFrame.xAxis.y * vec.y;
							det += coordFrame.xAxis.z * vec.z;
							if( det != 0.0 )
							{
								tetrahedron[0] = point0;
								tetrahedron[1] = point1;
								tetrahedron[2] = point2;
								tetrahedron[3] = point3;
								return true;
							}
						}
					}
				}
			}
		}
	}

	return false;
}

// ConvexHull.cpp