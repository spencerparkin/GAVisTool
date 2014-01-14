// ConvexHull.cpp

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "ConvexHull.h"

using namespace VectorMath;

//=============================================================================
ConvexHull::ConvexHull( void )
{
}

//=============================================================================
ConvexHull::~ConvexHull( void )
{
	pointList.RemoveAll( true );
	triangleList.RemoveAll( true );
	vertexArray.Clear();
}

//=============================================================================
void ConvexHull::AddVertex( double x, double y, double z )
{
	VectorMath::Vector vertex;
	VectorMath::Set( vertex, x, y, z );
	AddVertex( vertex );
}

//=============================================================================
void ConvexHull::AddVertex( const VectorMath::Vector& vertex )
{
	vertexArray.Append( vertex );
}

//=============================================================================
void ConvexHull::RemoveAllVertices( void )
{
	vertexArray.Clear();
}

//=============================================================================
const Utilities::List& ConvexHull::TriangleList( void ) const
{
	return triangleList;
}

//=============================================================================
const VectorMath::Vector& ConvexHull::operator[]( int index ) const
{
	return vertexArray[ index ];
}

//=============================================================================
VectorMath::Vector& ConvexHull::operator[]( int index )
{
	return vertexArray[ index ];
}

//=============================================================================
int ConvexHull::VertexCount( void ) const
{
	return vertexArray.Count();
}

//=============================================================================
bool ConvexHull::RegenerateTriangleList( void )
{
	pointList.RemoveAll( true );
	triangleList.RemoveAll( true );

	// Generate a list of all the points in the point-cloud.
	for( int vertexIndex = 0; vertexIndex < vertexArray.Count(); vertexIndex++ )
	{
		Point* point = new Point();
		point->vertexIndex = vertexIndex;
		pointList.InsertRightOf( pointList.RightMost(), point );
	}

	// We must be able to find an initial convex hull in the form of a tetrahedron.
	int tetrahedron[4];
	if( !FindFourNonCoplanarPoints( tetrahedron ) )
		return false;

	// Begin the convex hull.
	IntegrateTetrahedron( tetrahedron );
	CullPoints();

	// Expand the convex hull until we've exhausted all points exterior to it.
	while( pointList.Count() > 0 )
	{
		ExpandHull( ChoosePoint() );
		CullPoints();
	}

	// We're done!
	return true;
}

//=============================================================================
void ConvexHull::IntegrateTetrahedron( int* tetrahedron )
{
	for( int index = 0; index < 4; index++ )
	{
		Point point;
		point.vertexIndex = tetrahedron[ index ];

		Triangle* triangle = new Triangle();
		triangle->vertexIndex[0] = tetrahedron[ ( index + 1 ) % 4 ];
		triangle->vertexIndex[1] = tetrahedron[ ( index + 2 ) % 4 ];
		triangle->vertexIndex[2] = tetrahedron[ ( index + 3 ) % 4 ];

		if( CalculateTriangleSide( triangle, &point ) == FRONT )
		{
			triangle->vertexIndex[0] ^= triangle->vertexIndex[1];
			triangle->vertexIndex[1] ^= triangle->vertexIndex[0];
			triangle->vertexIndex[0] ^= triangle->vertexIndex[1];
		}

		//ASSERT( CalculateTriangleSide( triangle, &point ) == BACK );

		Triangle* foundTriangle = FindTriangle( triangle );
		if( !foundTriangle )
			triangleList.InsertRightOf( triangleList.RightMost(), triangle );
		else
		{
			triangleList.Remove( foundTriangle, true );
			delete triangle;
		}
	}
}

//=============================================================================
ConvexHull::Triangle* ConvexHull::FindTriangle( const Triangle* triangle )
{
	for( Triangle* foundTriangle = ( Triangle* )triangleList.LeftMost(); foundTriangle; foundTriangle = ( Triangle* )foundTriangle->Right() )
	{
		for( int index = 0; index < 3; index++ )
		{
			if( ( foundTriangle->vertexIndex[ index ] == triangle->vertexIndex[0] &&
				foundTriangle->vertexIndex[ ( index + 1 ) % 3 ] == triangle->vertexIndex[1] &&
				foundTriangle->vertexIndex[ ( index + 2 ) % 3 ] == triangle->vertexIndex[2] ) ||
				( foundTriangle->vertexIndex[ index ] == triangle->vertexIndex[0] &&
				foundTriangle->vertexIndex[ ( index + 2 ) % 3 ] == triangle->vertexIndex[1] &&
				foundTriangle->vertexIndex[ ( index + 1 ) % 3 ] == triangle->vertexIndex[2] ) )
			{
				return foundTriangle;
			}
		}
	}
	return 0;
}

