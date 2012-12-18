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
	maxIterations = 1;

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
	while( edgeList.Count() > 0 && ++iterationCount <= genParms.maxIterations )
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

	// Make sure that we're starting in bounds.
	if( Aabb::IS_OUTSIDE_BOX == AabbSide( genParms.aabb, triangleVertices.vertex[0], genParms.epsilon ) )
		return false;
	if( Aabb::IS_OUTSIDE_BOX == AabbSide( genParms.aabb, triangleVertices.vertex[1], genParms.epsilon ) )
		return false;
	if( Aabb::IS_OUTSIDE_BOX == AabbSide( genParms.aabb, triangleVertices.vertex[2], genParms.epsilon ) )
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
// If at all possible, we always want to connect an edge to an existing
// vertex.  Otherwise, our algorithm could never terminate in the way
// that we would hope, having completely covered the surface.  The first
// such vertex we search for is one on an edge adjacent to the edge that
// we are processing.  The second such vertex we search from is one within
// the given walk distance of the edge we're processing.  This is so that
// the mesh can grow into and not over itself.
bool SurfaceMesh::PathConnectedComponent::GenerateNewTriangle( const Surface& surface, const GenerationParameters& genParms )
{
	// If there is no edge to process, then our job is done.
	Edge* edge = ( Edge* )edgeList.LeftMost();
	if( !edge )
		return true;

	// No matter what happens with the edge, we remove it from the list
	// to indicate that we have considered it and processed it.
	edgeList.Remove( edge, false );
	Utilities::ScopeDelete< Edge > scopeDelete( edge );

	// If the entire edge is out of bounds, then we're done.
	if( Aabb::IS_OUTSIDE_BOX == AabbSide( genParms.aabb, edge->vertex[0]->point, genParms.epsilon ) )
		return true;
	if( Aabb::IS_OUTSIDE_BOX == AabbSide( genParms.aabb, edge->vertex[1]->point, genParms.epsilon ) )
		return true;

	// We first try to find an existing vertex as the CW or CCW vertex WRT this edge.
	Vertex* newVertex = 0;
	Vertex* ccwVertex = edge->FindAdjacentVertex( Edge::CCW_VERTEX );
	Vertex* cwVertex = edge->FindAdjacentVertex( Edge::CW_VERTEX );
	double ccwAngle = CalculateInteriorAngle( edge->vertex[0], edge->vertex[1], ccwVertex );
	double cwAngle = CalculateInteriorAngle( edge->vertex[1], edge->vertex[0], cwVertex );
	Plane edgePlane;
	edge->MakeEdgePlane( edgePlane );
	if( Plane::SIDE_FRONT == PlaneSide( edgePlane, ccwVertex->point, genParms.epsilon ) && ccwAngle < PI / 2.0 )
	{
		newVertex = ccwVertex;
	}
	else if( Plane::SIDE_FRONT == PlaneSide( edgePlane, cwVertex->point, genParms.epsilon ) && cwAngle < PI / 2.0 )
	{
		newVertex = cwVertex;
	}
	else
	{
		// Search the entire edge list.  If the edge we're processing is in the frontier
		// of an edge we're considering in our search, and one of its vertices is within
		// the given walk distance, then we can connect with it.
	}

	// Ultimately, if we couldn't find an existing vertex, we need to create a new one.
	if( !newVertex )
	{
		// Walk into the new frontier.  We fail here if we fail to converge to the surface.
		Vector point;
		Lerp( point, edge->vertex[0]->point, edge->vertex[1]->point, 0.5 );
		AddScale( point, point, edgePlane.normal, genParms.walkDistance );
		if( !surface.ConvergePointToSurfaceInPlane( 0, point, genParms.epsilon ) )
			return false;

		// Add the new point to our list.
		newVertex = new Vertex( point );
		vertexList.InsertRightOf( vertexList.RightMost(), newVertex );
	}

	// Create the new triangle.
	Triangle* triangle = new Triangle( edge->vertex[1], edge->vertex[0], newVertex );
	triangleList.InsertRightOf( triangleList.RightMost(), triangle );
	triangle->adjacentTriangle[0] = edge->triangle;

	// We either create a new CCW edge or delete an old one.
	if( ccwVertex == newVertex )
	{
		Edge* adjacentEdge = FindEdge( edge->vertex[0], newVertex );
		assert->Condition( adjacentEdge != 0, "Should have found adjacent edge!" );
		triangle->adjacentTriangle[1] = adjacentEdge->triangle;
		edgeList.Remove( adjacentEdge, true );
	}
	else
	{
		Edge* adjacentEdge = new Edge( edge->vertex[0], newVertex, triangle );
		edgeList.InsertRightOf( edgeList.RightMost(), adjacentEdge );
	}
	
	// We either create a new CW edge or delete an old one.
	if( cwVertex == newVertex )
	{
		Edge* adjacentEdge = FindEdge( newVertex, edge->vertex[1] );
		assert->Condition( adjacentEdge != 0, "Should have found adjacent edge!" );
		triangle->adjacentTriangle[2] = adjacentEdge->triangle;
		edgeList.Remove( adjacentEdge, true );
	}
	else
	{
		Edge* adjacentEdge = new Edge( newVertex, edge->vertex[1], triangle );
		edgeList.InsertRightOf( edgeList.RightMost(), adjacentEdge );
	}

	// Lastly, knowing that the new triangle points to its adjacent triangles,
	// make sure that its adjacent triangles point back to it.
	triangle->PatchAdjacencies();

	// Return success.
	return true;
}

