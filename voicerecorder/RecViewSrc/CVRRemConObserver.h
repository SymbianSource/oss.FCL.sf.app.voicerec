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
* Description:  Class that observer remote controller framework events
*                and forwards them as key events to observe
*
*/


#ifndef __CVRREMCONOBSERVER_H
#define __CVRREMCONOBSERVER_H

//  INCLUDES
#include <e32def.h>
#include <remconcoreapitargetobserver.h>

// FORWARD DECLARATIONS
class MVRVolumeEventObserver;
class CRemConInterfaceSelector;  
class CRemConCoreApiTarget;

// CLASS DECLARATION

/**
*  Class for monitoring side volume key events.
*  Is also responsible for creating repeating keypresses if key is held down
*
*/
NONSHARABLE_CLASS( CVRRemConObserver )
	: public CBase,	public MRemConCoreApiTargetObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CVRRemConObserver* NewL( MVRVolumeEventObserver* aObserver );
        
        /**
        * Destructor.
        */
        virtual ~CVRRemConObserver();

    protected: // Functions from base classes

        /**
        * @see MRemConCoreApiTargetObserver.
        *
        * A command has been received. 
        * @param aOperationId The operation ID of the command.
        * @param aButtonAct The button action associated with the command.
        */
        void MrccatoCommand( TRemConCoreApiOperationId aOperationId, 
                             TRemConCoreApiButtonAction aButtonAct);

    private:

        /**
        * C++ default constructor.
        */
        CVRRemConObserver( MVRVolumeEventObserver* aObserver );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * Catch timer event. Volume command is repeated
        @param aAny
        */
        static TInt DoHandleVolumeRepeatL( TAny* aAny );        

    private:    // Data
    	/** Observer to notify about a event. Not owned. **/
		MVRVolumeEventObserver* iObserver;
		
		/** Remote Controller. Owned */
        CRemConInterfaceSelector* iInterfaceSelector;  
        
        /** Remote Controller. Not owned. */
        CRemConCoreApiTarget* iCoreTarget;

        /** Timer for counting key press. Owned. */
        CPeriodic *iVolumeRepeatTimer;
         
        /** Status of volume command */
        TBool iRemConVolumeCommandOk;
        
        /** Volume change that is repeated */
        TKeyEvent iVolumeChange;
    };

#endif      // CVRRemConObserver_H   
            
// End of File
