// CanvasFrame.cpp

/*
 * Copyright (C) 2013-2014 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "CanvasFrame.h"
#include "Application.h"

//=========================================================================================
BEGIN_EVENT_TABLE( GAVisToolCanvasFrame, wxFrame )
	
	EVT_SHOW( GAVisToolCanvasFrame::OnShow )
	EVT_SIZE( GAVisToolCanvasFrame::OnSize )
	EVT_CLOSE( GAVisToolCanvasFrame::OnClose )

	EVT_IDLE( GAVisToolCanvasFrame::OnIdle )

	EVT_MENU( GAVisToolCanvasFrame::ID_RenderModeFastAlpha, OnRenderModeFastAlpha )
	EVT_MENU( GAVisToolCanvasFrame::ID_RenderModeSlowAlpha, OnRenderModeSlowAlpha )

	EVT_MENU( GAVisToolCanvasFrame::ID_RenderResLow, OnRenderResLow )
	EVT_MENU( GAVisToolCanvasFrame::ID_RenderResMedium, OnRenderResMedium )
	EVT_MENU( GAVisToolCanvasFrame::ID_RenderResHigh, OnRenderResHigh )

	EVT_MENU( GAVisToolCanvasFrame::ID_RenderShadingFlat, OnRenderFlatShading )
	EVT_MENU( GAVisToolCanvasFrame::ID_RenderShadingSmooth, OnRenderSmoothShading )

	EVT_MENU( GAVisToolCanvasFrame::ID_RenderGeometryModeSkinny, OnGeometryModeSkinny )
	EVT_MENU( GAVisToolCanvasFrame::ID_RenderGeometryModeFat, OnGeometryModeFat )

	EVT_MENU( GAVisToolCanvasFrame::ID_ToggleLighting, OnToggleLighting )
	EVT_MENU( GAVisToolCanvasFrame::ID_Draw3DVectors, OnDraw3DVectors )
	EVT_MENU( GAVisToolCanvasFrame::ID_DrawGeometryNames, OnDrawGeometryNames )
	EVT_MENU( GAVisToolCanvasFrame::ID_DrawCoordinateAxes, OnDrawCoordinateAxes )

	EVT_MENU( GAVisToolCanvasFrame::ID_ChooseGeometryColor, OnChooseGeometryColor )
	EVT_MENU( GAVisToolCanvasFrame::ID_ChooseGeometryAlpha, OnChooseGeometryAlpha )

	EVT_MENU( GAVisToolCanvasFrame::ID_RenderDebugShowBspDetail, OnShowBspDetail )

	EVT_MENU( GAVisToolCanvasFrame::ID_RenderDebugRandomTriangleInsertion, OnRandomTriangleInsertion )
	EVT_MENU( GAVisToolCanvasFrame::ID_RenderDebugAnalyzedTriangleInsertion, OnAnalyzedTriangleInsertion )

END_EVENT_TABLE()

//=========================================================================================
GAVisToolCanvasFrame::GAVisToolCanvasFrame( const wxPoint& pos, const wxSize& size ) : wxFrame( ( wxFrame* )NULL, wxID_ANY, "Canvas", pos, size )
{
	canvas = 0;
	inventoryTree = 0;

	menuBar = 0;
	statusBar = 0;

	BuildUserInterface();
}

//=========================================================================================
/*virtual*/ GAVisToolCanvasFrame::~GAVisToolCanvasFrame( void )
{
	// This seems ugly.  This prevents a heap corruption on exit.
	if( inventoryTree )
	{
		RemoveChild( inventoryTree );
		delete inventoryTree;
		inventoryTree = 0;
	}

	auiManager.UnInit();
}

//=========================================================================================
void GAVisToolCanvasFrame::OnShow( wxShowEvent& event )
{
	if( event.IsShown() )
		canvas->SetupOpenGL();
}

//=========================================================================================
void GAVisToolCanvasFrame::OnSize( wxSizeEvent& event )
{
	wxFrame::OnSize( event );
}

//=========================================================================================
void GAVisToolCanvasFrame::OnClose( wxCloseEvent& event )
{
	// Only let the canvas frame go away when the entire application goes away.
	//event.Veto();		// Oops, only do this if the application is not going down.
	// The veto is important or we'll crash on exit.
	// A better solution would be to just not present the close button.  Can we do that?

	wxFrame::OnCloseWindow( event );
}

//=========================================================================================
void GAVisToolCanvasFrame::OnIdle( wxIdleEvent& event )
{
	wxGetApp().environment->SatisfyConstraints();

	inventoryTree->RegenerateTreeIfNeeded();
}

