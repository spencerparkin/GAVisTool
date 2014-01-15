// MathAssert.cpp

/*
 * Copyright (C) 2013-2014 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "MathAssert.h"
#include "wxAll.h"

//=========================================================================================
MathAssert::MathAssert( void )
{
}

//=========================================================================================
/*virtual*/ MathAssert::~MathAssert( void )
{
}

//=========================================================================================
/*virtual*/ void MathAssert::AssertionFailure( const char* messageString )
{
	wxASSERT_MSG( false, messageString );
}

// MathAssert.cpp