// SurfaceMesh.cpp

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "SurfaceMesh.h"

using namespace VectorMath;

//=============================================================================
SurfaceMesh::SurfaceMesh( void )
{
}

//=============================================================================
/*virtual*/ SurfaceMesh::~SurfaceMesh( void )
{
	componentList.RemoveAll( true );
}

//=============================================================================
// Try to generate all the path connected components of the surface.
bool SurfaceMesh::Generate( const Surface& surface, const GenerationParameters& genParms )
{
	// Wipe out any existing components.  We're going to regenerate them.
	componentList.RemoveAll( true );

	// This is not a very scientific approach to finding all components of the given surface.
	// What we're doing is just trying a bunch of different seed points in the hope that each
	// one converges a new component of the surface.
	Vector seedList[2];
	//const int seedListSize = sizeof( seedList ) / sizeof( Vector );
	const int seedListSize = 1;
	Copy( seedList[0], genParms.aabb.min );
	Copy( seedList[1], genParms.aabb.max );

	// Go try all the seeds.
	for( int index = 0; index < seedListSize; index++ )
	{
		// Grab the next seed.
		Copy( genParms.seed, seedList[ index ] );

		// If it does not converge to a point on the surface, continue.
		if( !surface.ConvergePointToSurfaceInPlane( 0, genParms.seed, genParms.epsilon ) )
			continue;

		// If the converged point exists on a component we have already generated, continue.
		PathConnectedComponent* component = ( PathConnectedComponent* )componentList.LeftMost();
		while( component )
		{
			if( component->IsPointOnSurface( genParms.seed, genParms.epsilon ) )
				break;
			component = ( PathConnectedComponent* )component->Right();
		}
		if( component )
			continue;

		// Try to generate the component of the surface containing the converged point.
		PathConnectedComponentGenerator generator;
		component = generator.Generate( surface, genParms );
		if( component )
			componentList.InsertRightOf( componentList.RightMost(), component );
	}

	return true;
}

//=============================================================================
SurfaceMesh::PathConnectedComponent::PathConnectedComponent( void )
{
}

//=============================================================================
/*virtual*/ SurfaceMesh::PathConnectedComponent::~PathConnectedComponent( void )
{
	if( triangleList )
	{
		delete[] triangleList;
		triangleList = 0;
		triangleListSize = 0;
	}
}

//=============================================================================
bool SurfaceMesh::PathConnectedComponent::IsPointOnSurface( const Vector& point, double epsilon ) const
{
	for( int index = 0; index < triangleListSize; index++ )
	{
		VectorMath::Triangle& triangle = triangleList[ index ];
		Plane trianglePlane;
		MakePlane( triangle, trianglePlane );

		// Trying to account for the potential curvature of the surface, we can tolerate
		// the given point being some distance off the plane of the triangle.
		double orthoDist = ShortestDistanceToPlane( trianglePlane, point );
		if( orthoDist <= epsilon )
		{
			Vector projPoint;
			ProjectPointOntoPlane( trianglePlane, projPoint, point );
			if( IsPointInsideTriangle( triangle, trianglePlane, projPoint ) )
				return true;
		}
	}

	return false;
}

//=============================================================================
SurfaceMesh::PathConnectedComponentGenerator::PathConnectedComponentGenerator( void )
{
}

//=============================================================================
/*virtual*/ SurfaceMesh::PathConnectedComponentGenerator::~PathConnectedComponentGenerator( void )
{
	DeleteLists();
}

//=============================================================================
void SurfaceMesh::PathConnectedComponentGenerator::DeleteLists( void )
{
	growingPointList.RemoveAll( true );
	maturePointList.RemoveAll( true );
	stuntedPointList.RemoveAll( true );
	staticEdgeList.RemoveAll( true );
	dynamicEdgeList.RemoveAll( true );
	triangleList.RemoveAll( true );
}

