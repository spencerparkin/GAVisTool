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
#include "Assert.h"

using namespace VectorMath;

//=============================================================================
SurfaceMeshGenerator::Parameters::Parameters( void )
{
	epsilon = 1e-7;

	// For surfaces that go out to infinity, we need to contain them in an AABB.
	Vector center, delta;
	Zero( center );
	Set( delta, 10.0, 10.0, 10.0 );
	MakeAabb( aabb, center, delta );

	// A better algorithm would determine how many connected components a surface
	// has and be able to find a point on each one of them.  Here we are hoping
	// that well chosen seed points can be made to converge on as many of the
	// connected components as we can.
	seedPointCount = 2;
	Set( seedPoint[0], -10.0, -10.0, -10.0 );
	Set( seedPoint[0], 10.0, 10.0, 10.0 );

	// This is used both for debugging and for making sure that our algorithm always terminates.
	maxIterations = 2;

	// This is how far we walk in the tangent space of a surface point to find
	// a point that we'll converge back to the surface.
	surfaceWalkDistance = 0.5;
}

//=============================================================================
SurfaceMeshGenerator::SurfaceMeshGenerator( void )
{
}

//=============================================================================
/*virtual*/ SurfaceMeshGenerator::~SurfaceMeshGenerator( void )
{
}

//=============================================================================
void SurfaceMeshGenerator::Generate( SurfaceMesh& surfaceMesh, const Surface& surface )
{
	// Begin with a blank slate.
	surfaceMesh.WipeClean();

	// Try to generate all connected components of the given surface through the use of each seed point.
	for( int index = 0; index < parms.seedPointCount; index++ )
	{
		// Does the seed point converge to a point on the surface?
		Vector surfacePoint;
		Copy( surfacePoint, parms.seedPoint[ index ] );
		if( !surface.ConvergePointToSurfaceInPlane( 0, surfacePoint, parms.epsilon ) )
			continue;		// Yes, skip the point.

		// Is the surface point already part of a previously generated connected component of the surface?
		if( surfaceMesh.IsPointOnMesh( surfacePoint, parms.epsilon ) )
			continue;		// Yes, skip the point.

		// Can we generate an initial triangle on the surface mesh?
		SurfaceMesh connectedComponent;
		if( !connectedComponent.GenerateInitialTriangle( surfacePoint, surface, parms ) )
			continue;		// No, skip the point.

		// Work to the complete the surface mesh until it is finished.
		int iterationCount = 0;
		bool surfaceComplete = false;
		while( !surfaceComplete && iterationCount++ < parms.maxIterations )
			surfaceComplete = connectedComponent.CompleteAnEdge( surface, parms );
		
		// Absorb the newly created connected component of the surface.
		surfaceMesh.AbsorbAllCompletedTriangles( connectedComponent );
	}
}

//=============================================================================
SurfaceMesh::SurfaceMesh( void )
{
}

//=============================================================================
/*virtual*/ SurfaceMesh::~SurfaceMesh( void )
{
	WipeClean();
}

//=============================================================================
SurfaceMesh::Triangle::Triangle( void )
{
	Zero( vertices.vertex[0] );
	Zero( vertices.vertex[1] );
	Zero( vertices.vertex[2] );

	edgeComplete[0] = false;
	edgeComplete[1] = false;
	edgeComplete[2] = false;

	adjacentTriangle[0] = 0;
	adjacentTriangle[1] = 0;
	adjacentTriangle[2] = 0;
}

//=============================================================================
/*virtual*/ SurfaceMesh::Triangle::~Triangle( void )
{
}

//=============================================================================
bool SurfaceMesh::Triangle::IsComplete( void ) const
{
	if( !edgeComplete[0] )
		return false;
	if( !edgeComplete[1] )
		return false;
	if( !edgeComplete[2] )
		return false;
	return true;
}

//=============================================================================
int Surface::Triangle::FirstUncompletedEdge( void ) const
{
	if( !edgeCompleted[0] )
		return 0;
	if( !edgeCompleted[1] )
		return 1;
	if( !edgeCompleted[2] )
		return 2;
	return -1;
}

//=============================================================================
void SurfaceMesh::WipeClean( void )
{
	completedTriangleList.RemoveAll( true );
	uncompletedTriangleList.RemoveAll( true );
}

//=============================================================================
bool SurfaceMesh::IsPointOnMesh( const Vector& point, double epsilon ) const
{
	return false;
}

//=============================================================================
const Utilities::List& SurfaceMesh::TriangleList( void ) const
{
	return completedTriangleList;
}

//=============================================================================
void SurfaceMesh::AbsorbAllCompletedTriangles( SurfaceMesh& surfaceMesh )
{
	surfaceMesh.completedTriangleList.EmptyIntoOnRight( completedTriangleList );
}

