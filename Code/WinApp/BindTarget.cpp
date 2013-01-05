// BindTarget.cpp

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "BindTarget.h"
#include "Application.h"

IMPLEMENT_CALCLIB_CLASS1( GAVisToolBindTarget, GAVisToolInventoryTree::Item );

//=========================================================================================
GAVisToolBindTarget::GAVisToolBindTarget( BindType bindType /*= DOESNT_MATTER*/ )
{
	this->bindType = bindType;
	name = 0;
	changed = false;
}

//=========================================================================================
/*virtual*/ GAVisToolBindTarget::~GAVisToolBindTarget( void )
{
	if( name )
		delete[] name;
}

//=========================================================================================
/*virtual*/ Utilities::List::SortComparison GAVisToolBindTarget::SortCompare( const Utilities::List::Item* compareWithItem ) const
{
	// This sort is nonsensical, but we must implement something here that is
	// a valid sort compare, because some bind target derivative classes do sort
	// in a certain order with respect to one another.
	int thisInt = int( this );
	int givenInt = int( compareWithItem );
	if( thisInt < givenInt )
		return Utilities::List::SORT_COMPARE_LESS_THAN;
	else if( thisInt > givenInt )
		return Utilities::List::SORT_COMPARE_GREATER_THAN;
	return Utilities::List::SORT_COMPARE_EQUAL_TO;
}

//=========================================================================================
/*virtual*/ void GAVisToolBindTarget::Initialize( void )
{
}

//=========================================================================================
/*virtual*/ void GAVisToolBindTarget::Finalize( void )
{
}

//=========================================================================================
void GAVisToolBindTarget::SetName( const char* name )
{
	if( this->name )
		delete[] this->name;
	int len = strlen( name ) + 1;
	this->name = new char[ len ];
	strcpy_s( this->name, len, name );
}

//=========================================================================================
const char* GAVisToolBindTarget::GetName( void ) const
{
	return name;
}

//=========================================================================================
// This needs to be called anywhere and everywhere that a bind target has been changed.
void GAVisToolBindTarget::HasChanged( bool changed )
{
	this->changed = changed;
	
	// If a geometry changes, then this warrants a redraw with a primitive cache regeneration.
	if( changed && IsTypeOf( GAVisToolGeometry::ClassName() ) )
		wxGetApp().canvasFrame->canvas->RedrawNeeded( true );
}

//=========================================================================================
bool GAVisToolBindTarget::HasChanged( void )
{
	return changed;
}

// BindTarget.cpp