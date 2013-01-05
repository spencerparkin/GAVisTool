// InventoryTree.cpp

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "InventoryTree.h"
#include "Application.h"

//=========================================================================================
GAVisToolInventoryTree::Data::Data( int id )
{
	this->id = id;
}

//=========================================================================================
/*virtual*/ GAVisToolInventoryTree::Data::~Data( void )
{
}

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS0( GAVisToolInventoryTree::Item );

//=========================================================================================
int GAVisToolInventoryTree::Item::nextId = 0;

//=========================================================================================
GAVisToolInventoryTree::Item::Item( void )
{
	id = nextId++;
}

//=========================================================================================
/*virtual*/ GAVisToolInventoryTree::Item::~Item( void )
{
}

//=========================================================================================
int GAVisToolInventoryTree::Item::ID( void ) const
{
	return id;
}

//=========================================================================================
BEGIN_EVENT_TABLE( GAVisToolInventoryTree, wxTreeCtrl )
	EVT_TREE_SEL_CHANGED( GAVisToolInventoryTree::ID_InventoryTree, GAVisToolInventoryTree::OnSelectionChanged )
	EVT_TREE_ITEM_MENU( GAVisToolInventoryTree::ID_InventoryTree, GAVisToolInventoryTree::OnItemContextMenu )
	EVT_MENU( GAVisToolInventoryTree::ID_InventoryTree_DeleteItem, GAVisToolInventoryTree::OnDeleteContextItem )
END_EVENT_TABLE()

//=========================================================================================
GAVisToolInventoryTree::GAVisToolInventoryTree( wxWindow* parent ) : wxTreeCtrl( parent, ID_InventoryTree, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT )
{
	contextItem = 0;
	needsRegen = false;
}

//=========================================================================================
/*virtual*/ GAVisToolInventoryTree::~GAVisToolInventoryTree( void )
{
}

//=========================================================================================
void GAVisToolInventoryTree::RegenerationNeeded( void )
{
	needsRegen = true;
}

//=========================================================================================
void GAVisToolInventoryTree::RegenerateTreeIfNeeded( void )
{
	if( needsRegen )
	{
		RegenerateTree();
		needsRegen = false;
	}
}

//=========================================================================================
void GAVisToolInventoryTree::RegenerateTree( void )
{
	DeleteAllItems();

	wxTreeItemId rootItem = AddRoot( wxT( "Inventory" ), 0 );

	wxTreeItemId constraintsItem = AppendItem( rootItem, wxT( "Constraints" ), 0 );
	wxTreeItemId bindTargetsItem = AppendItem( rootItem, wxT( "Bind Targets" ), 0 );

	const Utilities::List& constraintList = wxGetApp().environment->ConstraintList();
	const GAVisToolConstraint* constraint = ( const GAVisToolConstraint* )constraintList.LeftMost();
	while( constraint )
	{
		constraint->AddInventoryTreeItem( this, constraintsItem );
		constraint = ( const GAVisToolConstraint* )constraint->Right();
	}

	const Utilities::List& bindTargetList = wxGetApp().environment->BindTargetList();
	const GAVisToolBindTarget* bindTarget = ( const GAVisToolBindTarget* )bindTargetList.LeftMost();
	while( bindTarget )
	{
		bindTarget->AddInventoryTreeItem( this, bindTargetsItem );
		bindTarget = ( const GAVisToolBindTarget* )bindTarget->Right();
	}

	Expand( constraintsItem );
	Expand( bindTargetsItem );
}

//=========================================================================================
GAVisToolInventoryTree::Item* GAVisToolInventoryTree::LookupInventoryItem( wxTreeEvent& event )
{
	wxTreeItemId treeItem = event.GetItem();
	if( !treeItem.IsOk() )
		return 0;

	Data* data = ( Data* )GetItemData( treeItem );
	if( !data )
		return 0;

	GAVisToolEnvironment* visToolEnv = wxGetApp().environment;
	GAVisToolInventoryTree::Item* invItem = visToolEnv->LookupInventoryItemByID( data->id );
	return invItem;
}

//=========================================================================================
void GAVisToolInventoryTree::OnSelectionChanged( wxTreeEvent& event )
{
	wxGetApp().canvasFrame->canvas->SetSelection( "" );
	GAVisToolInventoryTree::Item* invItem = LookupInventoryItem( event );
	if( invItem )
	{
		GAVisToolGeometry* geometry = invItem->Cast< GAVisToolGeometry >();
		if( geometry )
			wxGetApp().canvasFrame->canvas->SetSelection( geometry->GetName() );
	}

	wxGetApp().canvasFrame->canvas->RedrawNeeded( true );
}

//=========================================================================================
void GAVisToolInventoryTree::OnItemContextMenu( wxTreeEvent& event )
{
	contextItem = LookupInventoryItem( event );
	if( !contextItem )
		return;

	// Build the context menu for the inventory item.
	wxMenu contextMenu;
	if( contextItem->AddContextMenuItems( this, &contextMenu, ID_InventoryTree_Unused ) )
		contextMenu.AppendSeparator();
	contextMenu.Append( ID_InventoryTree_DeleteItem, wxT( "Delete" ) );

	// Pop it up at the cursor.
	wxPoint point = event.GetPoint();
	PopupMenu( &contextMenu, point );

	event.Skip();
}

//=========================================================================================
void GAVisToolInventoryTree::OnDeleteContextItem( wxCommandEvent& event )
{
	if( !contextItem )
		return;

	GAVisToolBindTarget* bindTarget = contextItem->Cast< GAVisToolBindTarget >();
	if( bindTarget )
	{
		wxString consoleInput = wxString::Format( wxT( "unbind( %s )" ), bindTarget->GetName() );
		wxGetApp().consoleFrame->ProcessConsoleInput( consoleInput );
		wxGetApp().canvasFrame->canvas->RedrawNeeded( true );
		return;
	}

	GAVisToolConstraint* constraint = contextItem->Cast< GAVisToolConstraint >();
	if( constraint )
	{
		wxGetApp().environment->RemoveConstraint( constraint );
		delete constraint;
	}
}

// InventoryTree.cpp