//=============================================================================
double SurfaceMesh::PathConnectedComponent::CalculateInteriorAngle( Vertex* vertex0, Vertex* vertex1, Vertex* vertex2 ) const
{
	Vector edge01, edge02;
	Sub( edge01, vertex1->point, vertex0->point );
	Sub( edge02, vertex2->point, vertex0->point );
	double angle = AngleBetween( edge01, edge02 );
	return angle;
}

//=============================================================================
void SurfaceMesh::Triangle::PatchAdjacencies( void )
{
	for( int adjacentTriangleIndex = 0; adjacentTriangleIndex < 3; adjacentTriangleIndex++ )
	{
		Triangle* triangle = adjacentTriangle[ adjacentTriangleIndex ];
		if( triangle )
		{
			int nonSharedVertexIndex = 0;
			for( nonSharedVertexIndex = 0; nonSharedVertexIndex < 3; nonSharedVertexIndex++ )
				if( FindVertexIndex( triangle->vertex[ nonSharedVertexIndex ] ) == -1 )
					break;
			assert->Condition( nonSharedVertexIndex < 3, "Could not find non-shared vertex index." );
			int index = ( nonSharedVertexIndex + 1 ) % 3;
			assert->Condition( FindVertexIndex( triangle->vertex[ index ] ) != -1, "Vertex should be shared." );
			assert->Condition( FindVertexIndex( triangle->vertex[ ( index + 1 ) % 3 ] ) != -1, "Vertex should also be shared." );
			triangle->adjacentTriangle[ index ] = this;
		}
	}
}

//=============================================================================
SurfaceMesh::Edge* SurfaceMesh::PathConnectedComponent::FindEdge( Vertex* vertex0, Vertex* vertex1 )
{
	Edge* foundEdge = 0;
	for( Edge* edge = ( Edge* )edgeList.LeftMost(); edge && !foundEdge; edge = ( Edge* )edge->Right() )
		if( ( edge->vertex[0] == vertex0 && edge->vertex[1] == vertex1 ) || ( edge->vertex[0] == vertex1 || edge->vertex[1] == vertex0 ) )
			foundEdge = edge;
	return foundEdge;
}

//=============================================================================
void SurfaceMesh::Edge::MakeEdgePlane( Plane& edgePlane ) const
{
	Vector edgeVector;
	Sub( edgeVector, vertex[1]->point, vertex[0]->point );
	Vector edge01, edge02, triangleNormal;
	Sub( edge01, triangle->vertex[1]->point, triangle->vertex[0]->point );
	Sub( edge02, triangle->vertex[2]->point, triangle->vertex[0]->point );
	Cross( triangleNormal, edge01, edge02 );
	Vector edgePlaneNormal;
	Cross( edgePlaneNormal, edgeVector, triangleNormal );
	MakePlane( edgePlane, vertex[0]->point, edgePlaneNormal );
}

