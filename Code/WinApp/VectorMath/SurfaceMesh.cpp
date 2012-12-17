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
SurfaceMesh::SurfaceMesh( void )
{
}

//=============================================================================
/*virtual*/ SurfaceMesh::~SurfaceMesh( void )
{
	componentList.RemoveAll( true );
}

//=============================================================================
// Setup reasonable defaults here.
SurfaceMesh::GenerationParameters::GenerationParameters( void )
{
	// The generated surface will fit inside this box.
	Vector center, delta;
	Zero( center );
	Set( delta, 10.0, 10.0, 10.0 );
	MakeAabb( aabb, center, delta );

	// Let's try this tolerance.
	epsilon = 1e-7;

	// Do no more than this many iterations while generating a component of the mesh.
	maxIterations = 0;

	// This probably needs to be fine tuned for the situation at hand.
	walkDistance = 1.5;

	// In theory, this means we can generate a mesh for a surface having up to
	// four path connected components.
	Set( seedList[0], -10.0, -10.0, -10.0 );
	Set( seedList[1], 10.0, 10.0, 10.0 );
	Set( seedList[2], 0.0, 0.0, 0.0 );
	Set( seedList[3], 0.0, 0.0, 0.0 );
	seedListSize = 1;
}

//=============================================================================
/*virtual*/ SurfaceMesh::GenerationParameters::~GenerationParameters( void )
{
}

//=============================================================================
SurfaceMesh::Vertex::Vertex( const Vector& point )
{
	Copy( this->point, point );
}

//=============================================================================
/*virtual*/ SurfaceMesh::Vertex::~Vertex( void )
{
}

//=============================================================================
SurfaceMesh::Triangle::Triangle( Vertex* vertex0, Vertex* vertex1, Vertex* vertex2 )
{
	vertex[0] = vertex0;
	vertex[1] = vertex1;
	vertex[2] = vertex2;

	adjacentTriangle[0] = 0;
	adjacentTriangle[1] = 0;
	adjacentTriangle[2] = 0;
}

//=============================================================================
/*virtual*/ SurfaceMesh::Triangle::~Triangle( void )
{
}

//=============================================================================
SurfaceMesh::Edge::Edge( Vertex* vertex0, Vertex* vertex1, Triangle* triangle )
{
	vertex[0] = vertex0;
	vertex[1] = vertex1;

	this->triangle = triangle;
}

//=============================================================================
/*virtual*/ SurfaceMesh::Edge::~Edge( void )
{
}

//=============================================================================
SurfaceMesh::PathConnectedComponent::PathConnectedComponent( void )
{
}

//=============================================================================
/*virtual*/ SurfaceMesh::PathConnectedComponent::~PathConnectedComponent( void )
{
	WipeClean();
}

//=============================================================================
void SurfaceMesh::PathConnectedComponent::WipeClean( void )
{
	vertexList.RemoveAll( true );
	triangleList.RemoveAll( true );
	edgeList.RemoveAll( true );
}

//=============================================================================
// Generate the path connected component of the given surface that contains
// the given surface point.
bool SurfaceMesh::PathConnectedComponent::Generate( const Surface& surface, const Vector& surfacePoint, const GenerationParameters& genParms )
{
	// Start with a clean slate.
	WipeClean();

	// We start by finding an initial triangle on the surface.
	if( !GenerateInitialTriangle( surface, surfacePoint, genParms ) )
		return false;

	// We then build upon the initial triangle until the final triangle is generated.
	int iterationCount = 0;
	while( edgeList.Count() > 0 && iterationCount++ < genParms.maxIterations )
		if( !GenerateNewTriangle( surface, genParms ) )
			return false;

	return true;
}

//=============================================================================
// We don't know what direction the initial triangle should face, but it will
// be important that all subsequently generated triangles face a consistent
// direction.  That is, for any two adjacent triangles, they must face the
// same direction.  We can then correct the direction all triangles face in
// the entire mesh later when we're done.
bool SurfaceMesh::PathConnectedComponent::GenerateInitialTriangle( const Surface& surface, const Vector& surfacePoint, const GenerationParameters& genParms )
{
	// Build a coordinate frame for the tangent space at the given surface point.
	CoordFrame coordFrame;
	surface.EvaluateGradientAt( surfacePoint, coordFrame.zAxis );
	Normalize( coordFrame.zAxis, coordFrame.zAxis );
	Scale( coordFrame.zAxis, coordFrame.zAxis, -1.0 );
	Orthogonal( coordFrame.yAxis, coordFrame.zAxis );
	Cross( coordFrame.xAxis, coordFrame.yAxis, coordFrame.zAxis );

	// Build an isosceles triangle in the tangent space.
	VectorMath::Triangle triangleVertices;
	Vector vec;
	Set( vec, cos( PI / 3.0 ), sin( PI / 3.0 ), 0.0 );
	Transform( vec, coordFrame, vec );
	Copy( triangleVertices.vertex[0], surfacePoint );
	AddScale( triangleVertices.vertex[1], surfacePoint, coordFrame.xAxis, genParms.walkDistance );
	AddScale( triangleVertices.vertex[2], surfacePoint, vec, genParms.walkDistance );

	// Now make sure that the triangle vertices are on the surface.
	if( !surface.ConvergePointToSurfaceInPlane( 0, triangleVertices.vertex[1], genParms.epsilon ) )
		return false;
	if( !surface.ConvergePointToSurfaceInPlane( 0, triangleVertices.vertex[2], genParms.epsilon ) )
		return false;

	// Add the triangle vertices.
	Vertex* vertex0 = new Vertex( triangleVertices.vertex[0] );
	Vertex* vertex1 = new Vertex( triangleVertices.vertex[1] );
	Vertex* vertex2 = new Vertex( triangleVertices.vertex[2] );
	vertexList.InsertRightOf( vertexList.RightMost(), vertex0 );
	vertexList.InsertRightOf( vertexList.RightMost(), vertex1 );
	vertexList.InsertRightOf( vertexList.RightMost(), vertex2 );

	// Add the triangle.
	Triangle* triangle = new Triangle( vertex0, vertex1, vertex2 );
	triangleList.InsertRightOf( triangleList.RightMost(), triangle );

	// Add the triangle edges.
	Edge* edge0 = new Edge( vertex0, vertex1, triangle );
	Edge* edge1 = new Edge( vertex1, vertex2, triangle );
	Edge* edge2 = new Edge( vertex2, vertex0, triangle );
	edgeList.InsertRightOf( edgeList.RightMost(), edge0 );
	edgeList.InsertRightOf( edgeList.RightMost(), edge1 );
	edgeList.InsertRightOf( edgeList.RightMost(), edge2 );

	// We're finished.
	return true;
}

