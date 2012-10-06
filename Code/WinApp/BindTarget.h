// BindTarget.h

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#include "Calculator/CalcLib.h"
#include "InventoryTree.h"

//=========================================================================================
class GAVisToolBindTarget : public Utilities::List::Item, public GAVisToolInventoryTree::Item
{
	DECLARE_CALCLIB_CLASS( GAVisToolBindTarget );

public:

	// The bind type indicates how we are bound to an element of geometric algebra.
	// The type of geometry that is bound will determine further how to interpret
	// the element of geometric algebra as a piece of geometry or whatever else.
	enum BindType
	{
		DOESNT_MATTER,
		NORMAL_FORM,
		DUAL_FORM,
		ROTATED_FORM,
	};

	GAVisToolBindTarget( BindType bindType = DOESNT_MATTER );
	virtual ~GAVisToolBindTarget( void );

	typedef GAVisToolBindTarget* ( *CreationFunction )( BindType bindType );

	virtual Utilities::List::SortComparison SortCompare( const Utilities::List::Item* compareWithItem ) const;

	virtual void DecomposeFrom( const GeometricAlgebra::SumOfBlades& element ) = 0;
	virtual void ComposeTo( GeometricAlgebra::SumOfBlades& element ) const = 0;

	virtual void DumpInfo( char* printBuffer, int printBufferSize ) const = 0;

	virtual void Initialize( void );
	virtual void Finalize( void );

	void SetName( const char* name );
	const char* GetName( void ) const;

	// This needs to be called anywhere and everywhere that a bind target has been changed.
	void HasChanged( bool changed );
	bool HasChanged( void );

protected:

	char* name;
	BindType bindType;
	bool changed;
};

// BindTarget.h