//=========================================================================================
void GAVisToolCanvasFrame::OnChooseGeometryColor( wxCommandEvent& event )
{
	GAVisToolGeometry* geometry = wxGetApp().environment->LookupGeometryByName( canvas->selectedGeometry );
	if( !geometry )
		wxMessageBox( wxT( "You must first select a geometry with the right mouse button.  Then you can change its color." ), wxT( "Oops!" ) );
	else
	{
		wxColour color;
		geometry->GetColor( color );
		wxColourData colorData;
		colorData.SetColour( color );
		wxColourDialog colorDlg( this, &colorData );
		if( wxID_OK == colorDlg.ShowModal() )
		{
			geometry->SetColor( colorDlg.GetColourData().GetColour() );
			canvas->SetSelection( "" );
			canvas->RedrawNeeded( true );
		}
	}
}

//=========================================================================================
void GAVisToolCanvasFrame::OnChooseGeometryAlpha( wxCommandEvent& event )
{
	GAVisToolGeometry* geometry = wxGetApp().environment->LookupGeometryByName( canvas->selectedGeometry );
	if( !geometry )
		wxMessageBox( wxT( "You must first select a geometry with the right mouse button.  Then you can change its alpha." ), wxT( "Oops!" ) );
	else
	{
		double alpha = geometry->GetAlpha();
		long integralAlpha = long( alpha * 255.0 );
		integralAlpha = wxGetNumberFromUser( wxT( "Please enter an alpha value in the range [0,255]." ), wxT( "Alpha: " ), wxT( "Choose Alpha" ), integralAlpha, 0, 255, this );
		if( integralAlpha != -1 )
		{
			alpha = double( integralAlpha ) / 255.0;
			geometry->SetAlpha( alpha );
			canvas->RedrawNeeded( true );
		}
	}
}

//=========================================================================================
void GAVisToolCanvasFrame::OnRenderModeFastAlpha( wxCommandEvent& event )
{
	canvas->render.SetRenderMode( GAVisToolRender::RENDER_MODE_NO_ALPHA_SORTING );
	UpdateUserInterface();
	canvas->RedrawNeeded( true );
}

//=========================================================================================
void GAVisToolCanvasFrame::OnRenderModeSlowAlpha( wxCommandEvent& event )
{
	canvas->render.SetRenderMode( GAVisToolRender::RENDER_MODE_ALPHA_SORTING );
	UpdateUserInterface();
	canvas->RedrawNeeded( true );
}

//=========================================================================================
void GAVisToolCanvasFrame::OnRenderResLow( wxCommandEvent& event )
{
	canvas->render.SetResolution( GAVisToolRender::RES_LOW );
	UpdateUserInterface();
	canvas->RedrawNeeded( true );
}

//=========================================================================================
void GAVisToolCanvasFrame::OnRenderResMedium( wxCommandEvent& event )
{
	canvas->render.SetResolution( GAVisToolRender::RES_MEDIUM );
	UpdateUserInterface();
	canvas->RedrawNeeded( true );
}

//=========================================================================================
void GAVisToolCanvasFrame::OnRenderResHigh( wxCommandEvent& event )
{
	canvas->render.SetResolution( GAVisToolRender::RES_HIGH );
	UpdateUserInterface();
	canvas->RedrawNeeded( true );
}

//=========================================================================================
void GAVisToolCanvasFrame::OnRenderFlatShading( wxCommandEvent& event )
{
	canvas->render.SetShading( GAVisToolRender::SHADE_FLAT );
	UpdateUserInterface();
	canvas->RedrawNeeded( false );
}

//=========================================================================================
void GAVisToolCanvasFrame::OnRenderSmoothShading( wxCommandEvent& event )
{
	canvas->render.SetShading( GAVisToolRender::SHADE_SMOOTH );
	UpdateUserInterface();
	canvas->RedrawNeeded( false );
}

//=========================================================================================
void GAVisToolCanvasFrame::OnGeometryModeSkinny( wxCommandEvent& event )
{
	canvas->render.SetGeometryMode( GAVisToolRender::GEOMETRY_MODE_SKINNY );
	UpdateUserInterface();
	canvas->RedrawNeeded( true );
}

//=========================================================================================
void GAVisToolCanvasFrame::OnGeometryModeFat( wxCommandEvent& event )
{
	canvas->render.SetGeometryMode( GAVisToolRender::GEOMETRY_MODE_FAT );
	UpdateUserInterface();
	canvas->RedrawNeeded( true );
}

