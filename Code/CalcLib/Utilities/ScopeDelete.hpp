// ScopeDelete.hpp

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
Utilities::ScopeDelete< Type >::ScopeDelete( Type* typePointer )
{
	this->typePointer = typePointer;
}

//=========================================================================================
template< typename Type >
/*virtual*/ Utilities::ScopeDelete< Type >::~ScopeDelete( void )
{
	delete typePointer;
	typePointer = 0;
}

// ScopeDelete.hpp