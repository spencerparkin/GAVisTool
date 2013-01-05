// MultiList.cpp

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "Utilities.h"

using namespace Utilities;

/*static*/ MultiList::ItemDataStorage MultiList::defaultItemDataStorage(0);

//=========================================================================================
MultiList::MultiList( ItemDataStorage* itemDataStorage /*= &defaultItemDataStorage*/ )
{
	this->itemDataStorage = itemDataStorage;
	leftMost = 0;
	rightMost = 0;
	count = 0;
}

//=========================================================================================
/*virtual*/ MultiList::~MultiList( void )
{
	RemoveAll( false );
}

//=========================================================================================
MultiList::Item* MultiList::LeftMost( void ) const
{
	return leftMost;
}

//=========================================================================================
MultiList::Item* MultiList::RightMost( void ) const
{
	return rightMost;
}

//=========================================================================================
int MultiList::Count( void ) const
{
	return count;
}

//=========================================================================================
bool MultiList::InsertLeftOf( Item* existingItemOnRight, Item* newItemOnLeft )
{
	if( !existingItemOnRight && count > 0 )
		return false;
	if( existingItemOnRight && !existingItemOnRight->IsMemberOfList( this ) )
		return false;
	if( !newItemOnLeft || newItemOnLeft->IsMemberOfList( this ) )
		return false;

	Item* right = existingItemOnRight;
	Item* left = existingItemOnRight ? existingItemOnRight->Left( this ) : leftMost;

	if( !newItemOnLeft->Link( left, right, this ) )
		return false;
	AdjustBoundsForNewlyAddedItem( newItemOnLeft );

	count++;
	return true;
}

//=========================================================================================
bool MultiList::InsertRightOf( Item* existingItemOnLeft, Item* newItemOnRight )
{
	if( !existingItemOnLeft && count > 0 )
		return false;
	if( existingItemOnLeft && !existingItemOnLeft->IsMemberOfList( this ) )
		return false;
	if( !newItemOnRight || newItemOnRight->IsMemberOfList( this ) )
		return false;

	Item* left = existingItemOnLeft;
	Item* right = existingItemOnLeft ? existingItemOnLeft->Right( this ) : rightMost;

	if( !newItemOnRight->Link( left, right, this ) )
		return false;
	AdjustBoundsForNewlyAddedItem( newItemOnRight );

	count++;
	return false;
}

//=========================================================================================
bool MultiList::Remove( Item* existingItem, bool deleteItem /*= false*/ )
{
	if( !existingItem || !existingItem->IsMemberOfList( this ) )
		return false;

	AdjustBoundsForItemToBeRemoved( existingItem );
	if( !existingItem->Unlink( this ) )
		return false;		// If this really failed, it could leave the list in a bad state!

	// If we're the last list hanging on to this item, we're about
	// to let it go, and it should be deleted, then do it.
	if( existingItem->itemDataList.Count() == 0 && deleteItem )
		existingItem->DeleteThis();

	count--;
	return true;
}

//=========================================================================================
bool MultiList::RemoveAll( bool deleteItems /*= false*/ )
{
	while( count > 0 )
		if( !Remove( leftMost, deleteItems ) )
			return false;
	return true;
}

//=========================================================================================
// Notice that this actually copies the items in one list and puts those copies
// into this list.  This is not the same as doing an assignment, which would
// simply make all members of the given list also members of this list.
bool MultiList::Copy( const MultiList& list, bool deleteItems /*= false*/ )
{
	if( !RemoveAll( deleteItems ) )
		return false;
	
	for( Item* item = list.leftMost; item; item = item->Right( &list ) )
		if( !InsertRightOf( rightMost, item->MakeCopy() ) )
			return false;

	return true;
}

//=========================================================================================
bool MultiList::Sort( List::SortOrder sortOrder, int* adjacentSwapCount /*= 0*/ )
{
	// Yeah, this is slower than if we just hunkered down and sorted
	// the list ourselves, but the multi-list class is a bit slow and
	// heavy-weight anyway.  The bind and unbind here may not be where
	// optimization is needed here anyway.
	List list;
	BindToList( list );
	list.Sort( sortOrder, adjacentSwapCount );
	UnbindFromList( list );
	return true;
}

//=========================================================================================
void MultiList::BindToList( List& list )
{
	list.leftMost = leftMost;
	list.rightMost = rightMost;
	list.count = count;
	for( Item* item = leftMost; item; item = item->Right( this ) )
	{
		Item* left = item->Left( this );
		Item* right = item->Right( this );
		if( left )
			item->left = left;
		if( right )
			item->right = right;
		item->list = &list;
	}
}

