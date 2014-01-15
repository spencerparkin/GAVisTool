// InventoryTree.h

/*
 * Copyright (C) 2013-2014 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#include "Calculator/CalcLib.h"
#include "wxAll.h"

//=========================================================================================
class GAVisToolInventoryTree : public wxTreeCtrl
{
public:

	class Data : public wxTreeItemData
	{
	public:

		Data( int id );
		virtual ~Data( void );

		int id;
	};

	class Item : public CalcLib::CalcLibClass
	{
		DECLARE_CALCLIB_CLASS( GAVisToolInventoryTree::Item );

	public:

		Item( void );
		virtual ~Item( void );

		virtual void AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const = 0;
		virtual bool AddContextMenuItems( wxTreeCtrl* treeCtrl, wxMenu* menu, int highestUnusedID ) { return false; }

		int ID( void ) const;

		int id;
		static int nextId;
	};

	GAVisToolInventoryTree( wxWindow* parent );
	virtual ~GAVisToolInventoryTree( void );

	enum
	{
		ID_InventoryTree = wxID_HIGHEST,
		ID_InventoryTree_DeleteItem,
		ID_InventoryTree_Unused,
	};

	void OnSelectionChanged( wxTreeEvent& event );
	void OnItemContextMenu( wxTreeEvent& event );
	void OnDeleteContextItem( wxCommandEvent& event );

	void RegenerationNeeded( void );
	void RegenerateTreeIfNeeded( void );
	void RegenerateTree( void );

private:

	Item* LookupInventoryItem( wxTreeEvent& event );
	Item* contextItem;
	bool needsRegen;

	DECLARE_EVENT_TABLE()
};

// InventoryTree.h