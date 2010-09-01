/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*     A compound control class. It is responsible for owning CVRButtonPanel
*     and CVRStateInfoPanel. It also handles the changing of softkeys, menubar
*     and volume control.
*
*/


#ifndef __CVRRECVIEWCONTAINER_H__
#define __CVRRECVIEWCONTAINER_H__

// INCLUDES
#include <bldvariant.hrh>
#include <coecntrl.h>
#include "MVRObserver.h"
#include "MVRVolumeEventObserver.h"
#include <AknUtils.h>

// FORWARD DECLARATIONS
class CVRButtonPanel;
class CVRStateInfoPanel;
class MVRButtonPanelModel;
class MVRDecoratorModel;
class MVRStateInfoModel;
class MVRKeyObserver;
class MVRVolumeChangeObserver;
class MVRVolumeEventObserver;
class CVRRecView;
class CAknNavigationDecorator;
class CAknNavigationControlContainer;
class MAknsSkinInstance;
class CAknsBasicBackgroundControlContext;
class CVRRemConObserver;


// CLASS DEFINITION
/**
* A compound control class. It is responsible for owning CVRButtonPanel
* and CVRStateInfoPanel. It also handles the changing of softkeys, menubar,
* volume control and navipane. It also observers volume changes through
* MRemConCoreApiTargetObserver interface.
*/
NONSHARABLE_CLASS( CVRRecViewContainer )
	: public CCoeControl, 
	  public MVRObserver, 
	  public MVRVolumeEventObserver,
	  public MCoeControlObserver
	{
	public:	// Constructors and destructor

		/**
		* Default constructor.
		*/
		CVRRecViewContainer();

		/**
		* Destructor
		*/
		~CVRRecViewContainer();

		/**
		* 2nd phase constructor
		* @param aRect The screen rectangle for this component
		* @param aButtonModelPointer to a button panel observer (or NULL)
		* @param aDecoratorModel Pointer to a decorator observer (or NULL)
		* @param aStateInfoModel Pointer to a state info observer (or NULL)
		* @param aOwningView Pointer to the view that owns us.
		*/
		void ConstructL( const TRect& aRect,
						 MVRButtonPanelModel* aButtonModel,
						 MVRDecoratorModel* aDecoratorModel,
						 MVRStateInfoModel* aStateInfoModel,
						 CVRRecView* aOwningView );

	public: // from CCoeControl

		/**
		* Specifies the help context for this form.
		* @param aContext the context IDs are placed here
		*/
		void GetHelpContext( TCoeHelpContext& aContext ) const;

	public:
		// from CCoeControl

		/**
		* Called by the framework to get the number of sub-components
		* contained in this component.
		* @return The number of component controls contained by this control
		*/
		TInt CountComponentControls() const;

		/**
		* Called by the framework to get a specified sub-component
		* of this component.
		* @param aIndex The index of the component to get.
		* @return The component control with an index of aIndex.
		*/
		CCoeControl* ComponentControl( TInt aIndex ) const;

		/**
		* When a key event occurs, the control framework calls this function
		* for each control on the control stack, until one of them can process
		* the key event (and returns EKeyWasConsumed).
		* @param aKeyEvent The key event.
		* @param aType The type of key event: EEventKey, EEventKeyUp
		*              or EEventKeyDown
		* @return Indicates whether or not the key event was used by this
		*         control
		*/
		TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
									 TEventCode aType );

	private: // from CCoeControl

		/*
		* From CCoeControl
		* @see CCoeControl
		*/
		void SizeChanged();

		/**
		* Draws this control.
		* @param aRect The area that needs updating. Ignored.
		*/
		void Draw( const TRect& aRect ) const;

		/*
		* From CCoeControl
		* @see CCoeControl
		*/
		TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
		
		/*
		* From CCoeControl
		* @see CCoeControl
		*/
		void HandlePointerEventL( const TPointerEvent& aPointerEvent );

	    /**
		*From MCoeControlObserver. To handle the volume events for touch UI.
		*/
    	void HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType );



	public: // from MVRObserver

		/**
		* Called to notify a change in the observed subject's state.
		*/
		void Update( TVRUpdateCommand aCommand = EVRUpdate );

	public: // from MVRVolumeEventObserver

		/**
		* Handles volume change event received from either side volume
		* keys or left-right rocker presses.
		* @param aKeyEvent The key event. Will be forwarded to CAknVolumeControl
		* @param aType The type of key event: EEventKey, EEventKeyUp
		*              or EEventKeyDown. Will be forwarded to CAknVolumeControl
		*/

		TInt HandleVolumeChangeL( const TKeyEvent& aKeyEvent,
									 TEventCode aType );		


	public: // new methods

		/**
		* Registers an observer for receiving
		* key event notifications from this class.
		* @param aObserver The observer object.
		*/
		void SetKeyObserver( MVRKeyObserver* aObserver );

		/**
		* Registers an observer for receiving
		* audio volume change notifications from this class.
		* @param aObserver The observer object.
		*/
		void SetVolumeChangeObserver( MVRVolumeChangeObserver* aObserver );

		/**
		* Updates the layout of all child controls to current active
		* layout (Euro, Apac, Arabic/Hebrew)
		*/
		void UpdateLayoutL();

		/**
		* Handles a change to the application's resources
		* @param aType The type of changed resource
		*/
		void HandleResourceChangeL( TInt aType );
		
		/**
		* Returns the currently focused button index in Button Panel
		* @return The focused button index
		*/
		TInt FocusedButton() const;
		
		/**
		* Gives the update command to Button Panel
		* @param Update command
		*/
		void UpdateButtonPanel( TVRUpdateCommand aCommand );


	private:

		/**
		* Tells the volume observer to update the volume
		*/
		void UpdateVolumeObserver();

		/**
		* Updates the navipane according to the current IHF setting
		*/
		void UpdateVolumeControlL();
		
		/**
		* Updates the CBA Buttons (LSK, RSK, MSK)
		*/
		void UpdateCBA();

	private: // data

		/**
		* The ihf volume control (in navi pane),
		*/
		CAknNavigationDecorator* iIhfVolumeControl;

		/**
		* The ear piece volume control (in navi pane),
		*/
		CAknNavigationDecorator* iEarPieceVolumeControl;

		/**
		* A pointer to the active volume control (ihf or ear piece)
		*/
		CAknNavigationDecorator* iActiveVolumeControl;

		/**
		* Pointer to the navi pane,
		* used as a short cut. Not owned.
		*/
		CAknNavigationControlContainer* iNaviPane;

		/**
		* Pointer to the button panel component. Owned.
		*/
		CVRButtonPanel* iButtonPanel;

		/**
		* Pointer to the state info panel component. Owned.
		*/
		CVRStateInfoPanel* iStateInfoPanel;

		/**
		* Pointer to the view that owns us. Not owned.
		*/
		CVRRecView* iOwningView;

		/**
		* Pointer to the decorator data model. Not owned.
		*/
		MVRDecoratorModel* iModel;

		/**
		* Pointer to the key event observer (or NULL). Not owned.
		*/
		MVRKeyObserver* iKeyObserver;

		/**
		* Pointer to the audio volume change observer (or NULL). Not owned.
		*/
		MVRVolumeChangeObserver* iVolumeChangeObserver;

		TRect iHorizontalLine;
		TRect iVerticalLine;

		TAknLayoutRect	iHorizontalLineRect;
		TAknLayoutRect	iVerticalLineRect;
		TAknLayoutRect	iButtonPanelRect;

	    /**
	    * Skin instance from AknsUtils. Not owned.
	    */
		MAknsSkinInstance* iSkinInstance;

        /**
        * Background context for clearing screen. Owned.
        */
		CAknsBasicBackgroundControlContext* iBackgroundSkinContext;	
		
		/**
		* Notifier for side volume key events. Owned.
		*/
		CVRRemConObserver* iRemConObserver;
	};

#endif // __CVRRECVIEWCONTAINER_H__
