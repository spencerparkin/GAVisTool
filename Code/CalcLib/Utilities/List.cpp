// List.cpp

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

//=========================================================================================
List::Item::Item( void )
{
	left = 0;
	right = 0;
	list = 0;
}

//=========================================================================================
/*virtual*/ List::Item::~Item( void )
{
}

//=========================================================================================
List::Item* List::Item::Left( void ) const
{
	return left;
}

//=========================================================================================
List::Item* List::Item::Right( void ) const
{
	return right;
}

//=========================================================================================
List* List::Item::ContainingList( void ) const
{
	return list;
}

//=========================================================================================
bool List::Item::IsLeftOf( const Item* item ) const
{
	while( item && item->left != this )
		item = item->left;
	if( !item )
		return false;
	return true;
}

//=========================================================================================
bool List::Item::IsRightOf( const Item* item ) const
{
	while( item && item->right != this )
		item = item->right;
	if( !item )
		return false;
	return true;
}

//=========================================================================================
/*virtual*/ List::SortComparison List::Item::SortCompare( const Item* compareWithItem ) const
{
	return SORT_COMPARE_UNKNOWN;
}

//=========================================================================================
/*virtual*/ List::Item* List::Item::MakeCopy( void ) const
{
	return 0;
}

//=========================================================================================
/*virtual*/ void List::Item::DeleteThis( void )
{
	delete this;
}

//=========================================================================================
void List::Item::Link( Item* left, Item* right )
{
	this->left = left;
	this->right = right;

	if( left )
		left->right = this;
	if( right )
		right->left = this;
}

//=========================================================================================
void List::Item::Unlink( void )
{
	if( left )
		left->right = right;
	if( right )
		right->left = left;

	left = 0;
	right = 0;
}

//=========================================================================================
List::List( void )
{
	leftMost = 0;
	rightMost = 0;
	count = 0;
}

//=========================================================================================
List::List( const List& list )
{
	leftMost = 0;
	rightMost = 0;
	count = 0;

	Copy( list );
}

//=========================================================================================
/*virtual*/ List::~List( void )
{
}

//=========================================================================================
List::Item* List::LeftMost( void ) const
{
	return leftMost;
}

//=========================================================================================
List::Item* List::RightMost( void ) const
{
	return rightMost;
}

//=========================================================================================
int List::Count( void ) const
{
	return count;
}

//=========================================================================================
// This could be sped up by caching the link-list entries in an array.  (Building an index.)
// The array would become invalid when the list is manipulated in any way.
List::Item* List::operator[]( int index ) const
{
	if( index < 0 || index >= count )
		return 0;
	Item* item = 0;
	for( item = leftMost; item; item = item->right )
		if( --index < 0 )
			break;
	return item;
}

//=========================================================================================
void List::AdjustBoundsForNewlyAddedItem( Item* newlyAddedItem )
{
	if( !newlyAddedItem->left )
		leftMost = newlyAddedItem;
	if( !newlyAddedItem->right )
		rightMost = newlyAddedItem;
}

//=========================================================================================
void List::AdjustBoundsForItemToBeRemoved( Item* removedItem )
{
	if( removedItem == leftMost )
		leftMost = leftMost->right;
	if( removedItem == rightMost )
		rightMost = rightMost->left;
}

//=========================================================================================
bool List::InsertLeftOf( Item* existingItemOnRight, Item* newItemOnLeft )
{
	if( !existingItemOnRight && count > 0 )
		return false;
	if( existingItemOnRight && existingItemOnRight->list != this )
		return false;
	if( !newItemOnLeft || newItemOnLeft->list != 0 )
		return false;

	Item* right = existingItemOnRight;
	Item* left = existingItemOnRight ? existingItemOnRight->left : leftMost;

	newItemOnLeft->Link( left, right );
	newItemOnLeft->list = this;
	AdjustBoundsForNewlyAddedItem( newItemOnLeft );

	count++;
	return true;
}

