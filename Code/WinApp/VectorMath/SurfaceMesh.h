// SurfaceMesh.h

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#include "Surface.h"

namespace VectorMath
{
	//=============================================================================
	class SurfaceMesh
	{
	public:

		SurfaceMesh( void );
		virtual ~SurfaceMesh( void );

		//=============================================================================
		class RenderInterface
		{
		public:

			virtual void RenderTriangle( const VectorMath::Triangle& triangle, const Vector& color, double alpha ) = 0;
			virtual void RenderLine( const Vector& vertex0, const Vector& vertex1, const Vector& color, double alpha ) = 0;
			virtual void RenderArrow( const Vector& vertex0, const Vector& vertex1, const Vector& color, double alpha ) {}
		};

		//=============================================================================
		class GenerationParameters
		{
		public:

			GenerationParameters( void );
			virtual ~GenerationParameters( void );

			// Not all surfaces fit in a finite volume, so we need to provide
			// one so that our algorithm doesn't run on forever.
			Aabb aabb;

			// This tolerance is used in a number of places.  We might consider
			// creating multiple tolerances for various purposes.
			double epsilon;

			// This is used for debugging and as a guarentee that our algorithm
			// will eventually terminate.
			int maxIterations;

			// This is how far we walk out into the tangent space of an edge
			// or point to build upon the partially generated mesh.
			double walkDistance;

			// This is the maximum angle at which two adjacent edges can be
			// incident and we'll still connect a triangle on these edges.
			double frontierAngle;

			// This is the maximum amount of angle we can deviate from a root
			// triangle used in a traversal algorithm about that root triangle.
			double deviationAngle;

			// We try to find all the different components of the mesh by converging
			// different seed points to the surface of the given surface.  This is
			// not a very smart way to do it, but until I can think of something better,
			// this will have to do.
			enum { MAX_SEEDS = 4 };
			Vector seedList[ MAX_SEEDS ];
			int seedListSize;
		};

		//=============================================================================
		class Vertex : public Utilities::List::Item
		{
		public:

			Vertex( const Vector& point );
			virtual ~Vertex( void );

			// Vertices are the positions in space for triangles and edges in the mesh.
			Vector point;

			// This is used in an algorithm for visiting the vertices of the mesh.
			int visitationKey;
		};

		//=============================================================================
		class Triangle : public Utilities::List::Item
		{
		public:

			Triangle( Vertex* vertex0, Vertex* vertex1, Vertex* vertex2 );
			virtual ~Triangle( void );

			// Assuming we have the given vertex, what index does it reside at?
			int FindVertexIndex( Vertex* vertex ) const;

			// Return the center of the triangle.
			void CalculateCenter( Vector& center ) const;

			// This is used to patch adjacencies of adjacent triangles.
			void PatchAdjacencies( void );

			// Tell us if the given vertex is in the space of this triangle.
			bool VertexIsInTriangleSpace( const Vector& vertex ) const;

			// The vertices are ordered counter clock-wise.  That is, when you
			// imagine the triangle on paper, the vertices are drawn in that order.
			Vertex* vertex[3];

			// An adjacent triangle K refers to the triangle sharing the edge
			// having vertices K and (K+1) mod 3.
			Triangle* adjacentTriangle[3];

			// These are used to prevent the creation of unwanted triangles.
			Vector normal;
			Plane edgePlane[3];
			int visitationKey;
		};

		//=============================================================================
		class Edge : public Utilities::List::Item
		{
		public:

			Edge( Vertex* vertex0, Vertex* vertex1, Triangle* triangle );
			virtual ~Edge( void );

			// The returned edge plane partitions space so that the space in front
			// of the plane is the new frontier of the partially generated mesh.
			void MakeEdgePlane( Plane& edgePlane ) const;

			// Wind through triangles as much as possible in the given direction
			// to find an adjacent vertex.
			enum VertexType { CW_VERTEX, CCW_VERTEX };
			Vertex* FindAdjacentVertex( VertexType vertexType ) const;

			// As part of a triangle, this edge, from vertex 0 to 1, goes in the
			// counter clock-wise direction.
			Vertex* vertex[2];

			// An edge is shared by 2 triangles, but edges are not meant to be a
			// part of the final mesh.  Edges are only used to mark the periphery
			// of the partially generated mesh.
			Triangle* triangle;
		};

		//=============================================================================
		// The zero set of a polynomial is a surface that may have zero or
		// more path connected components.  These are surfaces where any two
		// points of such surfaces can be connected by a continuous path on
		// the surface.
		class PathConnectedComponent : public Utilities::List::Item
		{
			friend class SurfaceMesh;

		public:

			PathConnectedComponent( void );
			virtual ~PathConnectedComponent( void );

			bool IsPointOnSurface( const Vector& point, double epsilon ) const;
			void Render( RenderInterface& renderInterface, bool forDebug ) const;

		private:

			void WipeClean( void );
			bool Generate( const Surface& surface, const Vector& surfacePoint, const GenerationParameters& genParms );
			bool GenerateInitialTriangle( const Surface& surface, const Vector& surfacePoint, const GenerationParameters& genParms );
			bool GenerateNewTriangle( const Surface& surface, const GenerationParameters& genParms );
			Edge* FindEdge( Vertex* vertex0, Vertex* vertex1 );
			static double CalculateInteriorAngle( Vertex* vertex0, Vertex* vertex1, Vertex* vertex2 );
			Vertex* FindVertexForEdge( Edge* processEdge, Vertex*& ccwVertex, Vertex*& cwVertex, Plane& edgePlane, const GenerationParameters& genParms );
			void EdgeProcessed( Edge* edge );
			bool FrontierPointIsAcceptable( const Vector& point, Triangle* triangle, const GenerationParameters& genParms );
			bool FrontierPointIsAcceptable( const Vector& point, const Vector& normal, Triangle* triangle, const GenerationParameters& genParms );

			Utilities::List vertexList;
			Utilities::List triangleList;
			Utilities::List edgeQueue;
			Utilities::List processedEdges;
			int visitationKey;
		};

		// Tell us if the given point is on any current component of the surface.
		bool IsPointOnSurface( const Vector& point, double epsilon ) const;

		// Generate all components of the given surface.
		bool Generate( const Surface& surface, const GenerationParameters& genParms );

		// Provide what might be a convenient way to render the mesh.
		void Render( RenderInterface& renderInterface, bool forDebug ) const;

	private:

		// Keep a list of all path-connected components of the surface.
		Utilities::List componentList;
	};
}

// SurfaceMesh.h