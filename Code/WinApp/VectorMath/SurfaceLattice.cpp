// SurfaceLattice.cpp

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "SurfaceLattice.h"
#include "Assert.h"

using namespace VectorMath;

//=============================================================================
SurfaceLattice::SurfaceLattice( void )
{
}

//=============================================================================
/*virtual*/ SurfaceLattice::~SurfaceLattice( void )
{
	DeleteAllLists();
}

//=============================================================================
void SurfaceLattice::DeleteAllLists( void )
{
	vertexList.RemoveAll( true );
	edgeList.RemoveAll( true );
	triangleList.RemoveAll( true );
}

//=============================================================================
SurfaceLattice::GenerationParameters::GenerationParameters( void )
{
	latticeWalkLength = 1.5;
	epsilon = 1e-7;
	Vector center, delta;
	Set( center, 0.0, 0.0, 0.0 );
	Set( delta, 10.0, 10.0, 10.0 );
	MakeAabb( aabb, center, delta );
	Zero( seed );
	directionCount = 4;
	maximumIterationCount = 30;		// Small for now...
}

//=============================================================================
bool SurfaceLattice::Generate( const Surface& surface, const GenerationParameters& genParms )
{
	bool success = true;

	if( !GenerateLattice( surface, genParms ) )
		success = false;

	if( !GenerateEdges( genParms ) )
		success = false;

	if( !GenerateMesh( genParms ) )
		success = false;

	return success;
}

//=============================================================================
void SurfaceLattice::DrawAsDebugLattice( SurfaceLatticeDrawCallback& drawCallback )
{
}

//=============================================================================
void SurfaceLattice::DrawAsWireFrame( SurfaceLatticeDrawCallback& drawCallback )
{
	for( Edge* edge = ( Edge* )edgeList.LeftMost(); edge; edge = ( Edge* )edge->Right() )
		drawCallback.DrawLine( edge->vertex[0]->point, edge->vertex[1]->point );
}

//=============================================================================
void SurfaceLattice::DrawAsTriangleMesh( SurfaceLatticeDrawCallback& drawCallback )
{
	for( Triangle* triangle = ( Triangle* )triangleList.LeftMost(); triangle; triangle = ( Triangle* )triangle->Right() )
		drawCallback.DrawTriangle( triangle->vertex[0]->point, triangle->vertex[1]->point, triangle->vertex[2]->point );
}

//=============================================================================
// The hope here is that the resulting lattice will have the same topology
// as the path-connected component of the given surface that we land on.
bool SurfaceLattice::GenerateLattice( const Surface& surface, const GenerationParameters& genParms )
{
	// Wipe out any existing list data.  We are going to regenerate it.
	DeleteAllLists();

	// Find a place to start on the given surface.
	Vector point;
	Copy( point, genParms.seed );
	if( !surface.ConvergePointToSurfaceInPlane( 0, point, genParms.epsilon ) )
		return false;

	// Create the initial vertex on the surface.
	Vertex* vertex = new Vertex( point, genParms.directionCount );
	vertex->CalculateTangentDirections( surface, genParms, 0 );

	// Enqueue the initial vertex.
	Utilities::List vertexQueue;
	vertexQueue.InsertRightOf( vertexQueue.RightMost(), vertex );

	// Our algorithm terminates when the vertex queue becomes empty.
	int iterationCount = 0;
	while( vertexQueue.Count() > 0 )
	{
		// Dequeue the next vertex to process.
		vertex = ( Vertex* )vertexQueue.LeftMost();
		vertexQueue.Remove( vertex, false );

		// Process the vertex.
		if( !ExploreSurface( surface, genParms, vertex, vertexQueue ) )
			break;

		// To insure that there's no chance that we loop forever, check
		// out iteration count against the maximum iterations count aloud.
		iterationCount++;
		assert->Condition( iterationCount <= genParms.maximumIterationCount, "Surface lattice algorithm blew the iteration limit of %d iterations.", genParms.maximumIterationCount );
		if( iterationCount > genParms.maximumIterationCount )
			break;
	}

	// If our algorithm terminated prematurely, then something went wrong.
	if( vertexQueue.Count() > 0 )
	{
		// Dump the queue into the explored list.  This allows us
		// to visualize partially generated lattice results.  We also
		// must do this or delete all lists, because some explored
		// vertices may point to unexplored vertices.
		vertexQueue.EmptyIntoOnRight( vertexList );
		return false;
	}

	// Return success!
	return true;
}

