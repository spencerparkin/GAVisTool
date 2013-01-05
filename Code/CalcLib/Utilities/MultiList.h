// MultiList.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

//=========================================================================================
// Instances of this class represent lists.  Unlike the
// regular list class, however, items of these lists can
// exist on more then one list.  If at all possible, you
// should probably use the non-multi-style list class List
// instead of this class.  List will be much faster than
// this class.
class Utilities::MultiList
{
public:

	class Item;

private:

	class ItemData : public List::Item
	{
	public:
	
		ItemData( void );
		virtual ~ItemData( void );
	
		MultiList::Item* right;
		MultiList::Item* left;
		MultiList* list;
	};
	
public:

	class ItemDataStorage
	{
		friend class MultiList;

	public:

		ItemDataStorage( int storageSize );
		virtual ~ItemDataStorage( void );

	private:

		ItemData* Allocate( void );
		bool Deallocate( ItemData* itemData );

		List list;
		ItemData* storage;
		int storageSize;
	};

private:

	static ItemDataStorage defaultItemDataStorage;

public:

	MultiList( ItemDataStorage* itemDataStorage = &defaultItemDataStorage );
	virtual ~MultiList( void );
	
	// If the user puts our type of items into a non-multi-list types,
	// they must know what they are doing!  The multi-list class will
	// always assume that an item is not a member of a non-multi-list type.
	// Deriving from List::Item here is debatable, especially since it
	// can cause confusion and perhaps bad usage which may lead to bugs.
	// The main purpose for deriving from it here is that of being able
	// to easily re-use code already written in the List class, such as
	// Sort(), for example.
	class Item : public List::Item
	{
		friend class MultiList;

	public:

		Item( void );
		virtual ~Item( void );

		// List traversal:
		Item* Left( const MultiList* list ) const;
		Item* Right( const MultiList* list ) const;

		bool IsMemberOfList( const MultiList* list );

		// You may choose to implement this interface.
		virtual Item* MakeCopy( void ) const;
		virtual void DeleteThis( void );

	private:
		
		// This is a linear search, which shouldn't be too bad
		// if an item is not on too many different lists.  As a
		// caching scheme, subsequent and immediate queries using
		// the same criteria here are O(1).
		ItemData* FindItemData( const MultiList* list ) const;

		bool Link( Item* left, Item* right, MultiList* list );
		bool Unlink( MultiList* list );
		
		mutable List itemDataList;
	};

	// List accessors:
	Item* LeftMost( void ) const;
	Item* RightMost( void ) const;
	int Count( void ) const;
	
	// List manipulation:
	bool InsertLeftOf( Item* existingItemOnRight, Item* newItemOnLeft );
	bool InsertRightOf( Item* existingItemOnLeft, Item* newItemOnRight );
	bool Remove( Item* existingItem, bool deleteItem = false );
	bool RemoveAll( bool deleteItems = false );
	bool Copy( const MultiList& list, bool deleteItems = false );
	bool Sort( List::SortOrder sortOrder, int* adjacentSwapCount = 0 );

	// Set theoretic functions:
	bool Assign( const MultiList& list, bool deleteItems = false );
	bool Union( const MultiList& listA, const MultiList& listB, bool deleteItems = false );
	bool Intersect( const MultiList& listA, const MultiList& listB, bool deleteItems = false );
	bool Subtract( const MultiList& list, bool deleteItems = false );
	bool IsSubsetOf( const MultiList& list ) const;
	bool IsSupersetOf( const MultiList& list ) const;
	
private:

	void BindToList( List& list );
	void UnbindFromList( List& list );

	void AdjustBoundsForNewlyAddedItem( Item* newlyAddedItem );
	void AdjustBoundsForItemToBeRemoved( Item* removedItem );

	ItemData* AllocateItemData( void );
	bool DeallocateItemData( ItemData* itemData );

	Item* leftMost;
	Item* rightMost;
	int count;
	ItemDataStorage* itemDataStorage;
};

// MultiList.h