//=========================================================================================
void GAVisToolCanvasFrame::OnToggleLighting( wxCommandEvent& event )
{
	canvas->render.SetDoLighting( !canvas->render.GetDoLighting() );
	UpdateUserInterface();
	canvas->RedrawNeeded( false );
}

//=========================================================================================
void GAVisToolCanvasFrame::OnDraw3DVectors( wxCommandEvent& event )
{
	canvas->render.SetDraw3DVectors( !canvas->render.GetDraw3DVectors() );
	UpdateUserInterface();
	canvas->RedrawNeeded( true );
}

//=========================================================================================
void GAVisToolCanvasFrame::OnDrawGeometryNames( wxCommandEvent& event )
{
	canvas->drawGeometryNames = !canvas->drawGeometryNames;
	if( canvas->drawGeometryNames )
		wxGetApp().environment->GenerateGeometryNameTextures();
	UpdateUserInterface();
	canvas->RedrawNeeded( false );
}

//=========================================================================================
void GAVisToolCanvasFrame::OnDrawCoordinateAxes( wxCommandEvent& event )
{
	canvas->drawCoordinateAxes = !canvas->drawCoordinateAxes;
	UpdateUserInterface();
	canvas->RedrawNeeded( true );
}

//=========================================================================================
void GAVisToolCanvasFrame::OnShowBspDetail( wxCommandEvent& event )
{
	canvas->render.ShowBspDetail( !canvas->render.ShowBspDetail() );
	UpdateUserInterface();
	canvas->RedrawNeeded( false );
}

//=========================================================================================
void GAVisToolCanvasFrame::OnRandomTriangleInsertion( wxCommandEvent& event )
{
	canvas->render.SetBspTreeCreationMethod( GAVisToolRender::RANDOM_TRIANGLE_INSERTION );
	UpdateUserInterface();
	canvas->RedrawNeeded( true );
}

//=========================================================================================
void GAVisToolCanvasFrame::OnAnalyzedTriangleInsertion( wxCommandEvent& event )
{
	canvas->render.SetBspTreeCreationMethod( GAVisToolRender::ANALYZED_TRIANGLE_INSERTION );
	UpdateUserInterface();
	canvas->RedrawNeeded( true );
}

