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
		bool surfaceComplete = false;
		while( !surfaceComplete )
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
}

//=============================================================================
/*virtual*/ SurfaceMesh::Triangle::~Triangle( void )
{
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
	return false;
}

//=============================================================================
bool SurfaceMesh::CompleteAnEdge( const Surface& surface, const SurfaceMeshGenerator::Parameters& parms )
{
	return false;
}

// SurfaceMesh.cpp