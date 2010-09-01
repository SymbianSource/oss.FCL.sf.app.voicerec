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
*     An empty control that is displayed until application is fully
*	  constructed
*
*/


#ifndef CVRRECVIEWACTIVATIONCONTAINER_H
#define CVRRECVIEWACTIVATIONCONTAINER_H

// INCLUDES
#include <bldvariant.hrh>
#include <coecntrl.h>
#include "MVRObserver.h"
#include <remconcoreapitargetobserver.h>
#include <remconinterfaceselector.h>
#include <remconcoreapitarget.h>
#include <AknUtils.h>

// FORWARD DECLARATIONS
class CVRButtonPanel;
class CVRStateInfoPanel;
class MVRButtonPanelModel;
class MVRDecoratorModel;
class MVRStateInfoModel;
class MVRKeyObserver;
class MVRVolumeChangeObserver;
class CVRRecView;
class CAknNavigationDecorator;
class CAknNavigationControlContainer;
class MAknsSkinInstance;
class CAknsBasicBackgroundControlContext;


// CLASS DEFINITION
/**
* An empty control that is displayed until application is fully
* constructed
*/
NONSHARABLE_CLASS( CVRRecViewActivationContainer )
	: public CCoeControl
	{
	public:	// Constructors and destructor

		/**
		* Default constructor.
		*/
		CVRRecViewActivationContainer();

		/**
		* Destructor
		*/
		~CVRRecViewActivationContainer();

		/**
		* 2nd phase constructor
		* @param aRect The screen rectangle for this component
		*/
		void ConstructL( const TRect& aRect );
	public: // from CCoeControl
		/*
		*
		*/
		void HandleResourceChange(TInt aType); 

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

		/**
		* Handles a change to the application's resources
		* @param aType The type of changed resource
		*/
		void HandleResourceChangeL( TInt aType );

	private: // data
	    /**
	    * Skin instance from AknsUtils. Not owned.
	    */
		MAknsSkinInstance* iSkinInstance;

        /**
        * Background context for clearing screen. Owned.
        */
		CAknsBasicBackgroundControlContext* iBackgroundSkinContext;	
	};

#endif // CVRRECVIEWACTIVATIONCONTAINER_H