//=========================================================================================
void MultiList::UnbindFromList( List& list )
{
	for( Item* item = ( Item* )list.leftMost; item; item = ( Item* )item->right )
	{
		ItemData* itemData = item->FindItemData( this );
		itemData->left = ( Item* )item->left;
		itemData->right = ( Item* )item->right;
		item->left = 0;
		item->right = 0;
		item->list = 0;
	}
	list.leftMost = 0;
	list.rightMost = 0;
	list.count = 0;
}

//=========================================================================================
// See the comment header for the copy method.
bool MultiList::Assign( const MultiList& list, bool deleteItems /*= false*/ )
{
	if( !RemoveAll( deleteItems ) )
		return false;

	for( Item* item = list.leftMost; item; item = item->Right( &list ) )
		if( !InsertRightOf( rightMost, item ) )
			return false;

	return true;
}

//=========================================================================================
bool MultiList::Union( const MultiList& listA, const MultiList& listB, bool deleteItems /*= false*/ )
{
	if( !RemoveAll( deleteItems ) )
		return false;

	for( Item* item = listA.leftMost; item; item = item->Right( &listA ) )
		if( !InsertRightOf( rightMost, item ) )
			return false;

	for( Item* item = listB.leftMost; item; item = item->Right( &listB ) )
		if( !item->IsMemberOfList( &listA ) )
			if( !InsertRightOf( rightMost, item ) )
				return false;

	return true;
}

//=========================================================================================
bool MultiList::Intersect( const MultiList& listA, const MultiList& listB, bool deleteItems /*= false*/ )
{
	if( !RemoveAll( deleteItems ) )
		return false;

	for( Item* item = listA.leftMost; item; item = item->Right( &listA ) )
		if( item->IsMemberOfList( &listB ) )
			if( !InsertRightOf( rightMost, item ) )
				return false;

	for( Item* item = listB.leftMost; item; item = item->Right( &listB ) )
		if( item->IsMemberOfList( &listA ) )
			if( !InsertRightOf( rightMost, item ) )
				return false;

	return true;
}

//=========================================================================================
bool MultiList::Subtract( const MultiList& list, bool deleteItems /*= false*/ )
{
	for( Item* item = list.leftMost; item; item = item->Right( &list ) )
		if( item->IsMemberOfList( this ) )
			if( !Remove( item, deleteItems ) )
				return false;

	return true;
}

//=========================================================================================
bool MultiList::IsSubsetOf( const MultiList& list ) const
{
	for( Item* item = leftMost; item; item = item->Right( this ) )
		if( !item->IsMemberOfList( &list ) )
			return false;

	return true;
}

//=========================================================================================
bool MultiList::IsSupersetOf( const MultiList& list ) const
{
	return list.IsSubsetOf( *this );
}

//=========================================================================================
void MultiList::AdjustBoundsForNewlyAddedItem( Item* newlyAddedItem )
{
	if( !newlyAddedItem->Left( this ) )
		leftMost = newlyAddedItem;
	if( !newlyAddedItem->Right( this ) )
		rightMost = newlyAddedItem;
}

//=========================================================================================
void MultiList::AdjustBoundsForItemToBeRemoved( Item* removedItem )
{
	if( removedItem == leftMost )
		leftMost = leftMost->Right( this );
	if( removedItem == rightMost )
		rightMost = rightMost->Left( this );
}

//=========================================================================================
MultiList::ItemData* MultiList::AllocateItemData( void )
{
	ItemData* itemData = 0;
	if( itemDataStorage )
		itemData = itemDataStorage->Allocate();
	return itemData;
}

//=========================================================================================
bool MultiList::DeallocateItemData( ItemData* itemData )
{
	if( !itemDataStorage )
		return false;
	return itemDataStorage->Deallocate( itemData );
}

//=========================================================================================
MultiList::Item::Item( void )
{
}

//=========================================================================================
MultiList::Item::~Item( void )
{
}

//=========================================================================================
MultiList::Item* MultiList::Item::Left( const MultiList* list ) const
{
	const ItemData* itemData = FindItemData( list );
	if( !itemData )
		return 0;
	return itemData->left;
}

//=========================================================================================
MultiList::Item* MultiList::Item::Right( const MultiList* list ) const
{
	const ItemData* itemData = FindItemData( list );
	if( !itemData )
		return 0;
	return itemData->right;
}

