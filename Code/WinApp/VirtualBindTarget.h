// VirtualBindTarget.h

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#include "BindTarget.h"

//=========================================================================================
// A virtual bind target is something that will not be made manifest in the canvas
// window in any form by any interpretation, geometric or otherwise.  The main
// purpose of such targets is to create connection points in the constraint dependency
// graph without needing to have something for that point in the canvas window.
class GAVisToolVirtualBindTarget : public GAVisToolBindTarget
{
	DECLARE_CALCLIB_CLASS( GAVisToolVirtualBindTarget );

public:

	GAVisToolVirtualBindTarget( BindType bindType );
	virtual ~GAVisToolVirtualBindTarget( void );

	static GAVisToolBindTarget* Create( BindType bindType );

	virtual void DecomposeFrom( const GeometricAlgebra::SumOfBlades& element ) override;
	virtual void ComposeTo( GeometricAlgebra::SumOfBlades& element ) const override;

	virtual void DumpInfo( char* printBuffer, int printBufferSize ) const override;

	virtual void AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const override;

	GeometricAlgebra::SumOfBlades element;
};

// VirtualBindTarget.h