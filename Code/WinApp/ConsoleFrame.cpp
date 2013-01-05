// ConsoleFrame.cpp

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "ConsoleFrame.h"
#include "Application.h"
#include "resource.h"

//=========================================================================================
BEGIN_EVENT_TABLE( GAVisToolConsoleFrame, wxFrame )
	
	EVT_SIZE( GAVisToolConsoleFrame::OnSize )
	EVT_SET_FOCUS( GAVisToolConsoleFrame::OnSetFocus )
	EVT_IDLE( GAVisToolConsoleFrame::OnIdle )

	EVT_MENU( GAVisToolConsoleFrame::ID_About, OnAbout )
	EVT_MENU( GAVisToolConsoleFrame::ID_Exit, OnExit )
	EVT_MENU( GAVisToolConsoleFrame::ID_ToggleLatexOutput, OnToggleLatexOutput )
	EVT_MENU( GAVisToolConsoleFrame::ID_SpecifyLatexCommandDir, OnSpecifyLatexCommandDir )
	EVT_MENU( GAVisToolConsoleFrame::ID_ClearConsoleOutput, OnClearConsoleOutput )
	EVT_MENU( GAVisToolConsoleFrame::ID_ChooseFont, OnChooseFont )
	EVT_MENU( GAVisToolConsoleFrame::ID_ResetEnvironment, OnResetEnvironment )
	EVT_MENU( GAVisToolConsoleFrame::ID_ExecuteScript, OnExecuteScript )
	EVT_MENU( GAVisToolConsoleFrame::ID_ShowInventory, OnShowInventory )

	EVT_TEXT_ENTER( GAVisToolConsoleFrame::ID_TextEnter, OnTextEnter )

END_EVENT_TABLE()

//=========================================================================================
GAVisToolConsoleFrame::GAVisToolConsoleFrame( const wxPoint& pos, const wxSize& size ) : wxFrame( ( wxFrame* )NULL, wxID_ANY, "Console", pos, size )
{
	consoleOutputCtrl = 0;
	consoleInputCtrl = 0;

	menuBar = 0;
	statusBar = 0;

	BuildUserInterface();
}

//=========================================================================================
/*virtual*/ GAVisToolConsoleFrame::~GAVisToolConsoleFrame( void )
{
}

//=========================================================================================
void GAVisToolConsoleFrame::OnSetFocus( wxFocusEvent& event )
{
	consoleInputCtrl->SetFocus();
}

//=========================================================================================
void GAVisToolConsoleFrame::OnIdle( wxIdleEvent& event )
{
}

//=========================================================================================
void GAVisToolConsoleFrame::OnSize( wxSizeEvent& event )
{
	wxFrame::OnSize( event );
}

//=========================================================================================
void GAVisToolConsoleFrame::OnExit( wxCommandEvent& event )
{
	wxGetApp().ExitApp();
}

//=========================================================================================
void GAVisToolConsoleFrame::OnAbout( wxCommandEvent& WXUNUSED( event ) )
{
	wxAboutDialogInfo aboutDlgInfo;

	aboutDlgInfo.SetName( wxT( "GAVisTool" ) );
	aboutDlgInfo.SetVersion( wxT( "0.5 Beta" ) );
#if 0
	aboutDlgInfo.SetDescription( wxT( "Please send bugs to spencer.parkin@gmail.com.\n"
										"Comments and questions are welcome.\n"
										"Thank you for your interest in this program." ) );
#else
	aboutDlgInfo.SetDescription( wxT( "Please send bugs to spencer.parkin@gmail.com.\nComments and questions are welcome.\nThank you for your interest in this program." ) );
#endif
	aboutDlgInfo.SetCopyright( wxT( "Copyright (C) 2012" ) );
	aboutDlgInfo.SetWebSite( wxT( "https://github.com/spencerparkin/GAVisTool" ) );
	aboutDlgInfo.AddDeveloper( wxT( "Spencer T. Parkin" ) );
	aboutDlgInfo.SetLicense( wxT( "MIT License" ) );

	// This doesn't work.  :(
	HICON hIcon = ::LoadIcon( wxGetInstance(), MAKEINTRESOURCE( IDI_ICON ) );
	if( hIcon != NULL )
	{
		wxIcon icon;
		icon.SetHICON( hIcon );
		aboutDlgInfo.SetIcon( icon );
	}

	wxAboutBox( aboutDlgInfo );
}

