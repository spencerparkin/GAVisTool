// GiftWrapper.cpp

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "GiftWrapper.h"
#include "Assert.h"

using namespace VectorMath;

//=============================================================================
GiftWrapper::GiftWrapper( void )
{
	neightborhoodRadius = 0.0;
}

//=============================================================================
/*virtual*/ GiftWrapper::~GiftWrapper( void )
{
	pointList.RemoveAll( true );
	edgeList.RemoveAll( true );
	triangleList.RemoveAll( true );
}

//=============================================================================
void GiftWrapper::AddPoint( const Vector& point )
{
	pointList.InsertRightOf( pointList.RightMost(), new Point( point ) );
}

//=============================================================================
void GiftWrapper::RemoveAllPoints( void )
{
	pointList.RemoveAll( true );
}

//=============================================================================
void GiftWrapper::SetNeighborhoodRadius( double radius )
{
	neighborhoodRadius = radius;
}

//=============================================================================
double GiftWrapper::GetNeighborhoodRadius( void ) const
{
	return neighborhoodRadius;
}

//=============================================================================
GiftWrapper::Hull::Hull( void )
{
	triangleList = 0;
	triangleListSize = 0;
}

//=============================================================================
/*virtual*/ GiftWrapper::Hull::~Hull( void )
{
	delete[] triangleList;
}

//=============================================================================
VectorMath::Triangle& GiftWrapper::Hull::operator[]( int index )
{
	return triangleList[ index ];
}

//=============================================================================
int GiftWrapper::Hull::Count( void ) const
{
	return triangleListSize;
}

//=============================================================================
GiftWrapper::Hull* GiftWrapper::GenerateHull( void )
{
	edgeList.RemoveAll( true );
	triangleList.RemoveAll( true );

	Triangle* triangle = GenerateInitialTriangle();
	if( !triangle )
		return 0;

	triangleList.InsertRightOf( triangleList.RightMost(), triangle );
	CullPointsInTriangle( triangle );
	Edge* edges[3];
	CreateTriangleEdges( triangle, edges );
	ExpandEdgeLoop( edges );

	// Our algorithm now terminates when there are no more edges to process.
	while( edgeList.Count() > 0 )
	{
		Edge* edge = ( Edge* )edgeList.LeftMost();
		triangle = GenerateNewTriangleFromEdge( edge );
		if( !triangle )
			edgeList.Remove( edge, true );
		else
		{
			triangleList.InsertRightOf( triangleList.RightMost(), triangle );
			CullPointsInTriangle( triangle );
			CreateTriangleEdges( triangle, edges );
			ExpandEdgeLoop( edges );
		}
	}

	return 0;
}

//=============================================================================
GiftWrapper::Triangle* GiftWrapper::GenerateInitialTriangle( void )
{
	// this is tricky if we're given a neighborhood radius.

	return 0;
}

