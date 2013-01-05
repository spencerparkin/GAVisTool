// Utilities.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#include <stdio.h>
#include <string.h>

namespace Utilities
{
	class List;
	class MultiList;
	template< typename EntryType > class Map;
	template< typename Type > class ScopeDelete;
}

#include "List.h"
#include "MultiList.h"
#include "Map.h"
#include "ScopeDelete.h"

// TODO: Move these out of the Utilities stuff.
#define PRINT_BUFFER_SIZE_LARGE		( 1024 * 16 )
#define PRINT_BUFFER_SIZE_SMALL		( 1024 * 8 )

// Utilities.h