//=============================================================================
bool SurfaceMesh::PathConnectedComponentGenerator::GenerateInitialTriangle(
											const Surface& surface,
											const GenerationParameters& genParms )
{
	VectorMath::Triangle triangleGeometry;

	Copy( triangleGeometry.vertex[0], genParms.seed );
	if( !surface.ConvergePointToSurfaceInPlane( 0, triangleGeometry.vertex[0], genParms.epsilon ) )
		return false;

	CoordFrame coordFrame;
	surface.EvaluateGradientAt( triangleGeometry.vertex[0], coordFrame.zAxis );
	Normalize( coordFrame.zAxis, coordFrame.zAxis );
	Orthogonal( coordFrame.yAxis, coordFrame.zAxis );
	Cross( coordFrame.xAxis, coordFrame.yAxis, coordFrame.zAxis );

	double scale = genParms.edgeLengthThreshold / sqrt( 2.0 );
	Scale( coordFrame.xAxis, coordFrame.xAxis, scale );
	Scale( coordFrame.yAxis, coordFrame.yAxis, scale );
	Add( triangleGeometry.vertex[1], triangleGeometry.vertex[0], coordFrame.xAxis );
	Add( triangleGeometry.vertex[2], triangleGeometry.vertex[0], coordFrame.yAxis );

	if( !surface.ConvergePointToSurfaceInPlane( 0, triangleGeometry.vertex[1], genParms.epsilon ) )
		return false;
	if( !surface.ConvergePointToSurfaceInPlane( 0, triangleGeometry.vertex[2], genParms.epsilon ) )
		return false;

	Vertex* vertex[3];
	vertex[0] = new Vertex( triangleGeometry.vertex[0] );
	vertex[1] = new Vertex( triangleGeometry.vertex[1] );
	vertex[2] = new Vertex( triangleGeometry.vertex[2] );

	growingPointList.InsertRightOf( growingPointList.RightMost(), vertex[0] );
	growingPointList.InsertRightOf( growingPointList.RightMost(), vertex[1] );
	growingPointList.InsertRightOf( growingPointList.RightMost(), vertex[2] );

	vertex[0]->adjacentVertex[0] = vertex[1];
	vertex[0]->adjacentVertex[1] = vertex[2];
	vertex[0]->degree = 2;

	vertex[1]->adjacentVertex[0] = vertex[0];
	vertex[1]->adjacentVertex[1] = vertex[2];
	vertex[1]->degree = 2;

	vertex[2]->adjacentVertex[0] = vertex[0];
	vertex[2]->adjacentVertex[1] = vertex[1];
	vertex[2]->degree = 2;

	Triangle* triangle = new Triangle( vertex[0], vertex[1], vertex[2] );
	triangleList.InsertRightOf( triangleList.RightMost(), triangle );

	Edge* edge[3];
	edge[0] = new Edge( vertex[0], vertex[1] );
	edge[1] = new Edge( vertex[1], vertex[2] );
	edge[2] = new Edge( vertex[2], vertex[0] );

	dynamicEdgeList.InsertRightOf( dynamicEdgeList.RightMost(), edge[0] );
	dynamicEdgeList.InsertRightOf( dynamicEdgeList.RightMost(), edge[1] );
	dynamicEdgeList.InsertRightOf( dynamicEdgeList.RightMost(), edge[2] );

	edge[0]->side[Edge::FRONT] = new Edge::Side();
	edge[0]->side[Edge::FRONT]->triangle = triangle;
	edge[0]->side[Edge::FRONT]->vertex = vertex[2];
	edge[0]->side[Edge::FRONT]->edge[Edge::Side::LEFT] = edge[2];
	edge[0]->side[Edge::FRONT]->edge[Edge::Side::RIGHT] = edge[1];

	edge[1]->side[Edge::FRONT] = new Edge::Side();
	edge[1]->side[Edge::FRONT]->triangle = triangle;
	edge[1]->side[Edge::FRONT]->vertex = vertex[0];
	edge[1]->side[Edge::FRONT]->edge[Edge::Side::LEFT] = edge[0];
	edge[1]->side[Edge::FRONT]->edge[Edge::Side::RIGHT] = edge[2];

	edge[2]->side[Edge::FRONT] = new Edge::Side();
	edge[2]->side[Edge::FRONT]->triangle = triangle;
	edge[2]->side[Edge::FRONT]->vertex = vertex[1];
	edge[2]->side[Edge::FRONT]->edge[Edge::Side::LEFT] = edge[1];
	edge[2]->side[Edge::FRONT]->edge[Edge::Side::RIGHT] = edge[0];

	return true;
}

