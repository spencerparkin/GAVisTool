// ScalarInterface.cpp

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "ScalarInterface.h"
#include "../Application.h"

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( ScalarInterface, GAVisToolInterface );

//=========================================================================================
ScalarInterface::ScalarInterface( BindType bindType ) : GAVisToolInterface( bindType )
{
	min = -10.0;
	max = 10.0;
	scalar = 0.0;

	wxWindow* parent = wxGetApp().canvasFrame;
	panel = new Panel( parent );
	panel->scalarInterface = this;
}

//=========================================================================================
/*virtual*/ ScalarInterface::~ScalarInterface( void )
{
	// wxWdigets deletes this for us.
//	delete panel;
//	panel = 0;
}

//=========================================================================================
/*static*/ GAVisToolBindTarget* ScalarInterface::Create( BindType bindType )
{
	return new ScalarInterface( bindType );
}

//=========================================================================================
/*virtual*/ void ScalarInterface::DecomposeFrom( const GeometricAlgebra::SumOfBlades& element )
{
	if( !element.AssignScalarTo( scalar ) )
	{
		// We should probably complain to the user in this case.  Be silent for now.
		scalar = 0.0;
	}
	else
	{
		AdjustBoundsAgainstScalar();
		PushDataToInterface();
	}
}

//=========================================================================================
/*virtual*/ void ScalarInterface::ComposeTo( GeometricAlgebra::SumOfBlades& element ) const
{
	element.AssignScalar( scalar );
}

//=========================================================================================
/*virtual*/ void ScalarInterface::DumpInfo( char* printBuffer, int printBufferSize ) const
{
	sprintf_s( printBuffer, printBufferSize,
			"The variable \"%s\" is being interpreted as a scalar and exposed through a scalar interface.\n"
			"Scalar: %f\n"
			"Min: %f\n"
			"Max: %f\n",
			name,
			scalar,
			min,
			max );
}

//=========================================================================================
/*virtual*/ void ScalarInterface::Initialize( void )
{
	wxString caption = wxString::Format( "Scalar Interface: %s", GetName() );
	PushDataToInterface();
	InstallPanel( panel, caption );
}

//=========================================================================================
/*virtual*/ void ScalarInterface::Finalize( void )
{
	UninstallPanel( panel );
}

//=========================================================================================
void ScalarInterface::PushDataToInterface( void )
{
	double t = ( scalar - min ) / ( max - min );
	double v = t * double( Panel::SliderResolution );
	panel->slider->SetValue( int(v) );

	panel->minTextCtrl->Clear();
	panel->minTextCtrl->AppendText( wxString::Format( "%f", min ) );
	panel->maxTextCtrl->Clear();
	panel->maxTextCtrl->AppendText( wxString::Format( "%f", max ) );
}

//=========================================================================================
void ScalarInterface::PullDataFromInterface( void )
{
	wxString minValue = panel->minTextCtrl->GetValue();
	minValue.ToDouble( &min );
	wxString maxValue = panel->maxTextCtrl->GetValue();
	maxValue.ToDouble( &max );

	double v = double( panel->slider->GetValue() );
	double t = v / double( Panel::SliderResolution );
	scalar = min + t * ( max - min );
}

//=========================================================================================
void ScalarInterface::AdjustScalarAgainstBounds( void )
{
	if( scalar < min )
		scalar = min;
	if( scalar > max )
		scalar = max;
}

//=========================================================================================
void ScalarInterface::AdjustBoundsAgainstScalar( void )
{
	if( scalar < min )
		min = scalar;
	if( scalar > max )
		max = scalar;
}

//=========================================================================================
bool ScalarInterface::SetMin( double min )
{
	if( min > max )
		return false;
	this->min = min;
	AdjustScalarAgainstBounds();
	return true;
}

//=========================================================================================
bool ScalarInterface::SetMax( double max )
{
	if( max < min )
		return false;
	this->max = max;
	AdjustScalarAgainstBounds();
	return true;
}

//=========================================================================================
double ScalarInterface::GetMin( void )
{
	return min;
}

//=========================================================================================
double ScalarInterface::GetMax( void )
{
	return max;
}

//=========================================================================================
BEGIN_EVENT_TABLE( ScalarInterface::Panel, wxPanel )
	EVT_COMMAND_SCROLL( ID_Slider, OnSlider )
	EVT_TEXT_ENTER( ID_MinText, OnMinTextEnter )
	EVT_TEXT_ENTER( ID_MaxText, OnMaxTextEnter )
END_EVENT_TABLE()

//=========================================================================================
ScalarInterface::Panel::Panel( wxWindow* parent ) : wxPanel( parent, wxID_ANY, wxDefaultPosition, wxSize( 100, 100 ) )
{
	slider = new wxSlider( this, ID_Slider, 0, 0, 10, wxDefaultPosition, wxSize( 50, 50 ), wxSL_HORIZONTAL );
	slider->SetMin( 0 );
	slider->SetMax( SliderResolution );

	minLabel = new wxStaticText( this, ID_MinLabel, wxT( "Min:" ), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	maxLabel = new wxStaticText( this, ID_MaxLabel, wxT( "Max:" ), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );

	minTextCtrl = new wxTextCtrl( this, ID_MinText, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	maxTextCtrl = new wxTextCtrl( this, ID_MaxText, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );

	wxBoxSizer* subSizer = new wxBoxSizer( wxHORIZONTAL );
	subSizer->Add( minLabel, 0, wxLEFT | wxRIGHT, 5 );
	subSizer->Add( minTextCtrl, 1, wxGROW | wxALL );
	subSizer->Add( maxLabel, 0, wxLEFT | wxRIGHT, 5 );
	subSizer->Add( maxTextCtrl, 1, wxGROW | wxALL );

	wxBoxSizer* sizer = new wxBoxSizer( wxVERTICAL );
	sizer->Add( slider, 0, wxGROW | wxALL );
	sizer->Add( subSizer, 0, wxGROW | wxALL );
	SetSizer( sizer );

	scalarInterface = 0;
}

//=========================================================================================
/*virtual*/ ScalarInterface::Panel::~Panel( void )
{
}

//=========================================================================================
void ScalarInterface::Panel::OnSlider( wxScrollEvent& event )
{
	if( scalarInterface )
	{
		scalarInterface->PullDataFromInterface();
		scalarInterface->HasChanged( true );
	}
}

//=========================================================================================
// wxWidgets supports validators.  I should probably figure out how to use those.
void ScalarInterface::Panel::OnMinTextEnter( wxCommandEvent& event )
{
	wxString minValue = minTextCtrl->GetValue();
	if( !minValue.IsNumber() )
		return;

	double min;
	if( !minValue.ToDouble( &min ) )
		return;

	scalarInterface->SetMin( min );
	scalarInterface->PushDataToInterface();
}

//=========================================================================================
void ScalarInterface::Panel::OnMaxTextEnter( wxCommandEvent& event )
{
	wxString maxValue = maxTextCtrl->GetValue();
	if( !maxValue.IsNumber() )
		return;

	double max;
	if( !maxValue.ToDouble( &max ) )
		return;

	scalarInterface->SetMax( max );
	scalarInterface->PushDataToInterface();
}

//=========================================================================================
/*virtual*/ void ScalarInterface::AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const
{
	wxString itemName = wxString::Format( wxT( "Scalar i-face: %s" ), name );
	treeCtrl->AppendItem( parentItem, itemName, -1, -1, new GAVisToolInventoryTree::Data( id ) );
}

// ScalarInterface.cpp