//=============================================================================
SurfaceMesh::Vertex* SurfaceMesh::Edge::FindAdjacentVertex( VertexType vertexType ) const
{
	// Which vertex are we pivoting about?
	Vertex* pivotVertex = 0;
	if( vertexType == CCW_VERTEX )
		pivotVertex = vertex[0];
	else if( vertexType == CW_VERTEX )
		pivotVertex = vertex[1];
	assert->Condition( pivotVertex != 0, "Null pivot vertex!" );

	// Find the last triangle we can find while winding about that vertex
	// in the desired direction.
	Triangle* windingTriangle = triangle;
	while( true )
	{
		int vertexIndex = windingTriangle->FindVertexIndex( pivotVertex );
		int triangleIndex = -1;
		if( vertexType == CCW_VERTEX )
			triangleIndex = ( vertexIndex + 2 ) % 3;
		else if( vertexType == CW_VERTEX )
			triangleIndex = vertexIndex;
		Triangle* adjacentTriangle = windingTriangle->adjacentTriangle[ triangleIndex ];
		if( !adjacentTriangle )
			break;
		windingTriangle = adjacentTriangle;
	}
	
	// Return the correct vertex of the found triangle.
	int vertexIndex = windingTriangle->FindVertexIndex( pivotVertex );
	if( vertexType == CCW_VERTEX )
		vertexIndex = ( vertexIndex + 2 ) % 3;
	else if( vertexType == CW_VERTEX )
		vertexIndex = ( vertexIndex + 1 ) % 3;
	return windingTriangle->vertex[ vertexIndex ];
}

//=============================================================================
int SurfaceMesh::Triangle::FindVertexIndex( Vertex* vertex ) const
{
	if( vertex == this->vertex[0] )
		return 0;
	if( vertex == this->vertex[1] )
		return 1;
	if( vertex == this->vertex[2] )
		return 2;
	return -1;
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
		Set( color, 1.0, 0.0, 0.0 );

		for( const Edge* edge = ( const Edge* )edgeList.LeftMost(); edge; edge = ( const Edge* )edge->Right() )
		{
			Vector vertex0, vertex1;
			Copy( vertex0, edge->vertex[0]->point );
			Copy( vertex1, edge->vertex[1]->point );

			renderInterface.RenderLine( vertex0, vertex1, color, 1.0 );
		}
	}

	// In debug mode, it would be helpful to see the adjacency information.
#if 0
	if( forDebug )
	{
		Vector color;
		Set( color, 0.0, 0.0, 1.0 );

		for( const Triangle* triangle = ( const Triangle* )triangleList.LeftMost(); triangle; triangle = ( const Triangle* )triangle->Right() )
		{
			Vector center;
			triangle->CalculateCenter( center );

			for( int index = 0; index < 3; index++ )
			{
				const Triangle* adjacentTriangle = triangle->adjacentTriangle[ index ];
				if( adjacentTriangle )
				{
					Vector adjacentCenter;
					adjacentTriangle->CalculateCenter( adjacentCenter );

					Vector edgePoint;
					Lerp( edgePoint, triangle->vertex[ index ]->point, triangle->vertex[ ( index + 1 ) % 3 ]->point, 0.25 );

					renderInterface.RenderArrow( center, edgePoint, color, 1.0 );
					renderInterface.RenderArrow( edgePoint, adjacentCenter, color, 1.0 );
				}
			}
		}
	}
#endif
}

//=============================================================================
void SurfaceMesh::Triangle::CalculateCenter( Vector& center ) const
{
	Add( center, vertex[0]->point, vertex[1]->point );
	Add( center, center, vertex[2]->point );
	Scale( center, center, 1.0 / 3.0 );
}

// SurfaceMesh.cpp