// BinaryFunctionSearch.cpp

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "BinaryFunctionSearch.h"
#include <math.h>

//=============================================================================
VectorMath::BinaryFunctionSearch::BinaryFunctionSearch( Function* function )
{
	this->function = function;
}

//=============================================================================
/*virtual*/ VectorMath::BinaryFunctionSearch::~BinaryFunctionSearch( void )
{
}

//=============================================================================
bool VectorMath::BinaryFunctionSearch::FindValue( double& x, double y, double x0, double x1 )
{
	double lower_x = x0;
	double upper_x = x1;
	double lower_y, upper_y;

	lower_y = function->Evaluate( lower_x );
	upper_y = function->Evaluate( upper_x );

	if( y < lower_y || y > upper_y )
		return false;

	double epsilon = 1e-7;
	double mid_x, mid_y;
	do
	{
		mid_x = ( lower_x + upper_x ) / 2.0;
		mid_y = function->Evaluate( mid_x );

		if( y == mid_y )
			break;

		if( y < mid_y )
			upper_x = mid_x;
		else
			lower_x = mid_x;
	}
	while( fabs( y - mid_y ) > epsilon );

	x = mid_x;
	return true;
}

// BinaryFunctionSearch.cpp