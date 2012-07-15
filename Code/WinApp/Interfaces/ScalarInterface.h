// ScalarInterface.h

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "../Interface.h"
#include "../wxAll.h"

class ScalarInterfacePanel;

//=========================================================================================
class ScalarInterface : public GAVisToolInterface
{
	DECLARE_CALCLIB_CLASS( ScalarInterface );

public:

	ScalarInterface( BindType bindType );
	virtual ~ScalarInterface( void );

	static GAVisToolBindTarget* Create( BindType bindType );

	virtual void DecomposeFrom( const GeometricAlgebra::SumOfBlades& element ) override;
	virtual void ComposeTo( GeometricAlgebra::SumOfBlades& element ) const override;

	virtual void DumpInfo( char* printBuffer, int printBufferSize ) const override;

	virtual void AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const override;

	virtual void Initialize( void );
	virtual void Finalize( void );

	bool SetMin( double min );
	bool SetMax( double max );
	double GetMin( void );
	double GetMax( void );

private:

	void AdjustScalarAgainstBounds( void );
	void AdjustBoundsAgainstScalar( void );

	// TODO: How hard would it be to detect that we docked onto the side of the
	//       frame, and then fix the orientation of the slider to be vertical?
	class Panel : public wxPanel
	{
	public:

		Panel( wxWindow* parent );
		virtual ~Panel( void );

		enum
		{
			ID_Slider = wxID_HIGHEST,
			ID_MinLabel,
			ID_MaxLabel,
			ID_MinText,
			ID_MaxText,
		};

		enum
		{
			SliderResolution = 200,
		};

		void OnSlider( wxScrollEvent& event );
		void OnMinTextEnter( wxCommandEvent& event );
		void OnMaxTextEnter( wxCommandEvent& event );

		wxSlider* slider;
		wxStaticText* minLabel;
		wxStaticText* maxLabel;
		wxTextCtrl* minTextCtrl;
		wxTextCtrl* maxTextCtrl;
		ScalarInterface* scalarInterface;

		DECLARE_EVENT_TABLE()
	};

	// wxWidgets probably has a better way of doing this.
	void PushDataToInterface( void );
	void PullDataFromInterface( void );

	Panel* panel;
	double scalar;
	double min, max;
};

// ScalarInterface.h