//=============================================================================
bool SurfaceMesh::PathConnectedComponent::GenerateNewTriangle( const Surface& surface, const GenerationParameters& genParms )
{
	return true;
}

//=============================================================================
bool SurfaceMesh::IsPointOnSurface( const Vector& point, double epsilon ) const
{
	// Is the given point on any one of the currently generated components?
	const PathConnectedComponent* component = ( const PathConnectedComponent* )componentList.LeftMost();
	while( component )
	{
		if( component->IsPointOnSurface( point, epsilon ) )
			return true;		// Approximately, yes.
		component = ( const PathConnectedComponent* )component->Right();
	}
	return false;		// No.
}

//=============================================================================
bool SurfaceMesh::PathConnectedComponent::IsPointOnSurface( const Vector& point, double epsilon ) const
{
	return false;
}

//=============================================================================
bool SurfaceMesh::Generate( const Surface& surface, const GenerationParameters& genParms )
{
	// Begin with a blank slate.
	componentList.RemoveAll( true );

	// Given each given seed a try.
	for( int index = 0; index < genParms.seedListSize; index++ )
	{
		// Try to converge this seed point to the surface.  If it does
		// not converge, skip the point.
		Vector surfacePoint;
		Copy( surfacePoint, genParms.seedList[ index ] );
		if( !surface.ConvergePointToSurfaceInPlane( 0, surfacePoint, genParms.epsilon ) )
			continue;

		// If the surface point found is already on an existing component,
		// then skip the point, because we don't want to regenerate that component.
		if( IsPointOnSurface( surfacePoint, genParms.epsilon ) )
			continue;

		// Try to generate the component.  Did we succeed?
		PathConnectedComponent* component = new PathConnectedComponent();
		if( !component->Generate( surface, surfacePoint, genParms ) )
		{
			// No.  Delete the component and return failure.
			delete component;
			return false;
		}
		else
		{
			// Yes.  Accumulate the component.
			componentList.InsertRightOf( componentList.RightMost(), component );
		}
	}

	// Return success.
	return true;
}

//=============================================================================
void SurfaceMesh::Render( RenderInterface& renderInterface, bool forDebug ) const
{
	const PathConnectedComponent* component = ( const PathConnectedComponent* )componentList.LeftMost();
	while( component )
	{
		component->Render( renderInterface, forDebug );
		component = ( const PathConnectedComponent* )component->Right();
	}
}

//=============================================================================
void SurfaceMesh::PathConnectedComponent::Render( RenderInterface& renderInterface, bool forDebug ) const
{
	// Go draw all the triangles with various colors so
	// that we can easily distinguish between them.
	srand(0);		// A consistent seed gives us consistent colors between frames.
	for( const Triangle* triangle = ( const Triangle* )triangleList.LeftMost(); triangle; triangle = ( const Triangle* )triangle->Right() )
	{
		Vector color;
		if( !forDebug )
			Zero( color );
		else
		{
			switch( rand() % 3 )
			{
				case 0: Set( color, 1.0, 1.0, 0.0 ); break;
				case 1: Set( color, 0.0, 1.0, 0.0 ); break;
				case 2: Set( color, 0.0, 0.0, 1.0 ); break;
			}
		}

		VectorMath::Triangle triangleVertices;
		Copy( triangleVertices.vertex[0], triangle->vertex[0]->point );
		Copy( triangleVertices.vertex[1], triangle->vertex[1]->point );
		Copy( triangleVertices.vertex[2], triangle->vertex[2]->point );

		renderInterface.RenderTriangle( triangleVertices, color, 0.5 );
	}
	
	// Now go draw all of the edges on the periphery.
	// This is only for debug, because only in debug do we
	// want to draw partially generated meshes.
	if( forDebug )
	{
		Vector color;
		Set( color, 1.f, 0.f, 0.f );
		for( const Edge* edge = ( const Edge* )edgeList.LeftMost(); edge; edge = ( const Edge* )edge->Right() )
		{
			Vector vertex0, vertex1;
			Copy( vertex0, edge->vertex[0]->point );
			Copy( vertex1, edge->vertex[1]->point );

			renderInterface.RenderEdge( vertex0, vertex1, color, 1.0 );
		}
	}
}

// SurfaceMesh.cpp