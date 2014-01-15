// InferredGeometry.h

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
// The inferred geometry class knows about all other conformal geometry classes
// and, given an element of geometric algebra, can deduce which conformal geometry
// is being represented by that element.  Using this ability, this geometry class
// can be used to interpret and manifest any conformal geometry.
class InferredGeometry : public GAVisToolGeometry
{
	DECLARE_CALCLIB_CLASS( InferredGeometry );

public:

	InferredGeometry( BindType bindType );
	virtual ~InferredGeometry( void );

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

	//...keep cached pointer to last deduced GAVisToolGeometry here...
};

// InferredGeometry.h