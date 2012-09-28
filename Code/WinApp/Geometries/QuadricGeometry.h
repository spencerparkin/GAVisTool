// QuadricGeometry.h

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
#include "../VectorMath/ConvexHull.h"

//=========================================================================================
class QuadricGeometry : public GAVisToolGeometry
{
	DECLARE_CALCLIB_CLASS( QuadricGeometry );

public:

	QuadricGeometry( BindType bindType );
	virtual ~QuadricGeometry( void );

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

	GeometricAlgebra::SumOfBlades element;
	CalcLib::Evaluator* samplePointTestEvaluator;
	Utilities::List samplePointCloudData;
	bool samplePointCloudDataValid;

	void RegeneratePointCloudData( void );

	class SamplePointData : public Utilities::List::Item
	{
	public:
		SamplePointData( const VectorMath::Vector& point, double scalar );
		virtual ~SamplePointData( void );

		virtual Utilities::List::SortComparison SortCompare( const Utilities::List::Item* compareWithItem ) const;

		VectorMath::Vector point;
		double scalar;
	};

	SamplePointData* TestSamplePoint( const VectorMath::Vector& samplePoint, CalcLib::GeometricAlgebraEnvironment& gaEnv );

public:

	static bool setupSamplingParameters;
	static VectorMath::Aabb samplingRegion;
	static VectorMath::Vector samplingResolution;
};

// QuadricGeometry.h