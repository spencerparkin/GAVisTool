// MathAssert.h

/*
 * Copyright (C) 2013-2014 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#include "VectorMath/Assert.h"

//=========================================================================================
class MathAssert : public VectorMath::Assert
{
public:

	MathAssert( void );
	virtual ~MathAssert( void );

	virtual void AssertionFailure( const char* messageString );
};

// MathAssert.h