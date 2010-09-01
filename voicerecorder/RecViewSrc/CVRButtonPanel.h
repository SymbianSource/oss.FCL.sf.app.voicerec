/*
* Copyright (c) 2002 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*     A compound control class. It is responsible for drawing and updating
*     a set of buttons. Button selections are also handled and forwarded
*     as commands to the AppUi.
*
*/


#ifndef __CVRBUTTONPANEL_H__
#define __CVRBUTTONPANEL_H__

// INCLUDES
#include <coecntrl.h>
#include <AknUtils.h>
#include <coecobs.h>	// MCoeControlObserver

#include "MVRObserver.h"

// FORWARD DECLARATIONS
class MVRButtonPanelModel;
class MAknsSkinInstance;
class CApaMaskedBitmap;
class TAknsItemID;
class CAknButton;

// CLASS DEFINITION
/**
* A compound control class. It is responsible for drawing and updating
* a set of buttons. Button selections are also handled and forwarded
* as commands to the AppUi.
*/
NONSHARABLE_CLASS( CVRButtonPanel )
	: 	public CCoeControl, 
		public MVRObserver, 
		public MCoeControlObserver,
		public MCoeControlBackground
	{
	public: // data types
	    enum TFocusChange 
	        {
	        EFocusUp = -1,
	        EFocusDown = 1,
	        EFocusInvalid = 2
	        };
	
	// pointer event types used in VR        
	enum TLastPointerEventType
	        {
	        EVRButtonDownEvent,
	        EVRButtonDragOutsideEvent,
	        EVRButtonUpEvent
	        };
	        
	public: // constructors and destructor
		
		/**
		* Constructor.
		* @param aModel Pointer to the button panel model.
		* Ownership is not transferred.
		*/
		CVRButtonPanel( MVRButtonPanelModel* aModel );

		/**
		* Constructor.
		* @param aModel Pointer to the button panel model.
		* @param aParentViewUid Uid of CVRRecView for fast swap
		* Ownership is not transferred.
		*/
		CVRButtonPanel( MVRButtonPanelModel* aModel, TUid aParentViewUid );

		/**
		* Destructor
		*/
		virtual ~CVRButtonPanel();

		/**
		* 2nd phase constructor.
		*/
		void ConstructL();

	private: // constructors

		/**
		* Default constructor. Not implemented.
		*/
		CVRButtonPanel();
		
		/**
		* Creates control buttons from given resources
		* @param aButtonsResourceId The resource used in buttons' construction.
		*/
		void CreateButtonsFromResourcesL( TInt aButtonsResourceId );

	public:
		
		/**
		* Updates the layout of all child controls to current active
		* layout (Euro, Apac, Arabic/Hebrew)
		*/		
		void UpdateLayoutL( );

		/**
		* Handles a change to the application's resources
		* @param aType The type of changed resource
		*/
		void HandleResourceChangeL( TInt aType );
		
	public: // from CCoeControl

		/*
		* From CCoeControl
		* @see CCoeControl
		*/
		void SizeChanged();

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
		
		/**
		* Gets called whenever a pointer event occurs.
		* @param aPointerEvent The pointer event to handle.
		*/							 
		void HandlePointerEventL( const TPointerEvent& aPointerEvent );
		
	public:	// from MVRObserver

		/**
		* Called to notify a change in the observed subject's state.
		*/
		void Update( TVRUpdateCommand aCommand );
	
	public:	// from MCoeControlObserver
		
		/**
		* This function is called when a control for which this control is the observer 
		* calls CCoeControl::ReportEventL(). In VoiceRecorder button pointer events are
		* handled here.
		* @param aControl The control that sent the event.
		* @param aEventType The event type. 
		*/
		void HandleControlEventL(CCoeControl *aControl, TCoeEvent aEventType);
	
	private:  // from MCoeControlBackground
		
		/*
		* From MCoeControlBackground
		* @see MCoeControlBackground
		*/
		void Draw(CWindowGc& aGc, const CCoeControl& aControl, const TRect& aRect) const;								 

	public:		// new methods
		
		/*
		* Checks which button is currently focused and returns the index
		* value of that
		* @return The index for the currently focused button
		*/
		TInt FocusedButton() const;
	
	private: // from CCoeControl

		/**
		* Draws this control.
		* @param aRect The area that needs updating. Ignored.
		*/
		void Draw( const TRect& aRect ) const;
		
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
		                          	  
	private: // new methods

		/**
		* Moves focus to the next non-dimmed button.
		* @param aDirection EFocusDown to move downwards, EFocusUp upwards
		*/
		void MoveFocus( const TFocusChange aDirection, TBool aDrawNow );

		/**
		* Updates buttons by changing their state
		*/
		void UpdateButtons();
		
		/**
		* Draws highlight rectangle for the focused button.
		* @param aGc Current Graphic Context in use.
		*/
		void DrawFocus( CWindowGc& aGc ) const;
		
		/**
		* Creates and starts a CPeriodic timer object
		* with a resolution of KVRFastForwUpdateDelay
		*/
		void StartTimerL();
		
		/**
		* Stops and deletes the timer object.
		*/
		void StopTimer();
		
		/**
		* Called by the CPeriodic timer to handle a tick.
		* Uses HandleTimerCallBack() to do the actual work.
		* @param aButtonPanel Always a valid pointer to CVRButtonPanel object
		* @return ETrue to indicate that the timer should continue.
		*/	
		static TInt TimerCallBack( TAny* aButtonPanel );
		
		/**
		* Non-static variant of TimerCallBack (more convinient to implement).
		*/
		void HandleTimerCallBack();
		
		/**
		* Search corresponding command of the focused/pressed button and 
		* sends it to active view 
		*/
		void FetchAndSendCommandL();
		
		/**
		* Handles special cases concerning button focus
		*/
		void TuneButtonFocus();
		
		
	private: // data

		/**
		* Pointer to the button panel model. Not owned.
		*/
		MVRButtonPanelModel* iModel;

		/**
		* The index of the currently focused button.
		*/
		TInt iFocusedButton;

		// Skin instance. Not owned.
		MAknsSkinInstance* iSkin;

		// Uid of the parent view
		TUid iParentViewUid;

		/**
		* Pointer to the timer object. Owned.
		*/
		CPeriodic* iTimer;
		
		// Button panel's button controls. Owned.
		CArrayPtrFlat< CAknButton > iVRButtons;
		
		/**
		* Tells what pointer event is the last that has happened
		*/
		TLastPointerEventType iLastPointerEvent;	
		
	};

#endif // __CVRBUTTONPANEL_H__