//=========================================================================================
// Update the user interface controls as a function of the application's internal settings.
// TODO: wxWidgets has a better way of handling this type of thing.  Use their method instead of this.
void GAVisToolCanvasFrame::UpdateUserInterface( void )
{
	wxMenuItem* renderModeFastAlphaMenuItem = menuBar->FindItem( ID_RenderModeFastAlpha );
	wxMenuItem* renderModeSlowAlphaMenuItem = menuBar->FindItem( ID_RenderModeSlowAlpha );
	GAVisToolRender::RenderMode renderMode = canvas->render.GetRenderMode();
	switch( renderMode )
	{
		case GAVisToolRender::RENDER_MODE_NO_ALPHA_SORTING:
		{
			renderModeFastAlphaMenuItem->Check( true );
			renderModeSlowAlphaMenuItem->Check( false );
			break;
		}
		case GAVisToolRender::RENDER_MODE_ALPHA_SORTING:
		{
			renderModeFastAlphaMenuItem->Check( false );
			renderModeSlowAlphaMenuItem->Check( true );
			break;
		}
	}

	wxMenuItem* renderResolutionLowMenuItem = menuBar->FindItem( ID_RenderResLow );
	wxMenuItem* renderResolutionMediumMenuItem = menuBar->FindItem( ID_RenderResMedium );
	wxMenuItem* renderResolutionHighMenuItem = menuBar->FindItem( ID_RenderResHigh );
	GAVisToolRender::Resolution resolution = canvas->render.GetResolution();
	switch( resolution )
	{
		case GAVisToolRender::RES_LOW:
		{
			renderResolutionLowMenuItem->Check( true );
			renderResolutionMediumMenuItem->Check( false );
			renderResolutionHighMenuItem->Check( false );
			break;
		}
		case GAVisToolRender::RES_MEDIUM:
		{
			renderResolutionLowMenuItem->Check( false );
			renderResolutionMediumMenuItem->Check( true );
			renderResolutionHighMenuItem->Check( false );
			break;
		}
		case GAVisToolRender::RES_HIGH:
		{
			renderResolutionLowMenuItem->Check( false );
			renderResolutionMediumMenuItem->Check( false );
			renderResolutionHighMenuItem->Check( true );
			break;
		}
	}

	wxMenuItem* renderShadingFlatMenuItem = menuBar->FindItem( ID_RenderShadingFlat );
	wxMenuItem* renderShadingSmoothMenuItem = menuBar->FindItem( ID_RenderShadingSmooth );
	GAVisToolRender::Shading shading = canvas->render.GetShading();
	switch( shading )
	{
		case GAVisToolRender::SHADE_FLAT:
		{
			renderShadingFlatMenuItem->Check( true );
			renderShadingSmoothMenuItem->Check( false );
			break;
		}
		case GAVisToolRender::SHADE_SMOOTH:
		{
			renderShadingFlatMenuItem->Check( false );
			renderShadingSmoothMenuItem->Check( true );
			break;
		}
	}

	wxMenuItem* renderGeometryModeSkinnyMenuItem = menuBar->FindItem( ID_RenderGeometryModeSkinny );
	wxMenuItem* renderGeometryModeFatMenuItem = menuBar->FindItem( ID_RenderGeometryModeFat );
	GAVisToolRender::GeometryMode geometryMode = canvas->render.GetGeometryMode();
	switch( geometryMode )
	{
		case GAVisToolRender::GEOMETRY_MODE_SKINNY:
		{
			renderGeometryModeSkinnyMenuItem->Check( true );
			renderGeometryModeFatMenuItem->Check( false );
			break;
		}
		case GAVisToolRender::GEOMETRY_MODE_FAT:
		{
			renderGeometryModeSkinnyMenuItem->Check( false );
			renderGeometryModeFatMenuItem->Check( true );
			break;
		}
	}

	wxMenuItem* renderLightingMenuItem = menuBar->FindItem( ID_ToggleLighting );
	renderLightingMenuItem->Check( canvas->render.GetDoLighting() );

	wxMenuItem* renderDraw3DVectorsMenuItem = menuBar->FindItem( ID_Draw3DVectors );
	renderDraw3DVectorsMenuItem->Check( canvas->render.GetDraw3DVectors() );

	wxMenuItem* renderDrawGeometryNamesMenuItem = menuBar->FindItem( ID_DrawGeometryNames );
	renderDrawGeometryNamesMenuItem->Check( canvas->drawGeometryNames );

	wxMenuItem* renderDrawCoordinateAxesMenuItem = menuBar->FindItem( ID_DrawCoordinateAxes );
	renderDrawCoordinateAxesMenuItem->Check( canvas->drawCoordinateAxes );

	wxMenuItem* renderDebugShowBspDetailMenuItem = menuBar->FindItem( ID_RenderDebugShowBspDetail );
	renderDebugShowBspDetailMenuItem->Check( canvas->render.ShowBspDetail() );

	wxMenuItem* renderDebugRandomTriangleInsertionMenuItem = menuBar->FindItem( ID_RenderDebugRandomTriangleInsertion );
	wxMenuItem* renderDebugAnalyzedTriangleInsertionMenuItem = menuBar->FindItem( ID_RenderDebugAnalyzedTriangleInsertion );
	switch( canvas->render.GetBspTreeCreationMethod() )
	{
		case GAVisToolRender::RANDOM_TRIANGLE_INSERTION:
		{
			renderDebugRandomTriangleInsertionMenuItem->Check( true );
			renderDebugAnalyzedTriangleInsertionMenuItem->Check( false );
			break;
		}
		case GAVisToolRender::ANALYZED_TRIANGLE_INSERTION:
		{
			renderDebugRandomTriangleInsertionMenuItem->Check( false );
			renderDebugAnalyzedTriangleInsertionMenuItem->Check( true );
			break;
		}
	}
}

