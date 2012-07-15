// RoundGeometry.h

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
class ConformalPoint : public GAVisToolGeometry
{
	DECLARE_CALCLIB_CLASS( ConformalPoint );

public:

	ConformalPoint( BindType bindType );
	virtual ~ConformalPoint( void );

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

	VectorMath::Vector center;
	double weight;
};

//=========================================================================================
class ConformalSphere : public GAVisToolGeometry
{
	DECLARE_CALCLIB_CLASS( ConformalSphere );

public:

	ConformalSphere( BindType bindType );
	virtual ~ConformalSphere( void );

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

private:

	VectorMath::Vector center;
	double radius, weight;
	bool imaginary;
};

//=========================================================================================
class ConformalCircle : public GAVisToolGeometry
{
	DECLARE_CALCLIB_CLASS( ConformalCircle );

public:

	ConformalCircle( BindType bindType );
	virtual ~ConformalCircle( void );

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

private:

	VectorMath::Vector center;
	VectorMath::Vector unitNormal;
	double radius, weight;
	bool imaginary;
};

//=========================================================================================
class ConformalPointPair : public GAVisToolGeometry
{
	DECLARE_CALCLIB_CLASS( ConformalPointPair );

public:

	ConformalPointPair( BindType bindType );
	virtual ~ConformalPointPair( void );

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

private:

	VectorMath::Vector center;
	VectorMath::Vector unitNormal;
	double radius, weight;
	bool imaginary;
};

// RoundGeometry.h