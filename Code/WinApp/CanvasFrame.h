// CanvasFrame.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#include "wxAll.h"
#include "Canvas.h"
#include "InventoryTree.h"

//=========================================================================================
class GAVisToolCanvasFrame : public wxFrame
{
public:

	GAVisToolCanvasFrame( const wxPoint& pos, const wxSize& size );
	virtual ~GAVisToolCanvasFrame( void );

	void OnShow( wxShowEvent& event );
	void OnSize( wxSizeEvent& event );
	void OnClose( wxCloseEvent& event );

	void OnIdle( wxIdleEvent& event );

	void OnRenderModeFastAlpha( wxCommandEvent& event );
	void OnRenderModeSlowAlpha( wxCommandEvent& event );

	void OnRenderResLow( wxCommandEvent& event );
	void OnRenderResMedium( wxCommandEvent& event );
	void OnRenderResHigh( wxCommandEvent& event );

	void OnRenderFlatShading( wxCommandEvent& event );
	void OnRenderSmoothShading( wxCommandEvent& event );

	void OnGeometryModeSkinny( wxCommandEvent& event );
	void OnGeometryModeFat( wxCommandEvent& event );

	void OnToggleLighting( wxCommandEvent& event );
	void OnDraw3DVectors( wxCommandEvent& event );
	void OnDrawGeometryNames( wxCommandEvent& event );
	void OnDrawCoordinateAxes( wxCommandEvent& event );

	void OnChooseGeometryColor( wxCommandEvent& event );
	void OnChooseGeometryAlpha( wxCommandEvent& event );

	void OnShowBspDetail( wxCommandEvent& event );

	void OnRandomTriangleInsertion( wxCommandEvent& event );
	void OnAnalyzedTriangleInsertion( wxCommandEvent& event );

	void BuildUserInterface( void );
	void UpdateUserInterface( void );
	void SetupOpenGL( void );

	enum
	{
		ID_RenderMode = wxID_HIGHEST,
		ID_Resolution,
		ID_Shading,
		ID_GeometryMode,
		ID_ToggleLighting,
		ID_Draw3DVectors,
		ID_DrawGeometryNames,
		ID_DrawCoordinateAxes,
		ID_RenderResLow,
		ID_RenderResMedium,
		ID_RenderResHigh,
		ID_RenderModeFastAlpha,
		ID_RenderModeSlowAlpha,
		ID_RenderShadingFlat,
		ID_RenderShadingSmooth,
		ID_ChooseGeometryColor,
		ID_ChooseGeometryAlpha,
		ID_RenderDebug,
		ID_RenderDebugShowBspDetail,
		ID_RenderDebugRandomTriangleInsertion,
		ID_RenderDebugAnalyzedTriangleInsertion,
		ID_RenderGeometryModeSkinny,
		ID_RenderGeometryModeFat,
	};

	GAVisToolCanvas* canvas;
	GAVisToolInventoryTree* inventoryTree;

	wxMenuBar* menuBar;
	wxStatusBar* statusBar;

	wxAuiManager auiManager;

	DECLARE_EVENT_TABLE()
};

// CanvasFrame.h