//=========================================================================================
/*virtual*/ void MultiList::Item::DeleteThis( void )
{
	delete this;
}

//=========================================================================================
/*virtual*/ MultiList::Item* MultiList::Item::MakeCopy( void ) const
{
	return 0;
}

//=========================================================================================
bool MultiList::Item::IsMemberOfList( const MultiList* list )
{
	ItemData* itemData = FindItemData( list );
	if( !itemData )
		return false;
	return true;
}

//=========================================================================================
MultiList::ItemData* MultiList::Item::FindItemData( const MultiList* list ) const
{
	ItemData* foundItemData = 0;
	for( ItemData* itemData = ( ItemData* )itemDataList.LeftMost(); itemData && !foundItemData; itemData = ( ItemData* )itemData->Right() )
		if( itemData->list == list )
			foundItemData = itemData;
	if( foundItemData )
	{
		// Make sure that a subsequent query for the same thing is done in O(1) time.
		itemDataList.Remove( foundItemData, false );
		itemDataList.InsertLeftOf( itemDataList.LeftMost(), foundItemData );
	}
	return foundItemData;
}

//=========================================================================================
// A failure here could leave the list in a bad state.
bool MultiList::Item::Link( Item* left, Item* right, MultiList* list )
{
	ItemData* itemData = list->AllocateItemData();
	if( !itemData )
		return false;
	if( !itemDataList.InsertLeftOf( itemDataList.LeftMost(), itemData ) )
		return false;

	itemData->left = left;
	itemData->right = right;
	itemData->list = list;

	if( left )
	{
		itemData = left->FindItemData( list );
		if( !itemData )
			return false;
		itemData->right = this;
	}

	if( right )
	{
		itemData = right->FindItemData( list );
		if( !itemData )
			return false;
		itemData->left = this;
	}

	return true;
}

//=========================================================================================
// A failure here could leave the list in a bad state.
bool MultiList::Item::Unlink( MultiList* list )
{
	ItemData* itemData = FindItemData( list );
	if( !itemData )
		return false;
	
	Item* left = itemData->left;
	Item* right = itemData->right;

	if( left )
	{
		itemData = left->FindItemData( list );
		if( !itemData )
			return false;
		itemData->right = right;
	}

	if( right )
	{
		itemData = right->FindItemData( list );
		if( !itemData )
			return false;
		itemData->left = left;
	}

	itemData = FindItemData( list );
	if( !itemDataList.Remove( itemData, false ) )
		return false;
	if( !list->DeallocateItemData( itemData ) )
		return false;
	return true;
}

//=========================================================================================
MultiList::ItemDataStorage::ItemDataStorage( int storageSize )
{
	this->storageSize = storageSize;
	if( storageSize == 0 )
		storage = 0;
	else
	{
		storage = new ItemData[ storageSize ];
		for( int index = 0; index < storageSize; index++ )
			list.InsertRightOf( list.RightMost(), &storage[ index ] );
	}
}

//=========================================================================================
MultiList::ItemDataStorage::~ItemDataStorage( void )
{
	// Delete members of the list that we did not allocate up front.
	while( list.Count() > 0 )
	{
		ItemData* itemData = ( ItemData* )list.LeftMost();
		bool deleteItemData = true;
		if( itemData >= storage && itemData < &storage[ storageSize ] )
			deleteItemData = false;
		list.Remove( itemData, deleteItemData );
	}

	// Now free what we did allocate up front.
	if( storage )
		delete[] storage;
	storage = 0;
	storageSize = 0;
}

//=========================================================================================
MultiList::ItemData* MultiList::ItemDataStorage::Allocate( void )
{
	// Pull an item off our list.  If our list is empty, allocate from elsewhere.
	ItemData* itemData = ( ItemData* )list.LeftMost();
	if( itemData )
		list.Remove( itemData, false );
	else
		itemData = new ItemData();
	itemData->left = 0;
	itemData->right = 0;
	itemData->list = 0;
	return itemData;
}

//=========================================================================================
bool MultiList::ItemDataStorage::Deallocate( ItemData* itemData )
{
	// Put the given item on our list.
	return list.InsertLeftOf( list.LeftMost(), itemData );
}

//=========================================================================================
MultiList::ItemData::ItemData( void )
{
	left = 0;
	right = 0;
	list = 0;
}

//=========================================================================================
/*virtual*/ MultiList::ItemData::~ItemData( void )
{
}

// MultiList.cpp