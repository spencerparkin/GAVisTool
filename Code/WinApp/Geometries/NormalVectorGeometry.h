// NormalVectorGeometry.h

/*
 * Copyright (C) 2013-2014 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#include "../Geometry.h"

//=========================================================================================
// A normal vector doesn't scale and has no position.
// We always draw it at the origin.
class NormalVector : public GAVisToolGeometry
{
	DECLARE_CALCLIB_CLASS( PositionVector );

public:

	NormalVector( BindType bindType );
	virtual ~NormalVector( void );

	static GAVisToolBindTarget* Create( BindType bindType );

	virtual void DecomposeFrom( const GeometricAlgebra::SumOfBlades& element ) override;
	virtual void ComposeTo( GeometricAlgebra::SumOfBlades& element ) const override;

	virtual void DumpInfo( char* printBuffer, int printBufferSize ) const override;

	virtual void AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const override;

	virtual void Draw( GAVisToolRender& render, bool selected );
	virtual void CalcCenter( VectorMath::Vector& center ) const;

	virtual void Translate( const VectorMath::Vector& delta );
	virtual void Rotate( const VectorMath::Vector& unitAxis, float angle );
	virtual void Scale( float scale );

	virtual void NameCenterOffset( VectorMath::Vector& offsetDelta );

private:

	VectorMath::Vector normal;
};

// NormalVectorGeometry.h