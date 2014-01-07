// PointSampledGeometry.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#include "../Geometry.h"

//=========================================================================================
class PointSampledGeometry : public GAVisToolGeometry
{
	DECLARE_CALCLIB_CLASS( ConformalPoint );

public:

	PointSampledGeometry( BindType bindType );
	virtual ~PointSampledGeometry( void );

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

	virtual void NameCenterOffset( VectorMath::Vector& offsetDelta );

private:

	double TestPoint( const VectorMath::Vector& point );

	class Box : public Utilities::List::Item
	{
	public:
		virtual Utilities::List::SortComparison SortCompare( const Utilities::List::Item* compareWithItem ) const override;
		VectorMath::Aabb aabb;
		double test;
	};

	Utilities::List positiveBoxList, negativeBoxList;
	VectorMath::Aabb::Plane splitPlane;

	void SortNewBox( const VectorMath::Aabb& aabb );
	void CullBoxList( Utilities::List& boxList );
	void RefineBoxList( Utilities::List& boxList );
	void ProcessBoxList( const Utilities::List& boxList );

	class Point : public Utilities::List::Item
	{
	public:
		VectorMath::Vector point;
	};

	Utilities::List pointCache;

	void GeneratePointCache( void );
	void WipePointCache( void );

	GeometricAlgebra::SumOfBlades element;

	CalcLib::Evaluator* testEvaluator;
};

// PointSampledGeometry.h