//=============================================================================
// Here we explore the given surface at the given vertex.
bool SurfaceLattice::ExploreSurface( const Surface& surface, const GenerationParameters& genParms, Vertex* vertex, Utilities::List& vertexQueue )
{
	// The first thing we do is mark the given vertex as
	// explored by putting it into or list of explored vertices.
	vertexList.InsertRightOf( vertexList.RightMost(), vertex );

	// Explore the surface in all direction from the given vertex.
	for( int dir = 0; dir < vertex->directionCount; dir++ )
	{
		// Walk along the surface in this direction.
		Vertex::DirectionData* dirData = &vertex->directionData[ dir ];
		Vector walkVector;
		Scale( walkVector, dirData->tangentDir, genParms.latticeWalkLength );
		Vector point;
		Add( point, vertex->point, walkVector );
		if( !surface.ConvergePointToSurfaceInPlane( 0, point, genParms.epsilon ) )
			return false;

		// If the adjacent point is out of bounds, then we simply don't go that direction.
		if( Aabb::IS_OUTSIDE_BOX == AabbSide( genParms.aabb, point ) )
			continue;

		// If we find an already explored vertex near enough to point to which we've
		// just walked, then we will consider the new point as having already been explored.
		Vertex* adjacentVertex = FindNearEnoughVertex( point, genParms );

		// If no such vertex was found, create a new vertex to explore.
		if( !adjacentVertex )
		{
			adjacentVertex = new Vertex( point, genParms.directionCount );
			adjacentVertex->CalculateTangentDirections( surface, genParms, &dirData->tangentDir );

			// Enqueue the vertex so that we explore it later.
			vertexQueue.InsertRightOf( vertexQueue.RightMost(), adjacentVertex );
		}

		// Connect this given vertex to the newly found or newly generated vertex.
		// We have to hope that if we explore from this adjacent vertex back toward
		// the given vertex, that we find the that vertex.  If this is not the case,
		// then we'll create a mess of vertices all over the surface, and the graph
		// edges will overlap in all sorts of crazy ways.  Furthermore, our algorithm
		// may have a hard time terminating.
		dirData->adjacentVertex = adjacentVertex;
	}

	return true;
}

//=============================================================================
// This routine represent a point of possible optimization.  Here we're doing
// a linear search, but conceivably, if we kept all explored vertices in some
// sort of spacial sorting data structure, we could improve the search time.
SurfaceLattice::Vertex* SurfaceLattice::FindNearEnoughVertex( const Vector& point, const GenerationParameters& genParms )
{
	double minimumDistance = 0.0;
	Vertex* nearestVertex = 0;
	for( Vertex* vertex = ( Vertex* )vertexList.LeftMost(); vertex; vertex = ( Vertex* )vertex->Right() )
	{
		double distance = Distance( vertex->point, point );
		if( !nearestVertex || distance < minimumDistance )
		{
			nearestVertex = vertex;
			minimumDistance = distance;
		}
	}

	if( Distance( nearestVertex->point, point ) <= 0.8 * genParms.latticeWalkLength )
		return nearestVertex;

	return 0;
}

//=============================================================================
bool SurfaceLattice::GenerateEdges( const GenerationParameters& genParms )
{
	for( Vertex* vertex = ( Vertex* )vertexList.LeftMost(); vertex; vertex = ( Vertex* )vertex->Right() )
		for( int dir = 0; dir < vertex->directionCount; dir++ )
			vertex->directionData[ dir ].edgeMade = false;
	
	for( Vertex* vertex = ( Vertex* )vertexList.LeftMost(); vertex; vertex = ( Vertex* )vertex->Right() )
	{
		for( int dir = 0; dir < vertex->directionCount; dir++ )
		{
			Vertex::DirectionData* dirData = &vertex->directionData[ dir ];
			if( !dirData->edgeMade && dirData->adjacentVertex )
			{
				Edge* edge = new Edge( vertex, dirData->adjacentVertex );
				edgeList.InsertRightOf( edgeList.RightMost(), edge );

				dirData->edgeMade = true;
				int oppositeDir = dirData->adjacentVertex->FindDirectionIndex( vertex );
				if( oppositeDir != -1 )
					dirData->adjacentVertex->directionData[ oppositeDir ].edgeMade = true;
			}
		}
	}

	return true;
}

