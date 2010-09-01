/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Monitors for media removal events
*
*/


#ifndef CVRMEDIAREMOVALMONITER_H
#define CVRMEDIAREMOVALMONITER_H

#include <e32base.h>
#include "MVRMediaRemovalObserver.h"

/**
 *  Class to monitors for File System dismount events
 *  @lib Harvester
 *  @since S60 3.0
 */
NONSHARABLE_CLASS( CVRMediaRemovalMonitor ) : public CActive
    {

public:

    /**
    * Two-phase constructor
    * @param aDrive drive to monitor
    * @param aFs file server session
    * @param aObserver observer to the event
    */
    static CVRMediaRemovalMonitor* NewL(  TInt aDrive, 
                                          RFs& aFs,
                                          MVRMediaRemovalObserver* aObserver );
    /**
    * Two-phase constructor
    * @param aDrive drive to monitor
    * @param aFs file server session
    * @param aObserver observer to the event
    */
    static CVRMediaRemovalMonitor* NewLC(  TInt aDrive, 
                                           RFs& aFs,
                                           MVRMediaRemovalObserver* aObserver );

    /**
    * Virtual destructor
    */
    virtual ~CVRMediaRemovalMonitor();

protected: // From base class 
    
    /*
    * From CActive
    */
    void RunL();
    
    /*
    * From CActive
    */
    void DoCancel();

    /**
    *  From CActive
    */
    TInt RunError(TInt aError);    
        
private:

    /**
    *  C++ constructor
    * @param aDrive drive to monitor
    * @param aFs file server session
    * @param aObserver observer to the event
    */
    CVRMediaRemovalMonitor(  TInt aDrive,
                             RFs& aFs,  
                             MVRMediaRemovalObserver* aObserver );

    void ConstructL();

private: // data
    
    TInt iDrive;        // Drive that is being monitored
    RFs& iFs;           // File Session, not owned
    TBool iDiskRemoved; // Is the disk inserted
    
    /*
    * Observer interface to callback to an observer
    */
    MVRMediaRemovalObserver* iObserver;
    };

#endif // CVRMEDIAREMOVALMONITER_H