//=============================================================================
SurfaceMesh::PathConnectedComponent* SurfaceMesh::PathConnectedComponentGenerator::Generate(
											const Surface& surface,
											const GenerationParameters& genParms )
{
	DeleteLists();

	if( !GenerateInitialTriangle( surface, genParms ) )
		return 0;

	while( growingPointList.Count() > 0 )
	{
		DetermineStuntedPoints( genParms );

		Edge* edge = FindEdgeAtCriticalLength( genParms );
		if( edge ) break;		// Hack for now!
		while( edge )
		{
			SplitEdge( edge, surface );
			edge = FindEdgeAtCriticalLength( genParms );
		}

		CalculateGrowDirections( genParms );
		GrowPoints( surface, genParms );
	}

	// TODO: How do we handle the stunted points?

	PathConnectedComponent* component = BuildComponent();
	return component;
}

//=============================================================================
void SurfaceMesh::PathConnectedComponentGenerator::CalculateGrowDirections( const GenerationParameters& genParms )
{
	for( Vertex* vertex = ( Vertex* )growingPointList.LeftMost(); vertex; vertex = ( Vertex* )vertex->Right() )
		Zero( vertex->growDirection );

	for( Edge* edge = ( Edge* )dynamicEdgeList.LeftMost(); edge; edge = ( Edge* )edge->Right() )
	{
		Vector growDirection;
		Sub( growDirection, edge->vertex[0]->point, edge->vertex[1]->point );
		double length = genParms.edgeLengthThreshold - Length( growDirection );		// This should always come out non-negative.
		if( length < genParms.epsilon )
			length = genParms.epsilon;
		Normalize( growDirection, growDirection );
		Scale( growDirection, growDirection, length );

		// One of these might be a mature or stunted point, but it doesn't matter.
		Add( edge->vertex[0]->growDirection, edge->vertex[0]->growDirection, growDirection );
		Sub( edge->vertex[1]->growDirection, edge->vertex[1]->growDirection, growDirection );
	}
}

//=============================================================================
void SurfaceMesh::PathConnectedComponentGenerator::GrowPoints( const Surface& surface, const GenerationParameters& genParms )
{
	for( Vertex* vertex = ( Vertex* )growingPointList.LeftMost(); vertex; vertex = ( Vertex* )vertex->Right() )
	{
		Normalize( vertex->growDirection, vertex->growDirection );
		Scale( vertex->growDirection, vertex->growDirection, genParms.growLength );
		Add( vertex->point, vertex->point, vertex->growDirection );

		surface.ConvergePointToSurfaceInPlane( 0, vertex->point, genParms.epsilon );
	}
}

//=============================================================================
SurfaceMesh::PathConnectedComponentGenerator::Edge* SurfaceMesh::PathConnectedComponentGenerator::FindEdgeAtCriticalLength( const GenerationParameters& genParms )
{
	Edge* edge = ( Edge* )dynamicEdgeList.LeftMost();
	while( edge )
	{
		double length = edge->Length();
		if( length >= genParms.edgeLengthThreshold )
			break;
		edge = ( Edge* )edge->Right();
	}
	return edge;
}

//=============================================================================
void SurfaceMesh::PathConnectedComponentGenerator::SplitEdge( Edge* edge, const Surface& surface )
{
}

//=============================================================================
// Move a point from the growing point list to the stunted point list if it
// is determined that it can no longer grow along the surface.  This is the
// case when a growing point comes into close-enough proximity to a point in
// the mesh to which it is not adjacent.  This algorithm is O(N^2), but here,
// N is not the number of vertices in the entire mesh, but only the number of
// vertices in the mesh that are currently growing.
void SurfaceMesh::PathConnectedComponentGenerator::DetermineStuntedPoints( const GenerationParameters& genParms )
{
	Vertex* next = 0;
	for( Vertex* vertex = ( Vertex* )growingPointList.LeftMost(); vertex; vertex = next )
	{
		next = ( Vertex* )vertex->Right();
		if( PointIsStunted( vertex, genParms ) )
		{
			growingPointList.Remove( vertex, false );
			stuntedPointList.InsertRightOf( stuntedPointList.RightMost(), vertex );
		}
	}
}