//=============================================================================
// Here we visit all irreducible cycles of the lattice.  The cycles can
// be used to generate triangle fans to approximate the surface with a mesh.
// We know that the cycle is irreducible at every point in the cycle, because
// of the way that we created the lattice structure.
bool SurfaceLattice::GenerateMesh( const GenerationParameters& genParms )
{
	// Make sure we throw away degenerate triangles.

	return true;
}

//=============================================================================
SurfaceLattice::Vertex::Vertex( const Vector& point, int directionCount )
{
	Copy( this->point, point );

	this->directionCount = directionCount;
	directionData = new DirectionData[ directionCount ];

	for( int dir = 0; dir < directionCount; dir++ )
	{
		DirectionData* dirData = &directionData[ dir ];
		dirData->adjacentVertex = 0;
		Zero( dirData->tangentDir );
		dirData->edgeMade = false;
	}
}

//=============================================================================
/*virtual*/ SurfaceLattice::Vertex::~Vertex( void )
{
	delete[] directionData;
	directionData = 0;
}

//=============================================================================
// A consistency in the winding order of the tangent directions is critical
// for the correctness of the algorithm we'll use to generate the triangular
// mesh from the surface lattice graph.
void SurfaceLattice::Vertex::CalculateTangentDirections( const Surface& surface, const GenerationParameters& genParms, const Vector* adjacentTangentDir )
{
	Vector gradient;
	surface.EvaluateGradientAt( point, gradient );
	CoordFrame coordFrame;
	Scale( coordFrame.zAxis, gradient, 1.0 );		// Not sure, actualy, how to make sure the z-axis faces the right way.
	Normalize( coordFrame.zAxis, coordFrame.zAxis );
	if( !adjacentTangentDir )
		Orthogonal( coordFrame.xAxis, coordFrame.zAxis );
	else
	{
		Scale( coordFrame.xAxis, *adjacentTangentDir, -1.0 );
		Reject( coordFrame.xAxis, coordFrame.xAxis, coordFrame.zAxis );
	}
	Cross( coordFrame.yAxis, coordFrame.zAxis, coordFrame.xAxis );
	for( int dir = 0; dir < genParms.directionCount; dir++ )
	{
		double angle = 2.0 * PI * double( dir ) / double( genParms.directionCount );
		Vertex::DirectionData* dirData = &directionData[ dir ];
		Set( dirData->tangentDir, cos( angle ), sin( angle ), 0.0 );
		Transform( dirData->tangentDir, coordFrame, dirData->tangentDir );
	}
}

//=============================================================================
int SurfaceLattice::Vertex::FindDirectionIndex( Vertex* adjacentVertex )
{
	for( int dir = 0; dir < directionCount; dir++ )
		if( directionData[ dir ].adjacentVertex == adjacentVertex )
			return dir;
	return -1;
}

//=============================================================================
SurfaceLattice::Edge::Edge( Vertex* vertex0, Vertex* vertex1 )
{
	vertex[0] = vertex0;
	vertex[1] = vertex1;
}

//=============================================================================
/*virtual*/ SurfaceLattice::Edge::~Edge( void )
{
}

//=============================================================================
SurfaceLattice::Triangle::Triangle( Vertex* vertex0, Vertex* vertex1, Vertex* vertex2 )
{
	vertex[0] = vertex0;
	vertex[1] = vertex1;
	vertex[2] = vertex2;
}

//=============================================================================
/*virtual*/ SurfaceLattice::Triangle::~Triangle( void )
{
}

// SurfaceLattice.cpp