//=========================================================================================
void GAVisToolConsoleFrame::OnToggleLatexOutput( wxCommandEvent& event )
{
	wxGetApp().SetGenerateLatexOutput( !wxGetApp().GetGenerateLatexOutput() );
	UpdateUserInterface();
}

//=========================================================================================
void GAVisToolConsoleFrame::OnSpecifyLatexCommandDir( wxCommandEvent& event )
{
	wxString latexCommandDir = wxGetApp().GetLatexCommandDir();
	wxDirDialog dirDialog( 0, "Locate LaTeX binaries directory", latexCommandDir, wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST );
	if( wxID_OK == dirDialog.ShowModal() )
	{
		latexCommandDir = dirDialog.GetPath();
		wxGetApp().SetLatexCommandDir( latexCommandDir );
	}
}

//=========================================================================================
void GAVisToolConsoleFrame::OnClearConsoleOutput( wxCommandEvent& event )
{
	consoleOutputCtrl->Clear();
}

//=========================================================================================
void GAVisToolConsoleFrame::OnChooseFont( wxCommandEvent& event )
{
	// TODO: Save chosen font to the registry for restoration upon program boot-up.

	wxRichTextRange entireRange( 0, consoleOutputCtrl->GetLastPosition() + 1 );

	wxRichTextFormattingDialog formatDlg( wxRICHTEXT_FORMAT_FONT, this );
	formatDlg.GetStyle( consoleOutputCtrl, entireRange );

	if( wxID_OK == formatDlg.ShowModal() )
	{
		consoleOutputCtrl->SetStyle( entireRange, formatDlg.GetAttributes() );

		// TODO: Also set the font of the input control to the same thing.
	}
}

//=========================================================================================
void GAVisToolConsoleFrame::OnResetEnvironment( wxCommandEvent& event )
{
	wxGetApp().environment->Wipe( true, true );
	wxGetApp().canvasFrame->canvas->RedrawNeeded( true );
}

//=========================================================================================
void GAVisToolConsoleFrame::OnExecuteScript( wxCommandEvent& event )
{
	wxFileDialog fileDlg( this, wxT( "Open Script File" ) );
	if( wxID_OK == fileDlg.ShowModal() )
	{
		wxString scriptFile = fileDlg.GetPath();
		wxFile file;
		if( !file.Open( scriptFile, wxFile::read ) )
			wxMessageBox( wxString::Format( "Failed to open file %s!", scriptFile.c_str() ), wxT( "Oops!" ) );
		else
		{
			wxFileOffset fileLength = file.Length();
			char* scriptText = new char[ fileLength + 1 ];
			if( fileLength != file.Read( scriptText, fileLength ) )
				wxMessageBox( wxString::Format( "Failed to read file %s!", scriptFile.c_str() ), wxT( "Oops!" ) );
			else
			{
				scriptText[ fileLength ] = '\0';
				wxString inputText = scriptText;
				ProcessConsoleInput( inputText );
			}
			delete[] scriptText;
			file.Close();
		}
	}
}

