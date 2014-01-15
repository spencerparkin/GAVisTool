// PointSampledGeometry.h

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
#include "../VectorMath/Surface.h"

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

	//=========================================================================================
	class Surface : public VectorMath::Surface
	{
	public:
		Surface( void );
		virtual ~Surface( void );

		virtual double EvaluateAt( const VectorMath::Vector& point ) const override;
		virtual double EvaluatePartialX( const VectorMath::Vector& point ) const override;
		virtual double EvaluatePartialY( const VectorMath::Vector& point ) const override;
		virtual double EvaluatePartialZ( const VectorMath::Vector& point ) const override;

		double EvaluateWith( CalcLib::Evaluator* evaluator, const VectorMath::Vector& point ) const;

		mutable CalcLib::Calculator calculator;
		CalcLib::Number* number;

		CalcLib::Evaluator* functionEvaluator;
		CalcLib::Evaluator* functionPartialXEvaluator;
		CalcLib::Evaluator* functionPartialYEvaluator;
		CalcLib::Evaluator* functionPartialZEvaluator;

		GeometricAlgebra::SumOfBlades element;
	};

	Surface surface;

	//=========================================================================================
	class Point : public Utilities::List::Item
	{
	public:
		VectorMath::Vector point;
	};

	Utilities::List pointList;

	void RegeneratePointListIfNeeded( void );
};

// PointSampledGeometry.h