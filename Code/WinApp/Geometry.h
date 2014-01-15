// Geometry.h

/*
 * Copyright (C) 2013-2014 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#include "Calculator/CalcLib.h"
#include "VectorMath/Vector.h"
#include "wxAll.h"
#include "Render.h"
#include "BindTarget.h"

//=========================================================================================
class GAVisToolGeometry : public GAVisToolBindTarget
{
	DECLARE_CALCLIB_CLASS( GAVisToolGeometry );

public:

	GAVisToolGeometry( BindType bindType );
	virtual ~GAVisToolGeometry( void );

	virtual void Draw( GAVisToolRender& render, bool selected ) = 0;
	virtual Utilities::List::SortComparison SortCompare( const Utilities::List::Item* compareWithItem ) const;
	virtual void CalcCenter( VectorMath::Vector& center ) const = 0;
	static float WeightAsColorComponent( double weight );
	
	virtual void Translate( const VectorMath::Vector& delta ) = 0;
	virtual void Rotate( const VectorMath::Vector& unitAxis, float angle ) = 0;
	virtual void Scale( float scale ) = 0;

	void DrawName( VectorMath::CoordFrame& cameraFrame );
	virtual void NameCenterOffset( VectorMath::Vector& offsetDelta );
	void GenerateNameTexture( bool regenerate = false );

	void SetColor( const VectorMath::Vector& color );
	void GetColor( VectorMath::Vector& color );
	void SetColor( const wxColour& color );
	void GetColor( wxColour& color ) const;
	void SetAlpha( double alpha );
	double GetAlpha( void ) const;
	bool IsOpaque( void ) const;

protected:

	GLuint nameTexture;
	VectorMath::Vector color;
	double alpha;

	// Using pre-compiled scripts is not as fast as
	// using the GA library types directly, but it
	// is much easier to program the correct formulas
	// from a script rather than long drawn out lines
	// of code, especially when it's easy to make a
	// mis-calculation.
	CalcLib::Evaluator* decompositionEvaluator;
	CalcLib::Evaluator* compositionEvaluator;
};

//=========================================================================================
// This geometry is designed for debugging purposes only.
class TestGeometry : public GAVisToolGeometry
{
public:

	TestGeometry( int nameIndex );
	virtual ~TestGeometry( void );

	virtual void DecomposeFrom( const GeometricAlgebra::SumOfBlades& element ) override;
	virtual void ComposeTo( GeometricAlgebra::SumOfBlades& element ) const override;

	virtual void DumpInfo( char* printBuffer, int printBufferSize ) const override;

	virtual void AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const override;

	virtual void Draw( GAVisToolRender& render, bool selected ) override;
	virtual void CalcCenter( VectorMath::Vector& center ) const override;
	
	virtual void Translate( const VectorMath::Vector& delta ) override;
	virtual void Rotate( const VectorMath::Vector& unitAxis, float angle ) override;
	virtual void Scale( float scale ) override;

	VectorMath::Triangle triangle;
};

// Geometry.h