//=============================================================================
GiftWrapper::Traingle* GiftWrapper::GenerateNewTriangleFromEdge( Edge* edge )
{
	// Float a plane hinging on the given edge to the surface of the hull.
	Point* point = PickPointNotOnLineOfEdge( edge );
	VectorMath::Triangle triangleGeometry;
	MakeTriangle( triangleGeometry, edge->vertex[0]->point, edge->vertex[1]->point, point->point );
	Plane plane;
	MakePlane( triangleGeometry, plane );
	Utilities::List pointsOnPlane, pointOnPlaneFront;
	DistributePointsAboutPlane( plane, pointList, pointsOnPlane, pointsOnPlaneFront, pointsList );
	while( pointsOnPlaneFront.Count() > 0 )
	{
		point = FindHighestPointAbovePlane( plane, pointsOnPlaneFront );
		pointsOnPlane.EmptyIntoOnRight( pointList );
		pointsOnPlaneFront.EmptyIntoOnRight( pointList );
		MakeTriangle( triangleGeometry, edge->vertex[0]->point, edge->vertex[1]->point, point->point );
		MakePlane( triangleGeometry, plane );
		DistributePointsAboutPlane( plane, pointList, pointsOnPlane, pointsOnPlaneFront, pointsList );
	}

	// Among the points on the floated plane, find the one that we'll use to complete the new triangle.
	Vector edgeNorm, edgeVec;
	Sub( edgeVec, edge->vertex[1], edge->vertex[0] );
	Cross( edgeNorm, plane.normal, edgeVec );
	Point* adjacentPoints[2];
	Vector adjacentPointVectors[2];
	FindAdjacentEdgePoints( edge, adjacentPoints, adjacentPointVectors );
	Point* newPoint = 0;
	if( Plane::SIDE_NEITHER == PlaneSide( plane, adjacentPoints[0]->point ) &&
			AngleBetween( edgeNorm, adjacentPointVectors[0] ) < PI )
	{
		newPoint = adjacentPoints[0];
	}
	else if( Plane::SIDE_NEITHER == PlaneSide( plane, adjacentPoints[1]->point ) &&
			AngleBetween( edgeNorm, adjacentPointVectors[1] ) < PI )
	{
		newPoint = adjacentPoints[1];
	}
	else
	{
		Vector center;
		edge->CalcCenter( center );
		Normalize( edgeNorm, edgeNorm );
		double maxDistance = 0.0;
		for( Point* point = ( Point* )pointsOnPlane.LeftMost(); point; point = ( Point* )point->Right() )
		{
			Vector pointVec;
			Sub( pointVec, point->point, center );
			double distance = Dot( pointVec, edgeNorm );
			if( distance > maxDistance && ( neighorhoodRadius == 0.0 || distance <= neighborhoodRadius ) )
			{
				maxDistance = distance;
				newPoint = point;
			}
		}
	}

	// Don't forget to put these points back on our point cloud list.
	pointsOnPlane.EmptyIntoOnRight( pointList );

	// Create and return the newly found triangle, if any.
	Triangle* triangle = 0;
	if( newPoint )
		triangle = new Triangle( edge[0], edge[1], newPoint );
	return triangle;
}

//=============================================================================
void GiftWrapper::DistributePointsAboutPlane( const Plane& plane,
												Utilities::List& pointsOnPlaneBack,
												Utilities::List& pointsOnPlane,
												Utilities::List& pointsOnPlaneFront,
												Utilities::List& pointList )
{
	while( pointList.Count() > 0 )
	{
		Point* point = ( Point* )pointList.LeftMost();
		pointList.Remove( point, false );
		switch( PlaneSide( plane, point->point ) )
		{
			case Plane::SIDE_BACK:
			{
				pointsOnPlaneBack.InsertRightOf( pointsOnPlaneBack.RightMost(), point );
				break;
			}
			case Plane::SIDE_NEITHER:
			{
				pointsOnPlane.InsertRightOf( pointsOnPlane.RightMost(), point );
				break;
			}
			case Plane::SIDE_FRONT:
			{
				pointsOnPlaneFront.InsertRightOf( pointsOnPlaneFront.RightMost(), point );
				break;
			}
		}
	}
}

//=============================================================================
void GiftWrapper::FindAdjacentEdgePoints( Edge* edge, Point** adjacentPoints, Vector* adjacentPointVectors )
{
	int index = 0;
	for( Edge* adjEdge = ( Edge* )edgeList.LeftMost(); adjEdge && index < 2; adjEdge = ( Edge* )adjEdge->Right() )
	{
		if( adjEdge->IsAdjacentTo( *edge ) )
		{
			adjacentPoints[ index ] = adjEdge->PointAdjacentTo( *edge );
			Point* otherPoint = adjEdge->OtherPoint( adjacentPoints[ index ] );
			Sub( adjacentPointVectors[ index ], adjacentPoints[ index ]->point, otherPoint->point );
			index++;
		}
	}

	// If "index" is not 2 at this point, then something is wrong with the algorithm.
	assert->Condition( index == 2, "FindAdjacentEdgePoints -- Did not find both edges." );
}

//=============================================================================
// How we create the edges from the given triangle is important
// to maintaining proper triangle winding order for future triangles.
// A triangle created from an edge uses edge vertices 0 and 1 as
// its first 2 vertices.  The last vertex, vertex 2, is found
// elsewhere in the cloud.
void GiftWrapper::CreateTriangleEdges( Triangle* triangle, Edge** edges )
{
	edges[0] = new Edge( triangle->vertex[1], triangle->vertex[0] );
	edges[2] = new Edge( triangle->vertex[2], triangle->vertex[1] );
	edges[3] = new Edge( triangle->vertex[0], triangle->vertex[2] );
}

