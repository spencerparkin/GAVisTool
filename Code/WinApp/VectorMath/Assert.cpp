// Assert.cpp

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "Assert.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

//=============================================================================
VectorMath::Assert defaultAssert;
VectorMath::Assert* VectorMath::assert = &defaultAssert;

//=============================================================================
VectorMath::Assert::Assert( void )
{
}

//=============================================================================
/*virtual*/ VectorMath::Assert::~Assert( void )
{
}

//=============================================================================
void VectorMath::Assert::Condition( bool condition, const char* formatString, ... )
{
	if( !condition )
	{
		char messageString[ 1024 ];

		va_list args;
		va_start( args, formatString );
		vsprintf_s( messageString, sizeof( messageString ), formatString, args );
		va_end( args );

		AssertionFailed( messageString );
	}
}

//=============================================================================
/*virtual*/ void VectorMath::Assert::AssertionFailed( const char* messageString )
{
	// Do nothing.
}

// Assert.cpp