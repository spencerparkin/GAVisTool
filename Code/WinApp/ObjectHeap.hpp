// ObjectHeap.hpp

/*
 * Copyright (C) 2013-2014 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

//=========================================================================================
template< typename ObjectType >
inline ObjectHeap< ObjectType >::ObjectHeap( int objectArraySize )
{
	this->objectArraySize = objectArraySize;
	objectArray = new ObjectType[ objectArraySize ];
	objectArrayIndex = 0;
}

//=========================================================================================
template< typename ObjectType >
inline /*virtual*/ ObjectHeap< ObjectType >::~ObjectHeap( void )
{
	delete[] objectArray;
	objectArraySize = 0;
	objectArrayIndex = 0;
}

//=========================================================================================
template< typename ObjectType >
inline ObjectType* ObjectHeap< ObjectType >::Allocate( void )
{
	if( objectArrayIndex >= objectArraySize )
		return 0;
	return &objectArray[ objectArrayIndex++ ];
}

//=========================================================================================
template< typename ObjectType >
inline ObjectType* ObjectHeap< ObjectType >::AllocateFresh( void )
{
	ObjectType* object = Allocate();
	if( object )
		object->Reset();
	return object;
}

//=========================================================================================
template< typename ObjectType >
inline void ObjectHeap< ObjectType >::FreeAll( void )
{
	objectArrayIndex = 0;
}

//=========================================================================================
template< typename ObjectType >
inline ObjectType* ObjectHeap< ObjectType >::Access( int index )
{
	if( index < 0 || index >= objectArrayIndex )
		return 0;
	return &objectArray[ index ];
}

//=========================================================================================
template< typename ObjectType >
int ObjectHeap< ObjectType >::AllocationCount( void )
{
	return objectArrayIndex;
}

// ObjectHeap.hpp