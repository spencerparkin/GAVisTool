// Map.hpp

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
Utilities::Map< EntryType >::Map( int hashTableSize /*= 512*/ )
{
	this->hashTableSize = hashTableSize;
	hashTable = new List[ hashTableSize ];
	count = 0;
}

//=========================================================================================
template< typename EntryType >
/*virtual*/ Utilities::Map< EntryType >::~Map( void )
{
	RemoveAll();
	delete[] hashTable;
}

//=========================================================================================
template< typename EntryType >
int Utilities::Map< EntryType >::Count( void ) const
{
	return count;
}

//=========================================================================================
template< typename EntryType >
bool Utilities::Map< EntryType >::Lookup( const char* entryName, EntryType* entry /*= 0*/ )
{
	TableEntry* tableEntry = LookupTableEntry( entryName );
	if( tableEntry )
	{
		if( entry )
			*entry = tableEntry->entry;
		return true;
	}
	return false;
}

//=========================================================================================
template< typename EntryType >
bool Utilities::Map< EntryType >::Insert( const char* entryName, EntryType entry )
{
	List* tableEntryList = 0;
	int tableEntryHash = 0;
	TableEntry* tableEntry = LookupTableEntry( entryName, &tableEntryList, &tableEntryHash );
	if( tableEntry )
		return false;

	tableEntry = new TableEntry;
	tableEntry->entry = entry;
	tableEntry->hash = tableEntryHash;
	strcpy_s( tableEntry->entryName, sizeof( tableEntry->entryName ), entryName );
	tableEntryList->InsertRightOf( tableEntryList->RightMost(), tableEntry );
	count++;
	return true;
}

//=========================================================================================
template< typename EntryType >
bool Utilities::Map< EntryType >::Remove( const char* entryName )
{
	List* tableEntryList = 0;
	TableEntry* tableEntry = LookupTableEntry( entryName, &tableEntryList );
	if( !tableEntry )
		return false;

	tableEntryList->Remove( tableEntry, true );
	count--;
	return true;
}

//=========================================================================================
template< typename EntryType >
bool Utilities::Map< EntryType >::RemoveAll( void )
{
	for( int index = 0; index < hashTableSize; index++ )
	{
		List& tableEntryList = hashTable[ index ];
		tableEntryList.RemoveAll( true );
	}
	count = 0;
	return true;
}

//=========================================================================================
// If the user creates a hash of pointers, this will call delete
// on all of the pointers in the hash.  If they didn't create a
// hash of pointers, then the program will still compile, provided
// they do not try to call this function.
template< typename EntryType >
bool Utilities::Map< EntryType >::DeleteAndRemoveAll( void )
{
	for( int index = 0; index < hashTableSize; index++ )
	{
		List& tableEntryList = hashTable[ index ];
		while( tableEntryList.Count() > 0 )
		{
			TableEntry* tableEntry = ( TableEntry* )tableEntryList.LeftMost();
			tableEntryList.Remove( tableEntry, false );
			delete tableEntry->entry;
			delete tableEntry;
		}
	}
	count = 0;
	return true;
}

//=========================================================================================
template< typename EntryType >
typename Utilities::Map< EntryType >::TableEntry* Utilities::Map< EntryType >::LookupTableEntry( const char* entryName, List** tableEntryList /*= 0*/, int* tableEntryHash /*= 0*/ )
{
	List* list = 0;
	if( !tableEntryList )
		tableEntryList = &list;

	int hash = HashFunction( entryName );
	if( tableEntryHash )
		*tableEntryHash = hash;

	int index = hash % hashTableSize;
	*tableEntryList = &hashTable[ index ];

	TableEntry* tableEntry = ( TableEntry* )( *tableEntryList )->LeftMost();
	while( tableEntry )
	{
		if( tableEntry->hash == hash )
		{
			// My hash function isn't very good.  Further qualify
			// the match by making sure that the entry names match.
			if( 0 == strcmp( tableEntry->entryName, entryName ) )
				break;
		}
		tableEntry = ( TableEntry* )tableEntry->Right();
	}

	return tableEntry;
}

//=========================================================================================
template< typename EntryType >
int Utilities::Map< EntryType >::HashFunction( const char* entryName )
{
	int hash = 0;

	// I'm just making this hash function up.  I should see if there's a better one.
	for( int index = 0; entryName[ index ] != '\0'; index++ )
	{
		int x = entryName[ index ];
		hash ^= x << ( 3 * index % 10 );
	}

	return hash;
}

//=========================================================================================
template< typename EntryType >
Utilities::Map< EntryType >::TableEntry::TableEntry( void )
{
}

//=========================================================================================
template< typename EntryType >
/*virtual*/ Utilities::Map< EntryType >::TableEntry::~TableEntry( void )
{
}

//=========================================================================================
template< typename EntryType >
Utilities::Map< EntryType >::Iterator::Iterator( const Map* map )
{
	Reset( map );
}

//=========================================================================================
template< typename EntryType >
/*virtual*/ Utilities::Map< EntryType >::Iterator::~Iterator( void )
{
}

//=========================================================================================
template< typename EntryType >
void Utilities::Map< EntryType >::Iterator::Reset( const Map* map )
{
	this->map = map;
	tableIndex = 0;
	FindNextEntry();
}

//=========================================================================================
template< typename EntryType >
bool Utilities::Map< EntryType >::Iterator::Finished( void )
{
	if( tableEntry )
		return false;
	return true;
}

//=========================================================================================
template< typename EntryType >
EntryType Utilities::Map< EntryType >::Iterator::CurrentEntry( const char** entryName )
{
	if( Finished() )
		return 0;
	if( entryName )
		*entryName = tableEntry->entryName;
	return tableEntry->entry;
}

//=========================================================================================
template< typename EntryType >
void Utilities::Map< EntryType >::Iterator::Next( void )
{
	if( !Finished() )
	{
		tableEntry = ( TableEntry* )tableEntry->Right();
		if( !tableEntry )
		{
			tableIndex++;
			FindNextEntry();
		}
	}
}

//=========================================================================================
template< typename EntryType >
void Utilities::Map< EntryType >::Iterator::FindNextEntry( void )
{
	do
	{
		tableEntry = ( TableEntry* )map->hashTable[ tableIndex ].LeftMost();
		if( !tableEntry )
			tableIndex++;
		else
			break;
	}
	while( tableIndex < map->hashTableSize );
}

// Map.hpp