// Map.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

//=========================================================================================
template< typename EntryType >
class Utilities::Map
{
	friend class Iterator;

public:

	Map( int hashTableSize = 512 );
	virtual ~Map( void );

	bool Lookup( const char* entryName, EntryType* entry = 0 );
	bool Insert( const char* entryName, EntryType entry );
	bool Remove( const char* entryName );
	bool RemoveAll( void );
	bool DeleteAndRemoveAll( void );
	int Count( void ) const;

private:

	class TableEntry : public List::Item
	{
	public:
		TableEntry( void );
		virtual ~TableEntry( void );

		EntryType entry;
		int hash;

		// We don't need this to implement the hash, but, given a
		// table entry, it would be nice to know what the key is.
		char entryName[ 512 ];
	};

	int HashFunction( const char* entryName );
	TableEntry* LookupTableEntry( const char* entryNameList, List** tableEntryList = 0, int* tableEntryHash = 0 );

	List* hashTable;
	int hashTableSize;
	int count;

public:

	// One must be careful about mutating the hash while also iterating the hash.
	// This iterator assumes a static hash table while iterating over it.
	class Iterator
	{
	public:

		Iterator( const Map* map );
		virtual ~Iterator( void );

		void Reset( const Map* map );
		bool Finished( void );
		EntryType CurrentEntry( const char** entryName = 0 );
		void Next( void );

	private:

		void FindNextEntry( void );

		const Map* map;
		int tableIndex;
		TableEntry* tableEntry;
	};
};

#include "Map.hpp"

// Map.h