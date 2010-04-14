/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*     This class is a part of the standard application framework.
*     It instantiates the application views.
*     It also acts as the default command handler for the application.
*
*/


#ifndef __CVRAPPUI_H__
#define __CVRAPPUI_H__

// INCLUDES
#include <aknViewAppUi.h>
#include <apparc.h>			// MApaEmbeddedDocObserver

#include "MVREmbeddedObserver.h"

// FORWARD DECLARATIONS
class MVRSelectionProvider;
class CErrorUI;
class MVRLayoutChangeObserver;

// CLASS DECLARATION
/**
* This class is a part of the standard application framework.
* It instantiates the application views.
* It also acts as the default command handler for the application.
*/
class CVRAppUi
	: public CAknViewAppUi
	{
	public: // Constructors and destructor

		/**
        * Default constructor.
        */
		CVRAppUi();

		/**
		* Destructor.
		*/
		~CVRAppUi();

		/**
        * 2nd phase constructor.
        */
		void ConstructL();

		/**
		* Receive file handle and observer for recording
		* for file new service
		* @param aFile File handle
		* @param aObserver Observer for handing events back to file service
		*/
		void RecordNewFileL( RFile& aFile, MVREmbeddedObserver* aObserver );

		/**
		* Method for setting observer for file new service
		* @param aObserver Observer for handing events back to file service
		*/
		void SetEmbeddedObserver( MVREmbeddedObserver* aObserver );

	private: // from CEikAppUi

		/**
		* Takes care of command handling.
		* @param aCommand Command to be handled
		*/
		void HandleCommandL( TInt aCommand );

		/**
		* Handles a change to the application's resources
		* @param aType The type of changed resource
		*/
		void HandleResourceChangeL( TInt aType );

		/**
		* CEikAppUi::ProcessCommandParametersL() is overwritten here, because
		* otherwise it messes with the document name.
		* @param aCommand The shell command sent to the application
		* @param aDocumentName The document name that will be given to OpenFileL()
		* @param aTail The rest of the command line
		* @return Whether the final document name represents an existing file
		*/
		TBool ProcessCommandParametersL( TApaCommand aCommand,
		    TFileName& aDocumentName, const TDesC8& aTail );

		/**
		* Handles events from window group  
		* @param aType The type of event
		*/
		
		void HandleApplicationSpecificEventL(TInt aType, const TWsEvent& aEvent);
		
	private: // new methods

		/**
		* Provides access to the current view's MVRSelectionProvider
		* interface.
		* @return pointer to the active view as MVRSelectionProvider.
		*/
		MVRSelectionProvider* SelectionProviderL();

		/**
		* Check USB connection state
		* interface.
		* @return True/False
		*/        
        TBool IsUsbActive();

	private: // data
		/**
		* CActiveScheduler wrapper object. Owned.
		*/
		CActiveSchedulerWait iActiveWait;

		/**
		* Owned: CErrorUI object used to display notes when errors occur
		* in embedded activation
		*/
		CErrorUI* iErrorUI;

		/**
		* Pointer to object that observers layout changes and forwards
		* notification to all UI controls
		*/
		MVRLayoutChangeObserver* iLayoutChangeObserver;
		
		/**
		* Pointer to embedded observer which is notified (if not null) when
		* embedded recording is ready
		*/
		MVREmbeddedObserver* iEmbeddedObserver;

		/*
		* For help context, whether settings page is active.
		*/
		TBool iIsSettingsOpen;

	};

#endif // __CVRAPPUI_H__