//=============================================================================
// A newly created triangle will share one or more edges with the existing
// edge list.  One edge is always shared.  Two edges are shared in the
// case that the algorithm decides to make the edge loop more convex, and
// three edges are shared in the case that the last triangle of the hull is
// found, in which case the edge list is finally emptied.  We do not attempt
// to keep the edge list ordered so that edges adjacent in the list are also
// geometrically adjacent.  This is to keep the algorithm simpler.  Though if
// we did maintain such an order, taking advantage of that order would
// allow for an optimization of our algorithm.  Specifically, we wouldn't have
// to search for a given edge's adjacent edge.
void GiftWrapper::ExpandEdgeLoop( Edge** edges )
{
	for( int index = 0; index < 3; index++ )
	{
		Edge* foundEdge = FindMatchingEdge( edges[ index ] );
		if( !foundEdge )
			edgeList.InsertRightOf( edgeList.RightMost(), edges[ index ] );
		else
		{
			delete edges[ index ];
			edgeList.Remove( foundEdge, true );
		}
		edges[ index ] = 0;
	}
}

//=============================================================================
GiftWrapper::Edge* GiftWrapper::FindMatchingEdge( Edge* newEdge )
{
	Edge* edge = ( Edge* )edgeList.LeftMost();
	while( edge )
	{
		if( edge->MatchesNewEdge( *newEdge ) )
			break;
		edge = ( Edge* )edge->Right();
	}
	return edge;
}

//=============================================================================
bool GiftWrapper::MatchesNewEdge( const Edge& newEdge ) const
{
	// When comparing the two edges to see if they're the same
	// edge, any newly created edge will always be in the reverse
	// order of its identical edge, if any, in the current edge list.
	if( newEdge.vertex[0] == vertex[1] &&
		newEdge.vertex[1] == vertex[0] )
	{
		return true;
	}
	return false;
}

//=============================================================================
// This routine shouldn't be needed for the correctness of our algorithm.
// It may not even be that much of an optimization.  In any case, if a
// a new triangle is added to the hull, then there is no need to consider
// any points in the cloud that are on the triangle.
void GiftWrapper::CullPointsInTriangle( Triangle* triangle )
{
	VectorMath::Triangle triangleGeometry;
	MakeTriangle( triangleGeometry,
						triangle->vertex[0]->point,
						triangle->vertex[1]->point,
						triangle->vertex[2]->point );

	Point* nextPoint = 0;
	for( Point* point = ( Point* )pointList; point; point = nextPoint )
	{
		nextPoint = ( Point* )point->Right();
		if( IsPointInsideTriangle( triangleGeometry, point->point ) )
			pointList.Remove( point, true );
	}
}

//=============================================================================
GiftWrapper::Point::Point( const Vector& point )
{
	Copy( this->point, point );
}

//=============================================================================
GiftWrapper::Triangle::Triangle( Point* vertex0, Point* vertex1, Point* vertex2 )
{
	vertex[0] = vertex0;
	vertex[1] = vertex1;
	vertex[2] = vertex2;
}

//=============================================================================
GiftWrapper::Edge::Edge( Point* vertex0, Point* vertex1 )
{
	vertex[0] = vertex0;
	vertex[1] = vertex1];
}

//=============================================================================
bool GiftWrapper::Edge::IsAdjacentTo( const Edge& edge ) const
{
	bool match00 = vertex[0] == edge.vertex[0];
	bool match01 = vertex[0] == edge.vertex[1];
	bool match10 = vertex[1] == edge.vertex[0];
	bool match11 = vertex[1] == edge.vertex[1];

	if( match00 && !match11 )
		return true;
	if( match01 && !match10 )
		return true;
	if( match10 && !match01 )
		return true;
	if( match11 && !match00 )
		return true;

	return false;
}

//=============================================================================
GiftWrapper::Point* GiftWrapper::Edge::PointAdjacentTo( const Edge& edge ) const
{
	if( vertex[0] == edge.vertex[0] || vertex[0] == edge.vertex[1] )
		return vertex[1];
	if( vertex[1] == edge.vertex[0] || vertex[1] == edge.vertex[1] )
		return vertex[0];
	return 0;
}

//=============================================================================
GiftWrapper::Point* GiftWrapper::Edge::OtherPoint( const Point* point ) const
{
	if( vertex[0] == point )
		return vertex[1];
	if( vertex[1] == point )
		return vertex[0];
	return 0;
}

//=============================================================================
void GiftWrapper::Edge::CalcCenter( Vector& center )
{
	Lerp( center, vertex[0]->point, vertex[1]->point, 0.5 );
}

// GiftWrapper.cpp