// ProjectiveGeometry.h

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#include "../Geometry.h"

//=========================================================================================
class ProjectivePoint : public GAVisToolGeometry
{
	DECLARE_CALCLIB_CLASS( ProjectivePoint );

public:

	ProjectivePoint( BindType bindType );
	virtual ~ProjectivePoint( void );

	static GAVisToolBindTarget* Create( BindType bindType );

	virtual void DecomposeFrom( const GeometricAlgebra::SumOfBlades& element ) override;
	virtual void ComposeTo( GeometricAlgebra::SumOfBlades& element ) const override;

	virtual void DumpInfo( char* printBuffer, int printBufferSize ) const override;

	virtual void AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const override;

	virtual void Draw( GAVisToolRender& render, bool selected ) override;
	virtual void CalcCenter( VectorMath::Vector& center ) const override;

	virtual void Translate( const VectorMath::Vector& delta ) override;
	virtual void Rotate( const VectorMath::Vector& unitAxis, float angle ) override;
	virtual void Scale( float scale ) override;

	virtual void NameCenterOffset( VectorMath::Vector& offsetDelta ) override;

private:

	VectorMath::Vector center;
	double weight;
};

//=========================================================================================
class ProjectiveLine : public GAVisToolGeometry
{
	DECLARE_CALCLIB_CLASS( ProjectiveLine );

public:

	ProjectiveLine( BindType bindType );
	virtual ~ProjectiveLine( void );

	static GAVisToolBindTarget* Create( BindType bindType );

	virtual void DecomposeFrom( const GeometricAlgebra::SumOfBlades& element ) override;
	virtual void ComposeTo( GeometricAlgebra::SumOfBlades& element ) const override;

	virtual void DumpInfo( char* printBuffer, int printBufferSize ) const override;

	virtual void AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const override;

	virtual void Draw( GAVisToolRender& render, bool selected ) override;
	virtual void CalcCenter( VectorMath::Vector& center ) const override;

	virtual void Translate( const VectorMath::Vector& delta ) override;
	virtual void Rotate( const VectorMath::Vector& unitAxis, float angle ) override;
	virtual void Scale( float scale ) override;

	virtual void NameCenterOffset( VectorMath::Vector& offsetDelta ) override;

private:

	VectorMath::Vector center;
	VectorMath::Vector unitNormal;
	double weight;
};

//=========================================================================================
class ProjectivePlane : public GAVisToolGeometry
{
	DECLARE_CALCLIB_CLASS( ProjectivePlane );

public:

	ProjectivePlane( BindType bindType );
	virtual ~ProjectivePlane( void );

	static GAVisToolBindTarget* Create( BindType bindType );

	virtual void DecomposeFrom( const GeometricAlgebra::SumOfBlades& element ) override;
	virtual void ComposeTo( GeometricAlgebra::SumOfBlades& element ) const override;

	virtual void DumpInfo( char* printBuffer, int printBufferSize ) const override;

	virtual void AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const override;

	virtual void Draw( GAVisToolRender& render, bool selected ) override;
	virtual void CalcCenter( VectorMath::Vector& center ) const override;

	virtual void Translate( const VectorMath::Vector& delta ) override;
	virtual void Rotate( const VectorMath::Vector& unitAxis, float angle ) override;
	virtual void Scale( float scale ) override;

	virtual void NameCenterOffset( VectorMath::Vector& offsetDelta ) override;

private:

	VectorMath::Vector center;
	VectorMath::Vector unitNormal;
	double weight;
};

// ProjectiveGeometry.h