//=========================================================================================
void GAVisToolConsoleFrame::OnShowInventory( wxCommandEvent& event )
{
	GAVisToolInventoryTree*& inventoryTree = wxGetApp().canvasFrame->inventoryTree;
	
	wxAuiManager& auiManager = wxGetApp().canvasFrame->auiManager;
	wxAuiPaneInfo& paneInfo = auiManager.GetPane( wxT( "InventoryTree" ) );
	if( !paneInfo.IsOk() )
		auiManager.AddPane( inventoryTree, wxAuiPaneInfo().Name( wxT( "InventoryTree" ) ).Caption( wxT( "Inventory" ) ).Left().CloseButton( true ) );
	else
		paneInfo.Dock().Left().Show();
	
	inventoryTree->RegenerateTree();
	auiManager.Update();
}

//=========================================================================================
void GAVisToolConsoleFrame::OnTextEnter( wxCommandEvent& WXUNUSED( event ) )
{
	wxString inputText = consoleInputCtrl->GetValue();
	consoleInputCtrl->Clear();

	ProcessConsoleInput( inputText );

	// Make sure we can always see what has been appended to the output window.
	long caretPos = consoleOutputCtrl->GetCaretPosition();
	consoleOutputCtrl->ScrollIntoView( caretPos, WXK_PAGEDOWN );

	// Record the input text in the command history.
	commandHistory.RememberCommand( inputText );
}

//=========================================================================================
void GAVisToolConsoleFrame::ProcessConsoleInput( const wxString& inputText )
{
	wxImage outputImage;
	wxString outputText;
	bool imageGenerated = wxGetApp().ProcessConsoleInput( inputText, outputText, outputImage );

	if( !outputText.IsEmpty() )
		consoleOutputCtrl->AppendText( outputText );

	if( imageGenerated )
		consoleOutputCtrl->WriteImage( outputImage );

	if( !outputText.IsEmpty() )
		consoleOutputCtrl->AppendText( "\n\n" );
}

//=========================================================================================
void GAVisToolConsoleFrame::OnKeyDown( wxKeyEvent& event )
{
	event.Skip();
}

//=========================================================================================
void GAVisToolConsoleFrame::OnKeyUp( wxKeyEvent& event )
{
	wxString command;
	if( event.GetKeyCode() == WXK_UP )
		command = commandHistory.RecallCommandUp();
	else if( event.GetKeyCode() == WXK_DOWN )
		command = commandHistory.RecallCommandDown();
	if( !command.IsEmpty() )
	{
		consoleInputCtrl->Clear();
		consoleInputCtrl->AppendText( command );
	}
}

//=========================================================================================
GAVisToolConsoleFrame::CommandHistory::CommandHistory( void )
{
	commandBufferSearchIndex = 0;
}

//=========================================================================================
/*virtual*/ GAVisToolConsoleFrame::CommandHistory::~CommandHistory( void )
{
}

//=========================================================================================
void GAVisToolConsoleFrame::CommandHistory::RememberCommand( const wxString& command )
{
	commandBufferSearchIndex = commandBuffer.Add( command ) + 1;
}

//=========================================================================================
wxString GAVisToolConsoleFrame::CommandHistory::RecallCommandUp( void )
{
	wxString command;
	for( unsigned int count = 0; count < commandBuffer.size(); count++ )
	{
		if( --commandBufferSearchIndex < 0 )
			commandBufferSearchIndex = commandBuffer.size() - 1;
		command = commandBuffer[ commandBufferSearchIndex ];
		if( !command.IsEmpty() )
			break;
	}
	return command;
}

//=========================================================================================
wxString GAVisToolConsoleFrame::CommandHistory::RecallCommandDown( void )
{
	wxString command;
	for( unsigned int count = 0; count < commandBuffer.size(); count++ )
	{
		commandBufferSearchIndex = ( commandBufferSearchIndex + 1 ) % commandBuffer.size();
		command = commandBuffer[ commandBufferSearchIndex ];
		if( !command.IsEmpty() )
			break;
	}
	return command;
}