//=============================================================================
bool SurfaceMesh::PathConnectedComponentGenerator::PointIsStunted( Vertex* testVertex, const GenerationParameters& genParms )
{
	for( Vertex* vertex = ( Vertex* )growingPointList.LeftMost(); vertex; vertex = ( Vertex* )vertex->Right() )
	{
		if( vertex != testVertex )
		{
			double distance = Distance( testVertex->point, vertex->point );
			if( distance <= genParms.stuntedDistanceThreshold )
				if( !testVertex->IsAdjacentTo( *vertex ) )
					return true;
		}
	}
	return false;
}

//=============================================================================
SurfaceMesh::PathConnectedComponent* SurfaceMesh::PathConnectedComponentGenerator::BuildComponent( void )
{
	SurfaceMesh::PathConnectedComponent* component = new SurfaceMesh::PathConnectedComponent();
	component->triangleListSize = triangleList.Count();
	component->triangleList = new VectorMath::Triangle[ component->triangleListSize ];

	int index = 0;
	for( Triangle* triangle = ( Triangle* )triangleList.LeftMost(); triangle; triangle = ( Triangle* )triangle->Right() )
	{
		VectorMath::Triangle* triangleGeometry = &component->triangleList[ index++ ];
		MakeTriangle( *triangleGeometry,
							triangle->vertex[0]->point,
							triangle->vertex[1]->point,
							triangle->vertex[2]->point );
	}

	return component;
}

//=============================================================================
SurfaceMesh::PathConnectedComponentGenerator::Vertex::Vertex( const Vector& point )
{
	Copy( this->point, point );
	for( int index = 0; index < MAX_DEGREE; index++ )
		adjacentVertex[ index ] = 0;
	degree = 0;
}

//=============================================================================
/*virtual*/ SurfaceMesh::PathConnectedComponentGenerator::Vertex::~Vertex( void )
{
}

//=============================================================================
bool SurfaceMesh::PathConnectedComponentGenerator::Vertex::IsAdjacentTo( const Vertex& vertex ) const
{
	for( int index = 0; index < degree; index++ )
		if( adjacentVertex[ index ] == &vertex )
			return true;
	return false;
}

//=============================================================================
SurfaceMesh::PathConnectedComponentGenerator::Edge::Edge( Vertex* vertex0, Vertex* vertex1 )
{
	vertex[0] = vertex0;
	vertex[1] = vertex1;
	side[0] = 0;
	side[1] = 0;
}

//=============================================================================
SurfaceMesh::PathConnectedComponentGenerator::Edge::~Edge( void )
{
	if( side[0] )
		delete side[0];
	if( side[1] )
		delete side[1];
}

//=============================================================================
double SurfaceMesh::PathConnectedComponentGenerator::Edge::Length( void ) const
{
	return Distance( vertex[0]->point, vertex[1]->point );
}

//=============================================================================
SurfaceMesh::PathConnectedComponentGenerator::Triangle::Triangle( Vertex* vertex0, Vertex* vertex1, Vertex* vertex2 )
{
	vertex[0] = vertex0;
	vertex[1] = vertex1;
	vertex[2] = vertex2;
}

//=============================================================================
SurfaceMesh::PathConnectedComponentGenerator::Triangle::~Triangle( void )
{
}

//=============================================================================
SurfaceMesh::GenerationParameters::GenerationParameters( void )
{
	Zero( seed );
	epsilon = 1e-7;
	edgeLengthThreshold = 3.0; //1.5;
	stuntedDistanceThreshold = 0.2;
	growLength = 0.1;
	Set( aabb.min, -10.0, -10.0, -10.0 );
	Set( aabb.max, 10.0, 10.0, 10.0 );
}

//=============================================================================
/*virtual*/ SurfaceMesh::GenerationParameters::~GenerationParameters( void )
{
}

// SurfaceMesh.cpp