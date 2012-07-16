// Render.h

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#include "Camera.h"
#include "ObjectHeap.h"
#include "VectorMath/Vector.h"
#include "VectorMath/Triangle.h"
#include "VectorMath/AxisAlignedBoundingBox.h"
#include "Calculator/CalcLib.h"

//=============================================================================
class GAVisToolRender
{
public:

	enum RenderMode
	{
		RENDER_MODE_SELECTION,
		RENDER_MODE_NO_ALPHA_SORTING,
		RENDER_MODE_ALPHA_SORTING,
	};

	enum GeometryMode
	{
		GEOMETRY_MODE_FAT,
		GEOMETRY_MODE_SKINNY,
	};

	enum Resolution
	{
		RES_LOW,
		RES_MEDIUM,
		RES_HIGH,
		NUM_RES_TYPES,
		RES_USER,	// This means, use the user specified resolution, which is one of low, medium or high.
	};

	enum Shading
	{
		SHADE_FLAT,
		SHADE_SMOOTH,
	};

	enum BspTreeCreationMethod
	{
		RANDOM_TRIANGLE_INSERTION,
		ANALYZED_TRIANGLE_INSERTION,
	};

	enum HighlightMethod
	{
		NO_HIGHLIGHTING,
		NORMAL_HIGHLIGHTING,
		SPACIAL_HIGHLIGHTING,
	};

	GAVisToolRender( void );
	virtual ~GAVisToolRender( void );

	class Drawer
	{
	public:
		virtual void Draw( GAVisToolRender& render ) = 0;
	};

	void Draw( Drawer& drawer );
	void InvalidatePrimitiveCache( void );

	void Highlight( HighlightMethod highlightMethod );
	void Color( const VectorMath::Vector& color, double alpha );
	void Color( double r, double g, double b, double a );

	// This is the main API for drawing stuff using the renderer.
	void DrawPoint( const VectorMath::Vector& pos, const VectorMath::Vector& normal );
	void DrawTriangle( const VectorMath::Triangle& triangleGeometry );
	void DrawLine( const VectorMath::Vector& pos0, const VectorMath::Vector& pos1 );
	void DrawTube( const VectorMath::Vector& pos0, const VectorMath::Vector& pos1, double tubeRadius, Resolution resolution = RES_USER );
	void DrawSphere( const VectorMath::Vector& pos, double sphereRadius, Resolution resolution = RES_USER );
	void DrawCircle( const VectorMath::Vector& pos, const VectorMath::Vector& norm, double circleRadius, Resolution resolution = RES_USER );
	void DrawTorus( const VectorMath::Vector& pos, const VectorMath::Vector& norm, double torusRadius, Resolution resolution = RES_USER );
	void DrawDisk( const VectorMath::Vector& pos, const VectorMath::Vector& norm, double diskRadius, Resolution resolution = RES_USER );
	void DrawVector( const VectorMath::Vector& pos, const VectorMath::Vector& vec, Resolution resolution = RES_USER, bool draw1DVector = false );
	void DrawArrow( const VectorMath::Vector& pos, const VectorMath::Vector& vec );

	void SetRenderMode( RenderMode renderMode );
	RenderMode GetRenderMode( void );

	void SetGeometryMode( GeometryMode geometryMode );
	GeometryMode GetGeometryMode( void );

	void SetResolution( Resolution resolution );
	Resolution GetResolution( void );

	void SetShading( Shading shading );
	Shading GetShading( void );

	void SetDraw3DVectors( bool draw3DVectors );
	bool GetDraw3DVectors( void );

	void SetDoLighting( bool doLighting );
	bool GetDoLighting( void );

	bool ShowBspDetail( void );
	void ShowBspDetail( bool showBspDetail );

	BspTreeCreationMethod GetBspTreeCreationMethod( void );
	void SetBspTreeCreationMethod( BspTreeCreationMethod bspTreeCreationMethod );

private:

	RenderMode renderMode;
	GeometryMode geometryMode;
	Resolution userResolution;
	Shading shading;
	bool draw3DVectors;
	bool doLighting;
	bool showBspDetail;
	BspTreeCreationMethod bspTreeCreationMethod;
	VectorMath::Vector currentColor;
	double currentAlpha;
	HighlightMethod currentHighlightMethod;

	Resolution DetermineResolution( Resolution overrideResolution, double sizeFactor );
	void SpecifyColor( const VectorMath::Vector& color, double alpha );
	void SpecifyColor( unsigned int colorBits, double alpha );

	class Geometry
	{
	public:
		
		enum GeoType
		{
			GEO_NONE,
			GEO_TUBE,
			GEO_SPHERE,
			GEO_TORUS,
			GEO_DISK,
			GEO_VECTOR,
		};

		Geometry( void );
		virtual ~Geometry( void );

		void GenerateGeometry( GeoType geoType, Resolution resolution );

		void WipeGeometry( void );

		void GenerateCanonicalUnitTube( void );
		void GenerateCanonicalUnitSphere( void );
		void GenerateCanonicalUnitTorus( void );
		void GenerateCanonicalUnitDisk( void );
		void GenerateCanonicalUnitVector( void );

		class Triangle
		{
		public:
			Triangle( void );

			VectorMath::Triangle triangle;
			VectorMath::TriangleNormals triangleNormals;
			bool doubleSided;
		};

		Triangle* triangleArray;
		int triangleArraySize;
		Resolution resolution;
		GeoType geoType;
	};

	Geometry* tubeGeometry[ NUM_RES_TYPES ];
	Geometry* sphereGeometry[ NUM_RES_TYPES ];
	Geometry* torusGeometry[ NUM_RES_TYPES ];
	Geometry* diskGeometry[ NUM_RES_TYPES ];
	Geometry* vectorGeometry[ NUM_RES_TYPES ];

	void InstanceGeometry( const VectorMath::CoordFrame& coordFrame, const VectorMath::Vector& origin, const Geometry& geometry );

	class Primitive : public Utilities::List::Item
	{
	public:
		Primitive( void );
		virtual ~Primitive( void );

		void Color( bool doLighting, bool showBspDetail );

		virtual void CalcCenter( VectorMath::Vector& center ) = 0;

		void CopyBaseData( const Primitive& primitive );

		VectorMath::Vector color;
		double alpha;
		VectorMath::Vector normal;
		HighlightMethod highlightMethod;
		VectorMath::Vector bspDetailColor;
		bool doubleSided;
	};

	class BspAnalysisData;

	class Triangle : public Primitive
	{
	public:

		Triangle( void );
		virtual ~Triangle( void );

		void Draw( Shading shading, bool doLighting, bool showBspDetail );
		bool StraddlesPlane( const VectorMath::Plane& plane ) const;
		void Reset( void );

		virtual void CalcCenter( VectorMath::Vector& center );

		VectorMath::Triangle triangle;
		VectorMath::TriangleNormals triangleNormals;
		VectorMath::Plane trianglePlane;

		BspAnalysisData* bspAnalysisData;
	};

	class Line : public Primitive
	{
	public:

		Line( void );
		virtual ~Line( void );

		void Draw( bool doLighting );
		void Reset( void );

		virtual void CalcCenter( VectorMath::Vector& center );

		VectorMath::Vector vertex[2];
	};

	class Point : public Primitive
	{
	public:

		Point( void );
		virtual ~Point( void );

		void Draw( bool doLighting, VectorMath::CoordFrame& cameraFrame );

		virtual void CalcCenter( VectorMath::Vector& center );

		VectorMath::Vector vertex;
	};

	class BspTree;

	class PrimitiveCache
	{
	public:
		PrimitiveCache( int triangleHeapSize, int lineHeapSize, int pointHeapSize, int bspNodeHeapSize );
		virtual ~PrimitiveCache( void );

