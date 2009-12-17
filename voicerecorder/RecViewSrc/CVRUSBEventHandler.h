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
* Description:  An active object class that is used to receive
*  phone call related events from PS API and forward them
*  to an MVRUSBEventObserver instance.
*
*
*/



#ifndef __CVRUSBEVENTHANDLER_H
#define __CVRUSBEVENTHANDLER_H

// INCLUDES
#include <e32base.h>
#include <e32property.h>

// FORWARD DECLARATIONS
class MVRUSBEventObserver;

// CLASS DECLARATION

/**
*  An active object class that is used to receive
*  phone call related events from RUSBAgent and forward them
*  to an MVRUSBEventObserver instance.
*/
NONSHARABLE_CLASS( CVRUSBEventHandler ): public CActive
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
		* @return A pointer to fully constructed instance of CVRUSBEventHandler
        */
        static CVRUSBEventHandler* NewL();

        /**
        * Destructor.
        */
        virtual ~CVRUSBEventHandler();

    public: // New functions
		/**
		* Get the current state of any pubsub key
		* @param aUid The UID of the PS category
		* @param aKey The id identifying the PS key
		* @return The value of the state variable
		*/
		TInt StateL( TUid aUid, TUint aKey );

		/**
		* Start listening to USB events
		* @param aUid The UID identifying the USB agent's state variable
		* @param aObserver The observer who will be notified of events
		*/
		void Listen( TUid aUid, TUint aKey, MVRUSBEventObserver* aObserver );

    public: // from CActive

		/**
		* When this method is called, an event has been received.
		* Notify the observer.
		*/
        void RunL();

		/**
		* Cancel the request for event notifications.
		*/
		void DoCancel();

    private:

		/**
		* Ask USB agent to notify us about events
		*/
		void IssueRequest();

        /**
        * Default constructor.
        */
        CVRUSBEventHandler();

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

    private:    // Data

		/**
		* For observing the USB events
		*/
		RProperty iProperty;

        /**
        * Pointer to the observer. Not owned.
        */
		MVRUSBEventObserver* iObserver;
    };

#endif      // __CVRUSBEVENTHANDLER_H

// End of File