//=============================================================================
bool SurfaceMesh::GenerateInitialTriangle( const Vector& surfacePoint, const Surface& surface, const SurfaceMeshGenerator::Parameters& parms )
{
	// Create a local coordinate frame for the tangent space at the given surface point.
	// For closed surfaces, we have no way of knowing if the z-axis is pointing into or
	// out of the surface.  The orientation/winding of this triangle will determine the
	// orientation/winding of all other triangles in the mesh.
	CoordFrame coordFrame;
	surface.EvaluateGradientAt( surfacePoint, coordFrame.zAxis );
	Normalize( coordFrame.zAxis, coordFrame.zAxis );
	Orthogonal( coordFrame.yAxis, coordFrame.zAxis );
	Cross( coordFrame.xAxis, coordFrame.yAxis, coordFrame.zAxis );

	// Create a new triangle in the tangent space.
	VectorMath::Triangle vertices;
	Copy( vertices.vertex[0], surfacePoint );
	AddScale( vertices.vertex[1], surfacePoint, coordFrame.xAxis, parms.surfaceWalkDistance );
	AddScale( vertices.vertex[2], surfacePoint, coordFrame.yAxis, parms.surfaceWalkDistance );
	
	// Converge the vertices of the triangle not on the surface onto the surface.
	if( !surface.ConvergePointToSurfaceInPlane( 0, vertices.vertex[1], parms.epsilon ) )
		return false;
	if( !surface.ConvergePointToSurfaceInPlane( 0, vertices.vertex[2], parms.epsilon ) )
		return false;

	// Make sure the triangle is in the bounding box.
	if( !IsTriangleInsideAabb( parms.aabb, vertices ) )
		return false;

	// Add the newly created triangle to the list of uncompleted triangles.
	Triangle* triangle = new Triangle();
	CopyTriangle( triangle->vertices, vertices );
	uncompletedTriangleList.InsertRightOf( uncompletedTriangleList.RightMost(), triangle );
	return true;
}

//=============================================================================
// Our return value here indicates whether or not we believe that the surface
// mesh is complete.  It is complete when there are no more uncompleted triangles.
bool SurfaceMesh::CompleteAnEdge( const Surface& surface, const SurfaceMeshGenerator::Parameters& parms )
{
	// Complete an edge of an uncompleted triangle.
	Triangle* triangle = ( Triangle* )uncompletedTriangleList.LeftMost();
	CompleteEdgeOfTriangle( triangle, surface, parms );
	if( triangle->IsComplete() )
	{
		uncompletedTriangleList.Remove( triangle, false );
		completedTriangleList.InsertRightOf( completedTriangleList.RightMost(), triangle );
	}

	// The mesh is complete when there are no more uncompleted triangles.
	if( uncompletedTriangleList.Count() > 0 )
		return false;
	return true;
}

//=============================================================================
// Here we choose an edge of the given triangle and we complete that edge.
// We will always succeed whether an adjacent triangle is created or not.
// The key to proper termination of our mesh generation algorithm will be
// the ability to add triangles that use existing edges of the mesh wherever
// possible.  If we fail to do this, then the mesh algorithm might run on
// forever, never completing, because it keeps creating new uncompleted triangles.
void SurfaceMesh::CompleteEdgeOfTriangle( Triangle* triangle, const Surface& surface, const SurfaceMeshGenerator::Parameters& parms )
{
	// If the triangle is actually complete, we're done.
	int edge = triangle->FirstUncompletedEdge();
	if( edge == -1 )
		return;

	// Mark the edge as complete.  We always succeed no matter what happens.
	triangle->edgeComplete[ edge ] = true;

	// Translate the edge index into its vertex indices.
	int vertex0 = edge;
	int vertex1 = ( edge + 1 ) % 3;

	// If a new adjacent triangle were to be created off of this edge,
	// could its remaining vertex be taken from an existing vertex in the mesh?
	// We want to use such a vertex whenever possible in the hopes that this
	// brings our algorithm to a proper end.
	Vector remainingVertex;
	bool foundVertex = FindVertexForNewTriangle( remainingVertex, adjacentTriangle1, adjacentTriangle2, triangle, edge, surface, parms );

	// Are we going to create a new adjacent triangle?
	if( foundVertex )
	{
		// Yes, create the adjacent triangle so that winding order is consistent across the mesh surface.
		Triangle* newTriangle = new Triangle();
		Copy( newTriangle->vertex[0], triangle->vertex[ vertex1 ] );
		Copy( newTriangle->vertex[1], triangle->vertex[ vertex0 ] );
		Copy( newTriangle->vertex[2], remainingVertex );
		triangle->adjacentTriangle[ edge ] = newTriangle;
		newTriangle->adjacentTriangle[0] = triangle;
		newTriangle->edgeComplete[0] = true;

		if( adjacentTriangle1

		// We always add newly created triangles to this list.
		uncompletedTriangleList.InsertRightOf( uncompletedTriangleList.RightMost(), newTriangle );
	}
}

// SurfaceMesh.cpp