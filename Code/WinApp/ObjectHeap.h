// ObjectHeap.h

/*
 * Copyright (C) 2013-2014 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

//=========================================================================================
template< typename ObjectType >
class ObjectHeap
{
public:

	ObjectHeap( int objectArraySize );
	virtual ~ObjectHeap( void );

	ObjectType* Allocate( void );
	ObjectType* AllocateFresh( void );
	void FreeAll( void );

	ObjectType* Access( int index );
	int AllocationCount( void );

private:

	ObjectType* objectArray;
	int objectArraySize;
	int objectArrayIndex;
};

#include "ObjectHeap.hpp"

// ObjectHeap.h