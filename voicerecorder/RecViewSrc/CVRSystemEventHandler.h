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
*  to an MVRSystemEventObserver instance.
*
*
*/



#ifndef __CVRSYSTEMEVENTHANDLER_H
#define __CVRSYSTEMEVENTHANDLER_H

// INCLUDES
#include <e32base.h>
#include <e32property.h>

// FORWARD DECLARATIONS
class MVRSystemEventObserver;

// CLASS DECLARATION

/**
*  An active object class that is used to receive
*  phone call related events from RSystemAgent and forward them
*  to an MVRSystemEventObserver instance.
*/
NONSHARABLE_CLASS( CVRSystemEventHandler ): public CActive
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
		* @return A pointer to fully constructed instance of CVRSystemEventHandler
        */
        static CVRSystemEventHandler* NewL();

        /**
        * Destructor.
        */
        virtual ~CVRSystemEventHandler();

    public: // New functions
		/**
		* Get the current state of any pubsub key
		* @param aUid The UID of the PS category
		* @param aKey The id identifying the PS key
		* @return The value of the state variable
		*/
		TInt StateL( TUid aUid, TUint aKey );

		/**
		* Start listening to system events
		* @param aUid The UID identifying the system agent's state variable
		* @param aObserver The observer who will be notified of events
		*/
		void Listen( TUid aUid, TUint aKey, MVRSystemEventObserver* aObserver );

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
		* Ask system agent to notify us about events
		*/
		void IssueRequest();

        /**
        * Default constructor.
        */
        CVRSystemEventHandler();

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

    private:    // Data

		/**
		* For observing the system events
		*/
		RProperty iProperty;

        /**
        * Pointer to the observer. Not owned.
        */
		MVRSystemEventObserver* iObserver;
    };

#endif      // __CVRSYSTEMEVENTHANDLER_H

// End of File
