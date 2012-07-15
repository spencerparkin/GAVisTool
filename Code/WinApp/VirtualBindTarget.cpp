// VirtualBindTarget.cpp

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "VirtualBindTarget.h"

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( GAVisToolVirtualBindTarget, GAVisToolBindTarget );

//=========================================================================================
GAVisToolVirtualBindTarget::GAVisToolVirtualBindTarget( BindType bindType ) : GAVisToolBindTarget( bindType )
{
}

//=========================================================================================
/*virtual*/ GAVisToolVirtualBindTarget::~GAVisToolVirtualBindTarget( void )
{
}

//=========================================================================================
/*static*/ GAVisToolBindTarget* GAVisToolVirtualBindTarget::Create( BindType bindType )
{
	return new GAVisToolVirtualBindTarget( bindType );
}

//=========================================================================================
/*virtual*/ void GAVisToolVirtualBindTarget::DecomposeFrom( const GeometricAlgebra::SumOfBlades& element )
{
	this->element.AssignSumOfBlades( element );
}

//=========================================================================================
/*virtual*/ void GAVisToolVirtualBindTarget::ComposeTo( GeometricAlgebra::SumOfBlades& element ) const
{
	element.AssignSumOfBlades( this->element );
}

//=========================================================================================
/*virtual*/ void GAVisToolVirtualBindTarget::DumpInfo( char* printBuffer, int printBufferSize ) const
{
	sprintf_s( printBuffer, printBufferSize,
			"The variable \"%s\" is not being interpreted in any way, but it "
			"can be made part of the dependency graph of constraints.",
			name );
}

//=========================================================================================
/*virtual*/ void GAVisToolVirtualBindTarget::AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const
{
	wxString itemName = wxString::Format( wxT( "Virtual: %s" ), name );
	treeCtrl->AppendItem( parentItem, itemName, -1, -1, new GAVisToolInventoryTree::Data( id ) );
}

// VirtualBindTarget.cpp