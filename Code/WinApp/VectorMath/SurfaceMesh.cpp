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

	// In theory, this means we can generate a mesh for a surface having up to
	// four path connected components.
	Set( seedList[0], -10.0, -10.0, -10.0 );
	Set( seedList[1], 10.0, 10.0, 10.0 );
	Set( seedList[2], 0.0, 0.0, 0.0 );
	Set( seedList[4], 0.0, 0.0, 0.0 );
	seedListSize = 1;
}

//=============================================================================
/*virtual*/ SurfaceMesh::GenerationParameters::~GenerationParameters( void )
{
}

//=============================================================================
SurfaceMesh::Vertex::Vertex( void )
{
	Zero( point );
}

//=============================================================================
/*virtual*/ SurfaceMesh::Vertex::~Vertex( void )
{
}

//=============================================================================
SurfaceMesh::Triangle::Triangle( void )
{
	vertex[0] = 0;
	vertex[1] = 0;
	vertex[2] = 0;

	adjacentTriangle[0] = 0;
	adjacentTriangle[1] = 0;
	adjacentTriangle[2] = 0;
}

//=============================================================================
/*virtual*/ SurfaceMesh::Triangle::~Triangle( void )
{
}

//=============================================================================
SurfaceMesh::Edge::Edge( void )
{
	vertex[0] = 0;
	vertex[1] = 0;

	triangle = 0;
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
}

//=============================================================================
// Generate the path connected component of the given surface that contains
// the given surface point.
bool SurfaceMesh::PathConnectedComponent::Generate( const Surface& surface, const Vector& surfacePoint, const GenerationParameters& genParms )
{
	return true;
}

//=============================================================================
bool SurfaceMesh::IsPointOnSurface( const Vector& point, double epsilon ) const
{
	const PathConnectedComponent* component = ( const PathConnectedComponent* )componentList.LeftMost();
	while( component )
	{
		if( component->IsPointOnSurface( point, epsilon ) )
			return true;
		component = ( const PathConnectedComponent* )component->Right();
	}
	return false;
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
				case 0: Set( color, 1.0, 0.0, 0.0 ); break;
				case 1: Set( color, 0.0, 1.0, 0.0 ); break;
				case 2: Set( color, 0.0, 0.0, 1.0 ); break;
			}
		}

		VectorMath::Triangle triangleVertices;
		Copy( triangleVertices.vertex[0], triangle->vertex[0]->point );
		Copy( triangleVertices.vertex[1], triangle->vertex[1]->point );
		Copy( triangleVertices.vertex[2], triangle->vertex[2]->point );

		renderInterface.RenderTriangle( triangleVertices, color );
	}
	
	// Now go draw all of the edges on the periphery.
	// This is only for debug, because only in debug do we
	// want to draw partially generated meshes.
	if( forDebug )
	{
		Vector color;
		Set( color, 0.f, 0.f, 0.f );
		for( const Edge* edge = ( const Edge* )edgeList.LeftMost(); edge; edge = ( const Edge* )edge->Right() )
		{
			Vector vertex0, vertex1;
			Copy( vertex0, edge->vertex[0]->point );
			Copy( vertex1, edge->vertex[1]->point );

			renderInterface.RenderEdge( vertex0, vertex1, color );
		}
	}
}

// SurfaceMesh.cpp