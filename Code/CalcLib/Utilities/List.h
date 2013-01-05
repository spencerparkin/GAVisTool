// List.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

//=========================================================================================
// Instances of this class represent lists.  These are
// sets of items given in a specific order.  Admittedly,
// the typical list is thought of as a list of items from
// top to bottom, but here these lists are lists of items
// from left to right.
class Utilities::List
{
	friend class MultiList;

public:

	// Lists can be sorted.
	enum SortOrder
	{
		SORT_ORDER_ASCENDING,
		SORT_ORDER_DESCENDING,
	};

	// These are used to indicate the comparison between two list items.
	enum SortComparison
	{
		SORT_COMPARE_LESS_THAN,
		SORT_COMPARE_GREATER_THAN,
		SORT_COMPARE_EQUAL_TO,
		SORT_COMPARE_UNKNOWN,
	};

	// These are used in the split method.
	enum ItemPivotType
	{
		ITEM_PIVOT_TYPE_PART_OF_LEFT_LIST,
		ITEM_PIVOT_TYPE_PART_OF_RIGHT_LIST,
		ITEM_PIVOT_TYPE_PART_OF_NEITHER_LIST,
	};

	// Users of the list class must define a derivative of the item class.
	// An item can be a member of at most one list.  This is the only
	// significant limitation of the list class that I can think of.
	class Item
	{
		friend List;
		friend class MultiList;

	public:

		Item( void );
		virtual ~Item( void );

		// Item accessors:
		Item* Left( void ) const;
		Item* Right( void ) const;
		List* ContainingList( void ) const;

		// Queries:
		bool IsLeftOf( const Item* item ) const;
		bool IsRightOf( const Item* item ) const;

		// You may choose to implement this interface.
		virtual SortComparison SortCompare( const Item* compareWithItem ) const;
		virtual Item* MakeCopy( void ) const;
		virtual void DeleteThis( void );

	private:

		void Link( Item* prev, Item* next );
		void Unlink( void );

		Item* left;
		Item* right;
		List* list;
	};

	// List construction/destruction:
	List( void );
	List( const List& list );
	virtual ~List( void );

	// List accessors:
	Item* LeftMost( void ) const;
	Item* RightMost( void ) const;
	int Count( void ) const;
	Item* operator[]( int index ) const;

	// List manipulation:
	bool InsertLeftOf( Item* existingItemOnRight, Item* newItemOnLeft );
	bool InsertRightOf( Item* existingItemOnLeft, Item* newItemOnRight );
	bool InsertSorted( Item* newItem, SortOrder sortOrder );
	bool Remove( Item* existingItem, bool deleteItem = false );
	bool RemoveAll( bool deleteItems = false );
	bool Copy( const List& list, bool deleteItems = false );
	bool ConcatinateCopyOnLeft( const List& list );
	bool ConcatinateCopyOnRight( const List& list );
	bool EmptyIntoOnLeft( List& list );
	bool EmptyIntoOnRight( List& list );
	bool Split( Item* pivotItem, List* leftList, List* rightList, ItemPivotType itemPivotType );
	bool Sort( SortOrder sortOrder, int* adjacentSwapCount = 0 );
	bool Reverse( void );

private:

	void AdjustBoundsForNewlyAddedItem( Item* newlyAddedItem );
	void AdjustBoundsForItemToBeRemoved( Item* removedItem );

	Item* leftMost;
	Item* rightMost;
	int count;
};

// List.h