// ConformalQuarticGeometry.h

/*
 * Copyright (C) 2013-2014 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#include "SurfaceGeometry.h"
#include "../VectorMath/Surface.h"

//=========================================================================================
class ConformalQuarticGeometry : public SurfaceGeometry
{
	DECLARE_CALCLIB_CLASS( ConformalQuarticGeometry );

public:

	ConformalQuarticGeometry( BindType bindType );
	virtual ~ConformalQuarticGeometry( void );

	static GAVisToolBindTarget* Create( BindType bindType );

	virtual void DecomposeFrom( const GeometricAlgebra::SumOfBlades& element ) override;
	
	virtual void DumpInfo( char* printBuffer, int printBufferSize ) const override;
	virtual void AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const override;

private:

	VectorMath::ConformalQuartic conformalQuartic;
};

// ConformalQuarticGeometry.h