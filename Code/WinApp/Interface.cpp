// Interface.cpp

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "Interface.h"
#include "Application.h"

IMPLEMENT_CALCLIB_CLASS0( GAVisToolInterface );

//=========================================================================================
GAVisToolInterface::GAVisToolInterface( BindType bindType ) : GAVisToolBindTarget( bindType )
{
}

//=========================================================================================
/*virtual*/ GAVisToolInterface::~GAVisToolInterface( void )
{
}

//=========================================================================================
void GAVisToolInterface::InstallPanel( wxPanel* panel, const wxString& caption )
{
	wxAuiManager* auiManager = &wxGetApp().canvasFrame->auiManager;
	auiManager->AddPane( panel, wxAuiPaneInfo().Name( GetName() ).Caption( caption ).Bottom().CloseButton( false ) );
	auiManager->Update();
}

//=========================================================================================
void GAVisToolInterface::UninstallPanel( wxPanel* panel )
{
	wxAuiManager* auiManager = &wxGetApp().canvasFrame->auiManager;
	auiManager->DetachPane( panel );
	auiManager->Update();
}

// Interface.cpp