//=========================================================================================
void GAVisToolCanvasFrame::BuildUserInterface( void )
{
	auiManager.SetManagedWindow( this );

	inventoryTree = new GAVisToolInventoryTree( this );
	auiManager.AddPane( inventoryTree,
					wxAuiPaneInfo().
					Name( wxT( "InventoryTree" ) ).
					Caption( wxT( "Inventory" ) ).
					Left().CloseButton( true ).Hide() );

	wxMenu* renderModeMenu = new wxMenu;
	renderModeMenu->Append( ID_RenderModeFastAlpha, wxT( "Fast Alpha Blending" ), wxString( "Perform fast alpha blending that is sometimes wrong." ), true );
	renderModeMenu->Append( ID_RenderModeSlowAlpha, wxT( "Slow Alpha Blending" ), wxString( "Perform slow alpha blending that is more often right." ), true );

	wxMenu* renderResolutionMenu = new wxMenu;
	renderResolutionMenu->Append( ID_RenderResLow, wxT( "Low" ), wxString( "Render all geometries at low resolution." ), true );
	renderResolutionMenu->Append( ID_RenderResMedium, wxT( "Medium" ), wxString( "Render all geometries at medium resolution." ), true );
	renderResolutionMenu->Append( ID_RenderResHigh, wxT( "High" ), wxString( "Render all geometries at high resolution." ), true );

	wxMenu* renderShadingMenu = new wxMenu;
	renderShadingMenu->Append( ID_RenderShadingFlat, wxT( "Flat" ), wxString( "Use the same lighting normal on all vertices of a triangle." ), true );
	renderShadingMenu->Append( ID_RenderShadingSmooth, wxT( "Smooth" ), wxString( "Use different lighting normals on all vertices of a triangle." ), true );

	wxMenu* renderGeometryModeMenu = new wxMenu;
	renderGeometryModeMenu->Append( ID_RenderGeometryModeSkinny, wxT( "Skinny" ), wxString( "Subsitute line-segments for tube-like meshes with girth when possible." ), true );
	renderGeometryModeMenu->Append( ID_RenderGeometryModeFat, wxT( "Fat" ), wxString( "Give geometries that are infinitely thin some polygonal mesh girth." ), true );

	wxMenu* debugMenu = new wxMenu;
	debugMenu->Append( ID_RenderDebugShowBspDetail, wxT( "Show BSP Detail" ), wxString( "Show the triangle tesselations made by the BSP tree cuts." ), true );
	debugMenu->AppendSeparator();
	debugMenu->Append( ID_RenderDebugRandomTriangleInsertion, wxT( "Random Triangle Insertion" ), wxString( "Construct BSP trees using the random triangle insertion method." ), true );
	debugMenu->Append( ID_RenderDebugAnalyzedTriangleInsertion, wxT( "Analyzed Triangle Insertion" ), wxString( "Construct BSP trees using an analyzed triangle insertion method." ), true );

	wxMenu* renderMenu = new wxMenu;
	renderMenu->Append( ID_RenderMode, wxT( "Mode" ), renderModeMenu );
	renderMenu->Append( ID_Resolution, wxT( "Resolution" ), renderResolutionMenu );
	renderMenu->Append( ID_Shading, wxT( "Shading" ), renderShadingMenu );
	renderMenu->Append( ID_GeometryMode, wxT( "Geometry Mode" ), renderGeometryModeMenu );
	renderMenu->Append( ID_ToggleLighting, wxT( "Lighting" ), wxString( "Toggle lighting on/off." ), true );
	renderMenu->Append( ID_Draw3DVectors, wxT( "Draw 3D Vectors" ), wxString( "Toggle between 3D vectors and 1D vectors." ), true );
	renderMenu->Append( ID_DrawGeometryNames, wxT( "Draw Geometry Names" ), wxString( "Toggle the drawing of name-labels for each geometry." ), true );
	renderMenu->Append( ID_DrawCoordinateAxes, wxT( "Draw Coordinate Axes" ), wxString( "Toggle the drawing of the origin and coordinate axes." ), true );
	renderMenu->AppendSeparator();
	renderMenu->Append( ID_ChooseGeometryColor, wxT( "Geometry Color..." ), wxString( "Change the color of the selected geometry." ) );
	renderMenu->Append( ID_ChooseGeometryAlpha, wxT( "Geometry Alpha..." ), wxString( "Change the alpha of the selected geometry." ) );
	renderMenu->AppendSeparator();
	renderMenu->Append( ID_RenderDebug, wxT( "Debug" ), debugMenu );

	menuBar = new wxMenuBar;
	menuBar->Append( renderMenu, wxT( "Render Options" ) );
	SetMenuBar( menuBar );

	wxPanel* controlPanel = new wxPanel( this );

	canvas = new GAVisToolCanvas( controlPanel );

	wxBoxSizer* panelSizer = new wxBoxSizer( wxVERTICAL );
	panelSizer->Add( canvas, 1, wxGROW | wxALL );
	controlPanel->SetSizer( panelSizer );

	auiManager.AddPane( controlPanel, wxAuiPaneInfo().Name( wxT( "Canvas" ) ).Caption( wxT( "Canvas" ) ).CenterPane().PaneBorder( true ) );

	statusBar = new wxStatusBar( this );
	SetStatusBar( statusBar );

	auiManager.Update();

	UpdateUserInterface();
}

// CanvasFrame.cpp