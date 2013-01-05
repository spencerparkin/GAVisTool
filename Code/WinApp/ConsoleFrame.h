// ConsoleFrame.h

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

//=========================================================================================
class GAVisToolConsoleFrame : public wxFrame
{
public:

	GAVisToolConsoleFrame( const wxPoint& pos, const wxSize& size );
	virtual ~GAVisToolConsoleFrame( void );

	void OnSize( wxSizeEvent& event );
	void OnSetFocus( wxFocusEvent& event );
	void OnIdle( wxIdleEvent& event );
	void OnExit( wxCommandEvent& event );
	void OnAbout( wxCommandEvent& event );
	void OnToggleLatexOutput( wxCommandEvent& event );
	void OnSpecifyLatexCommandDir( wxCommandEvent& event );
	void OnClearConsoleOutput( wxCommandEvent& event );
	void OnChooseFont( wxCommandEvent& event );
	void OnResetEnvironment( wxCommandEvent& event );
	void OnExecuteScript( wxCommandEvent& event );
	void OnShowInventory( wxCommandEvent& event );
	void OnTextEnter( wxCommandEvent& event );
	void OnKeyUp( wxKeyEvent& event );
	void OnKeyDown( wxKeyEvent& event );

	enum
	{
		ID_Exit = wxID_EXIT,
		ID_About = wxID_ABOUT,
		ID_ToggleLatexOutput = wxID_HIGHEST,
		ID_SpecifyLatexCommandDir,
		ID_ClearConsoleOutput,
		ID_ChooseFont,
		ID_ResetEnvironment,
		ID_ExecuteScript,
		ID_ShowInventory,
		ID_TextEnter,
	};

	void BuildUserInterface( void );
	void UpdateUserInterface( void );

	void ProcessConsoleInput( const wxString& inputText );

	wxTextCtrl* consoleInputCtrl;
	wxRichTextCtrl* consoleOutputCtrl;		// We will want to support LaTeX output, so we will need a rich text control.

	wxMenuBar* menuBar;
	wxStatusBar* statusBar;

	class CommandHistory
	{
	public:

		CommandHistory( void );
		virtual ~CommandHistory( void );

		void RememberCommand( const wxString& command );
		wxString RecallCommandUp( void );
		wxString RecallCommandDown( void );

	private:

		wxArrayString commandBuffer;
		int commandBufferSearchIndex;
	};

	CommandHistory commandHistory;

	DECLARE_EVENT_TABLE()
};

// ConsoleFrame.h