//=============================================================================
void ConvexHull::ExpandHull( const Point* point )
{
	Utilities::List processList;
	triangleList.EmptyIntoOnRight( processList );

	while( processList.Count() > 0 )
	{
		Triangle* triangle = ( Triangle* )processList.LeftMost();
		processList.Remove( triangle );
		triangleList.InsertRightOf( triangleList.RightMost(), triangle );

		// If we're not careful enough to avoid degenerate tetrahedrons here, we may
		// accidently get a triangle on the hull with its normal facing the wrong way.
		if( CalculateTriangleSide( triangle, point, 1e-3 ) == FRONT )
		{
			int tetrahedron[4];
			tetrahedron[0] = triangle->vertexIndex[0];
			tetrahedron[1] = triangle->vertexIndex[1];
			tetrahedron[2] = triangle->vertexIndex[2];
			tetrahedron[3] = point->vertexIndex;
			//ASSERT( !IsDegenerate( tetrahedron ) );
			IntegrateTetrahedron( tetrahedron );
		}
	}
}

//=============================================================================
bool ConvexHull::IsDegenerate( int* tetrahedron )
{
	for( int i = 0; i < 4; i++ )
		for( int j = i + 1; j < 4; j++ )
			if( tetrahedron[i] == tetrahedron[j] )
				return true;
	return false;
}

//=============================================================================
const ConvexHull::Point* ConvexHull::ChoosePoint( void )
{
	return ( const Point* )pointList.LeftMost();
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
bool ConvexHull::PointIsOnOrInsideHull( const Point* point )
{
	for( Triangle* triangle = ( Triangle* )triangleList.LeftMost(); triangle; triangle = ( Triangle* )triangle->Right() )
		if( CalculateTriangleSide( triangle, point, 1e-3 ) == FRONT )
			return false;
	return true;
}

//=============================================================================
ConvexHull::TriangleSide ConvexHull::CalculateTriangleSide( const Triangle* triangle, const Point* point, double epsilon /*= 1e-7*/ )
{
	VectorMath::Vector vertex0, vertex1, vertex2, vertex;

	VectorMath::Copy( vertex0, vertexArray[ triangle->vertexIndex[0] ] );
	VectorMath::Copy( vertex1, vertexArray[ triangle->vertexIndex[1] ] );
	VectorMath::Copy( vertex2, vertexArray[ triangle->vertexIndex[2] ] );
	VectorMath::Copy( vertex, vertexArray[ point->vertexIndex ] );

	VectorMath::Vector edge0, edge1, edge2;
	VectorMath::Sub( edge0, vertex1, vertex0 );
	VectorMath::Sub( edge1, vertex2, vertex0 );
	VectorMath::Sub( edge2, vertex, vertex0 );

	VectorMath::Vector cross;
	VectorMath::Cross( cross, edge0, edge1 );
	double dot = VectorMath::Dot( cross, edge2 );
	if( dot < epsilon )
		return BACK;
	if( dot > epsilon )
		return FRONT;
	return NEITHER;
}

//=============================================================================
bool ConvexHull::FindFourNonCoplanarPoints( int* tetrahedron )
{
	for( tetrahedron[0] = 0; tetrahedron[0] < vertexArray.Count(); tetrahedron[0]++ )
		for( tetrahedron[1] = tetrahedron[0] + 1; tetrahedron[1] < vertexArray.Count(); tetrahedron[1]++ )
			for( tetrahedron[2] = tetrahedron[1] + 1; tetrahedron[2] < vertexArray.Count(); tetrahedron[2]++ )
				for( tetrahedron[3] = tetrahedron[2] + 1; tetrahedron[3] < vertexArray.Count(); tetrahedron[3]++ )
					if( TetrahedronVolume( tetrahedron ) > 0.f )
						return true;
	return false;
}

//=============================================================================
double ConvexHull::TetrahedronVolume( int* tetrahedron )
{
	VectorMath::Vector vertex0, vertex1, vertex2, vertex3;

	VectorMath::Copy( vertex0, vertexArray[ tetrahedron[0] ] );
	VectorMath::Copy( vertex1, vertexArray[ tetrahedron[1] ] );
	VectorMath::Copy( vertex2, vertexArray[ tetrahedron[2] ] );
	VectorMath::Copy( vertex3, vertexArray[ tetrahedron[3] ] );

	VectorMath::Vector edge0, edge1, edge2;

	VectorMath::Sub( edge0, vertex0, vertex1 );
	VectorMath::Sub( edge1, vertex0, vertex2 );
	VectorMath::Sub( edge2, vertex0, vertex3 );

	VectorMath::Vector cross;
	VectorMath::Cross( cross, edge0, edge1 );
	double dot = VectorMath::Dot( cross, edge2 );
	return fabs( dot ) / 6.f;
}

// ConvexHull.cpp