//=========================================================================================
// Update the user interface controls as a function of the application's internal settings.
void GAVisToolConsoleFrame::UpdateUserInterface( void )
{
	wxMenuItem* generateLatextOutputMenuItem = menuBar->FindItem( ID_ToggleLatexOutput );
	generateLatextOutputMenuItem->Check( wxGetApp().GetGenerateLatexOutput() );
}

//=========================================================================================
void GAVisToolConsoleFrame::BuildUserInterface( void )
{
	wxMenu* fileMenu = new wxMenu;
	fileMenu->Append( ID_ExecuteScript, wxT( "Execute Script..." ), wxT( "Load and execute the contents of the given text file as a script." ) );
	fileMenu->AppendSeparator();
	fileMenu->Append( ID_Exit, wxT( "E&xit" ) );

	wxMenu* envMenu = new wxMenu;
	envMenu->Append( ID_ShowInventory, wxT( "Show Inventory" ), wxT( "Bring up the inventory pane which lists all currently created constraints and bind targets." ) );
	envMenu->AppendSeparator();
	envMenu->Append( ID_ResetEnvironment, wxT( "Reset Environment" ), wxT( "Delete all bindings and all variables." ) );
	envMenu->Append( ID_ClearConsoleOutput, wxT( "Clear Console Output" ), wxT( "Reset the console output to a blank screen." ) );

	wxMenu* optionsMenu = new wxMenu;
	optionsMenu->Append( ID_ChooseFont, wxT( "Choose Font..." ), wxT( "Choose what font to use in the console window." ) );
	optionsMenu->AppendSeparator();
	optionsMenu->Append( ID_ToggleLatexOutput, wxT( "Latex Output" ), wxT( "If enabled, latex output is generated in the console window." ), true );
	optionsMenu->Append( ID_SpecifyLatexCommandDir, wxT( "Latex Command Directory..." ), wxT( "Tell GAVisTool where your latex command installation resides." ) );

	wxMenu* helpMenu = new wxMenu;
	helpMenu->Append( ID_About, wxT( "About" ) );

	menuBar = new wxMenuBar;
	menuBar->Append( fileMenu, wxT( "&File" ) );
	menuBar->Append( envMenu, wxT( "&Environment" ) );
	menuBar->Append( optionsMenu, wxT( "&Options" ) );
	menuBar->Append( helpMenu, wxT( "&Help" ) );
	SetMenuBar( menuBar );

	wxPanel* controlPanel = new wxPanel( this );
	
	consoleOutputCtrl = new wxRichTextCtrl( controlPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxRE_READONLY | wxRE_MULTILINE );
	consoleOutputCtrl->AppendText( "Welcome to GAVisTool!\n\n" );

	consoleInputCtrl = new wxTextCtrl( controlPanel, ID_TextEnter, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	consoleInputCtrl->Connect( wxID_ANY, wxID_ANY, wxEVT_KEY_UP, wxKeyEventHandler( GAVisToolConsoleFrame::OnKeyUp ), 0, this );
	consoleInputCtrl->Connect( wxID_ANY, wxID_ANY, wxEVT_KEY_DOWN, wxKeyEventHandler( GAVisToolConsoleFrame::OnKeyDown ), 0, this );
	consoleInputCtrl->SetFocus();

	wxBoxSizer* panelSizer = new wxBoxSizer( wxVERTICAL );
	panelSizer->Add( consoleOutputCtrl, 1, wxGROW | wxALL );
	panelSizer->Add( consoleInputCtrl, 0, wxGROW | wxNORTH, 2 );
	controlPanel->SetSizer( panelSizer );

	wxBoxSizer* frameSizer = new wxBoxSizer( wxVERTICAL );
	frameSizer->Add( controlPanel, 1, wxGROW | wxALL );
	SetSizer( frameSizer );

	statusBar = new wxStatusBar( this );
	statusBar->PushStatusText( wxT( "GAVisTool is ready for input!" ) );
	SetStatusBar( statusBar );

	UpdateUserInterface();
}

// ConsoleFrame.cpp