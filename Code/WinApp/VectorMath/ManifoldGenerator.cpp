// ManifoldGenerator.cpp

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "ManifoldGenerator.h"

using namespace VectorMath;

//=============================================================================
Manifold::Manifold( void )
{
}

//=============================================================================
/*virtual*/ Manifold::~Manifold( void )
{
}

//=============================================================================
bool Manifold::IsPointOnManifold( const Vector& point, double epsilon )
{
	return false;
}

//=============================================================================
ManifoldGenerator::ManifoldGenerator( void )
{
}

//=============================================================================
/*virtual*/ ManifoldGenerator::~ManifoldGenerator( void )
{
}

//=============================================================================
bool ManifoldGenerator::GenerateManifold( const Surface& surface, const Vector& seed, Manifold& manifold )
{
	return false;
}

// ManifoldGenerator.cpp