//=========================================================================================
bool List::InsertRightOf( Item* existingItemOnLeft, Item* newItemOnRight )
{
	if( !existingItemOnLeft && count > 0 )
		return false;
	if( existingItemOnLeft && existingItemOnLeft->list != this )
		return false;
	if( !newItemOnRight || newItemOnRight->list != 0 )
		return false;

	Item* left = existingItemOnLeft;
	Item* right = existingItemOnLeft ? existingItemOnLeft->right : rightMost;

	newItemOnRight->Link( left, right );
	newItemOnRight->list = this;
	AdjustBoundsForNewlyAddedItem( newItemOnRight );

	count++;
	return true;
}

//=========================================================================================
bool List::InsertSorted( Item* newItem, SortOrder sortOrder )
{
	if( !InsertLeftOf( leftMost, newItem ) )
		return false;
	return Sort( sortOrder );
}

//=========================================================================================
bool List::Remove( Item* existingItem, bool deleteItem /*= false*/ )
{
	if( !existingItem || existingItem->list != this )
		return false;

	AdjustBoundsForItemToBeRemoved( existingItem );
	existingItem->Unlink();
	existingItem->list = 0;

	if( deleteItem )
		existingItem->DeleteThis();

	count--;
	return true;
}

//=========================================================================================
bool List::RemoveAll( bool deleteItems /*= false*/ )
{
	while( count > 0 )
		if( !Remove( leftMost, deleteItems ) )
			return false;
	return true;
}

//=========================================================================================
bool List::Copy( const List& list, bool deleteItems /*= false*/ )
{
	if( !RemoveAll( deleteItems ) )
		return false;
	return ConcatinateCopyOnRight( list );
}

//=========================================================================================
bool List::ConcatinateCopyOnLeft( const List& list )
{
	for( Item* item = list.rightMost; item; item = item->left )
		if( !InsertLeftOf( leftMost, item->MakeCopy() ) )
			return false;
	return true;
}

//=========================================================================================
bool List::ConcatinateCopyOnRight( const List& list )
{
	for( Item* item = list.leftMost; item; item = item->right )
		if( !InsertRightOf( rightMost, item->MakeCopy() ) )
			return false;
	return true;
}

//=========================================================================================
// Empty this list into the given list.
bool List::EmptyIntoOnLeft( List& list )
{
	while( count > 0 )
	{
		Item* item = rightMost;
		if( !Remove( item, false ) )
			return false;
		if( !list.InsertLeftOf( list.leftMost, item ) )
			return false;
	}
	return true;
}

//=========================================================================================
// Empty this list into the given list.
bool List::EmptyIntoOnRight( List& list )
{
	// This is certainly a slow way to do it.
	// We could just hook up one node to another
	// to concatinate the lists, but we have
	// to iterate over all of the nodes anyway
	// to update the list ownership pointer.
	while( count > 0 )
	{
		Item* item = leftMost;
		if( !Remove( item, false ) )
			return false;
		if( !list.InsertRightOf( list.rightMost, item ) )
			return false;
	}
	return true;
}

//=========================================================================================
bool List::Split( Item* pivotItem, List* leftList, List* rightList, ItemPivotType itemPivotType )
{
	if( !pivotItem || pivotItem->list != this )
		return false;
	if( ( leftList && leftList->Count() != 0 ) || ( rightList && rightList->Count() != 0 ) )
		return false;

	List* populatingList = leftList;
	while( count > 0 )
	{
		Item* currentItem = leftMost;
		Remove( currentItem, false );
		if( currentItem != pivotItem )
		{
			if( !populatingList )
				return false;
			populatingList->InsertRightOf( populatingList->rightMost, currentItem );
		}
		else
		{
			populatingList = rightList;
			switch( itemPivotType )
			{
				case ITEM_PIVOT_TYPE_PART_OF_LEFT_LIST:
				{
					if( !leftList )
						return false;
					leftList->InsertRightOf( leftList->rightMost, currentItem );
					break;
				}
				case ITEM_PIVOT_TYPE_PART_OF_RIGHT_LIST:
				{
					if( !rightList )
						return false;
					rightList->InsertLeftOf( rightList->leftMost, currentItem );
					break;
				}
			}
		}
	}

	if( itemPivotType == ITEM_PIVOT_TYPE_PART_OF_NEITHER_LIST )
		InsertRightOf( leftMost, pivotItem );

	return true;
}

