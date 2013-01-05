// ScopeDelete.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

//=========================================================================================
template< typename Type >
class Utilities::ScopeDelete
{
public:
	ScopeDelete( Type* typePointer );
	virtual ~ScopeDelete( void );

private:
	Type* typePointer;
};

#include "ScopeDelete.hpp"

// ScopeDelete.h