		Triangle* AllocateTriangle( void );
		Line* AllocateLine( void );
		Point* AllocatePoint( void );
		void Draw( GAVisToolRender& render );		// Draw all primitives in this cache.
		void Wipe( void );							// Reset this cache to empty.
		void Flush( GAVisToolRender& render );		// Draw all primitives in this cache, then reset it to empty.
		void Invalidate( void );
		void Validate( void );
		bool IsValid( void );
		void OptimizeForAlphaSorting( BspTreeCreationMethod bspTreeCreationMethod );

	private:

		bool cacheValid;
		bool optimizedForAlphaSorting;
		Utilities::List triangleList;
		Utilities::List lineList;
		Utilities::List pointList;
		ObjectHeap< Triangle > triangleHeap;
		ObjectHeap< Line > lineHeap;
		ObjectHeap< Point > pointHeap;
		BspTree* bspTree;
	};

	PrimitiveCache selectionPrimitiveCache;
	PrimitiveCache noAlphaBlendingPrimitiveCache;
	PrimitiveCache alphaBlendingPrimitiveCache;

	PrimitiveCache* activePrimitiveCache;

	class BspNode
	{
	public:

		BspNode( void );
		~BspNode( void );

		void Reset( void );
		void Insert( Line* line, ObjectHeap< Line >& lineHeap, BspTree* bspTree );
		void Insert( Triangle* triangle, ObjectHeap< Triangle >& triangleHeap, BspTree* bspTree );
		void Insert( Utilities::List& subTreeTriangleList, ObjectHeap< Triangle >& triangleHeap, BspTree* bspTree );
		void Draw( const VectorMath::Vector& cameraEye, GAVisToolRender& render );

		void DistributeLine(
					Line* line,
					Line*& backLine,
					Line*& frontLine,
					ObjectHeap< Line >& lineHeap,
					BspTree* bspTree );
		void DistributeTriangle(
					Triangle* triangle,
					Utilities::List& backTriangleList,
					Utilities::List& frontTriangleList,
					ObjectHeap< Triangle >& triangleHeap,
					BspTree* bspTree );

		Triangle* ChooseBestRootTriangle( Utilities::List& subTreeTriangleList );

		Utilities::List triangleList;		// A list of triangles in the partitioning plane.
		Utilities::List lineList;			// A list of lines in the partitioning plane.
		VectorMath::Plane partitionPlane;
		bool partitionPlaneCreated;
		BspNode* frontNode;
		BspNode* backNode;
	};

	class BspAnalysisData : public Utilities::MultiList::Item
	{
	public:
		BspAnalysisData( void );
		virtual ~BspAnalysisData( void );

		void Reset( void );

		Utilities::MultiList trianglesSplitByThis;
		Utilities::MultiList trianglesSplittingThis;
		Triangle* triangle;
	};

	class BspTree
	{
	public:

		static const double HALF_PLANE_THICKNESS;

		BspTree( int bspNodeHeapSize );
		~BspTree( void );

		void Destroy( void );
		void Create(
				Utilities::List& triangleList,
				ObjectHeap< Triangle >& triangleHeap,
				Utilities::List& lineList,
				ObjectHeap< Line >& lineHeap,
				BspTreeCreationMethod creationMethod );
		void Draw( const VectorMath::Vector& cameraEye, GAVisToolRender& render );
		
		void AnalyzeTriangleList( Utilities::List& triangleList );
		static void AnalyzeTrianglePair( Triangle* triangle0, Triangle* triangle1 );
		static void OldTriangleDies( Triangle* oldTriangle );
		static void NewTriangleBornFromOld( Triangle* newTriangle, Triangle* oldTriangle, ObjectHeap< BspAnalysisData >& bspAnalysisDataHeap );

		BspNode* rootNode;
		ObjectHeap< BspNode > bspNodeHeap;
		ObjectHeap< BspAnalysisData > bspAnalysisDataHeap;
		int preBspTriangleCount;
		int postBspTriangleCount;
		int preBspLineCount;
		int postBspLineCount;
		int triangleSplitCount;
		int lineSplitCount;
	};
};

// Render.h