//=========================================================================================
// This routine implements bubble sort.  There are some interesting things worth mentioning
// here about this algorithm.  First of all, the best and worst case running times of this
// algorithm are O(n) and O(n^2).  Already sorted or already partially sorted lists are going
// to sort in about O(n) time complexity, provided each iteration of the algorithm scans the
// list in the best direction, which, admittedly, we make no attempt to do here.  (I'm not
// sure what hueristic, if any, we would use to figure that out anyway.)  Secondly, it is
// important that the list-item sort-comparison routine be both commutative and transitive.
// Otherwise, this algorithm is not guarenteed to terminate.  For example, if we're to sort
// the list {A,B}, and A compares as A<B with respect to B, and B compares as B<A with respect
// to A, then there is no order of the list {A,B} that is sorted.  Also, if we're to sort
// the list {A,B,C}, and A compares with B (in any order) as A<B, and B with C as B<C and
// C with A as C<A, then, unlike the previous example, this is consistent, but here again
// there does not exist an order of the list that is sorted.  Transitivity means that A<B
// B<C implies that A<C.
bool List::Sort( SortOrder sortOrder, int* adjacentSwapCount /*= 0*/ )
{
	if( adjacentSwapCount )
		*adjacentSwapCount = 0;

	if( count <= 1 )
		return true;

	bool swapMade;

	do
	{
		swapMade = false;

		// The correctness of our algorithm here does not depend upon the direction in which
		// we scan the list.  However, our efficiency in sorting an already partially sorted
		// list does depend upon this direction.  For example, if the list would be already
		// sorted if but for a single element, then we want only a single pass of the list as
		// necessary to sort it.
		Item* left, *right;
		if( sortOrder == SORT_ORDER_ASCENDING )
		{
			left = leftMost;
			right = leftMost->right;
		}
		else if( sortOrder == SORT_ORDER_DESCENDING )
		{
			left = rightMost->left;
			right = rightMost;
		}
		while( left && right )
		{
			SortComparison comparison = left->SortCompare( right );
			if( comparison == SORT_COMPARE_UNKNOWN )
				return false;

			if( comparison != SORT_COMPARE_EQUAL_TO )
			{
				bool swapNeeded = false;
				if( comparison == SORT_COMPARE_LESS_THAN && sortOrder == SORT_ORDER_DESCENDING )
					swapNeeded = true;
				if( comparison == SORT_COMPARE_GREATER_THAN && sortOrder == SORT_ORDER_ASCENDING )
					swapNeeded = true;

				if( swapNeeded )
				{
					Remove( left, false );
					InsertRightOf( right, left );
					left = left->left;
					right = right->right;
					swapMade = true;

					if( adjacentSwapCount )
						( *adjacentSwapCount )++;
				}
			}

			if( sortOrder == SORT_ORDER_ASCENDING )
			{
				left = left->right;
				right = right->right;
			}
			else if( sortOrder == SORT_ORDER_DESCENDING )
			{
				left = left->left;
				right = right->left;
			}
		}
	}
	while( swapMade );

	return true;
}

//=========================================================================================
bool List::Reverse( void )
{
	if( count > 1 )
	{
		Item* item = leftMost;
		Remove( item, false );
		Reverse();
		InsertRightOf( rightMost, item );
	}

	return true;
}

// List.cpp