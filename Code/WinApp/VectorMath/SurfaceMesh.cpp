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
}

//=============================================================================
bool SurfaceMesh::Generate( const Surface& surface, const GenerationParameters& genParms )
{
	// Go generate all the path connected components of the surface.

	return true;
}

//=============================================================================
SurfaceMesh::PathConnectedComponentGenerator::PathConnectedComponentGenerator( void )
{
}

//=============================================================================
/*virtual*/ SurfaceMesh::PathConnectedComponentGenerator::~PathConnectedComponentGenerator( void )
{
}

//=============================================================================
SurfaceMesh::PathConnectedComponent* SurfaceMesh::PathConnectedComponentGenerator::Generate(
											const Surface& surface,
											const GenerationParameters& genParms )
{
	// Create initial triangle on surface...
	//...

	while( growingPointList.Count() > 0 )
	{
		// For each growing point, determine which points should become stunted.
		// A growing point becomes stunted when it comes near another growing
		// point that it is not adjacent to in the graph.

		// For each growing point, grow the point along the surface in a direction
		// that expands it's connecting edges.

		// For each edge, determine which edges need to split and split them.
		// The edge split algorithm must maintain triangle winding and determine
		// correctly which points are growing and mature, and which edges are
		// dynamic and static.  An edge, by definition, is dynamic if and only
		// if one or more of its connected points are growing.  An edge is static
		// if it is not dynamic.
	}

	// Deal with the stunted points somehow?

	// Finally, dump all mature points into the path connected component object's point-list.

	return 0;
}

// SurfaceMesh.cpp