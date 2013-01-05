// ClassInfo.cpp

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "CalcLib.h"

using namespace CalcLib;

//=========================================================================================
/*virtual*/ bool CalcLibClass::IsTypeOf( const char* givenClassName ) const
{
	return false;
}

//=========================================================================================
Utilities::Map< CalcLibClassInfo* >* CalcLibClassInfo::classInfoMap = 0;

//=========================================================================================
CalcLibClassInfo::CalcLibClassInfo( const char* className )
{
	AllocateInheritingClassesArray(0);
	MapClass( className );
}

//=========================================================================================
CalcLibClassInfo::CalcLibClassInfo( const char* className, const char* inheritingClassName1 )
{
	AllocateInheritingClassesArray(1);
	MapClassInheritance( inheritingClassName1 );
	MapClass( className );
}

//=========================================================================================
CalcLibClassInfo::CalcLibClassInfo( const char* className, const char* inheritingClassName1, const char* inheritingClassName2 )
{
	AllocateInheritingClassesArray(2);
	MapClassInheritance( inheritingClassName1 );
	MapClassInheritance( inheritingClassName2 );
	MapClass( className );
}

//=========================================================================================
CalcLibClassInfo::CalcLibClassInfo( const char* className, const char* inheritingClassName1, const char* inheritingClassName2, const char* inheritingClassName3 )
{
	AllocateInheritingClassesArray(3);
	MapClassInheritance( inheritingClassName1 );
	MapClassInheritance( inheritingClassName2 );
	MapClassInheritance( inheritingClassName3 );
	MapClass( className );
}

//=========================================================================================
CalcLibClassInfo::~CalcLibClassInfo( void )
{
	DeleteInheritingClassesArray();
	
	// We assume that if one class-info is going down,
	// then the entire system is going down.
	if( classInfoMap )
	{
		delete classInfoMap;
		classInfoMap = 0;
	}
}

//=========================================================================================
void CalcLibClassInfo::AllocateInheritingClassesArray( int count )
{
	inhertingClassesCount = 0;
	if( count <= 0 )
		inheritingClassNames = 0;
	else
	{
		inheritingClassNames = new char*[ count ];
		for( int index = 0; index < count; index++ )
			inheritingClassNames[ index ] = 0;
	}
}

//=========================================================================================
void CalcLibClassInfo::DeleteInheritingClassesArray( void )
{
	for( int index = 0; index < inhertingClassesCount; index++ )
		delete[] inheritingClassNames[ index ];
	delete[] inheritingClassNames;
	inheritingClassNames = 0;
	inhertingClassesCount = 0;
}

//=========================================================================================
void CalcLibClassInfo::MapClass( const char* className )
{
	if( !classInfoMap )
		classInfoMap = new Utilities::Map< CalcLibClassInfo* >();
	classInfoMap->Insert( className, this );
}

//=========================================================================================
void CalcLibClassInfo::MapClassInheritance( const char* inheritingClassName )
{
	// Remember that we inherit from the given class.
	char*& className = inheritingClassNames[ inhertingClassesCount++ ];
	int inheritingClassNameLen = strlen( inheritingClassName ) + 1;
	className = new char[ inheritingClassNameLen ];
	strcpy_s( className, inheritingClassNameLen, inheritingClassName );
}

//=========================================================================================
// This RTTI mechanism is not thread-safe.  We would have to do some sort of thread
// synchronization here if we didn't find a lock-less algorithm or lock-less data stuctures.
/*static*/ bool CalcLibClassInfo::IsTypeOf( const char* childClassName, const char* parentClassName )
{
	if( 0 == strcmp( childClassName, parentClassName ) )
		return true;

	if( !classInfoMap )
		return false;

	CalcLibClassInfo* classInfo = 0;
	if( !classInfoMap->Lookup( childClassName, &classInfo ) )
		return false;

	// There can't be any circular class hierarchies, so this
	// algorithm should always terminate.
	bool parentFound = false;
	Utilities::List queue;
	queue.InsertRightOf( 0, classInfo );
	while( queue.Count() > 0 && !parentFound )
	{
		classInfo = ( CalcLibClassInfo* )queue.LeftMost();
		queue.Remove( classInfo, false );

		for( int index = 0; index < classInfo->inhertingClassesCount && !parentFound; index++ )
		{
			const char* superClassName = classInfo->inheritingClassNames[ index ];
			if( 0 == strcmp( parentClassName, superClassName ) )
				parentFound = true;
			else
			{
				CalcLibClassInfo* superClassInfo = 0;
				if( classInfoMap->Lookup( superClassName, &superClassInfo ) )
					queue.InsertRightOf( queue.RightMost(), superClassInfo );
			}
		}
	}

	queue.RemoveAll( false );
	return parentFound;
}

// ClassInfo.cpp