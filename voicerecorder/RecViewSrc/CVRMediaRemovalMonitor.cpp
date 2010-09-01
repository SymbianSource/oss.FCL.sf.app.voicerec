/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Monitors for Media removal
*
*/


#include <e32base.h>
#include <f32file.h>
#include "CVRMediaRemovalMonitor.h"

// ---------------------------------------------------------------------------
// C++ Constructor
// ---------------------------------------------------------------------------
//
CVRMediaRemovalMonitor::CVRMediaRemovalMonitor
                  ( TInt aDrive, RFs& aFs, MVRMediaRemovalObserver* aObserver ) 
                                                       : CActive(EPriorityHigh),
                                                         iDrive( aDrive ),
                                                         iFs( aFs ),
                                                         iDiskRemoved( EFalse ),
                                                         iObserver( aObserver )
                                                         
    {
    CActiveScheduler::Add(this);
    }


// ---------------------------------------------------------------------------
// 2nd Phase Constructor
// ---------------------------------------------------------------------------
//
void CVRMediaRemovalMonitor::ConstructL()
    {
 
    // Initial state
    TDriveInfo drive;
  	User::LeaveIfError(iFs.Drive(drive, TInt(iDrive)));
   	iDiskRemoved = (drive.iType == EMediaNotPresent);

    // Start listening
    TNotifyType notType(ENotifyDisk);
    iFs.NotifyChange( notType, iStatus );
    SetActive();
    }


// ---------------------------------------------------------------------------
// Two-Phased Constructor
// ---------------------------------------------------------------------------
//
CVRMediaRemovalMonitor* CVRMediaRemovalMonitor::NewL
                ( TInt aDrive, RFs& aFs, MVRMediaRemovalObserver* aObserver )
    {
    CVRMediaRemovalMonitor* self
                               = CVRMediaRemovalMonitor::NewLC( aDrive,
                                                                 aFs,
                                                                 aObserver );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Two-Phased Constructor
// ---------------------------------------------------------------------------
//
CVRMediaRemovalMonitor* CVRMediaRemovalMonitor::NewLC
               ( TInt aDrive, RFs& aFs, MVRMediaRemovalObserver* aObserver )
    {
    CVRMediaRemovalMonitor* self = 
                          new( ELeave ) CVRMediaRemovalMonitor( aDrive,
                                                                 aFs,
                                                                 aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CVRMediaRemovalMonitor::~CVRMediaRemovalMonitor()
    {
    Cancel();
    }

    
// ---------------------------------------------------------------------------
// Service the request
// ---------------------------------------------------------------------------
//
void CVRMediaRemovalMonitor::RunL()
    {
    // Re-subscribe to event.
    TNotifyType notType(ENotifyDisk);
    iFs.NotifyChange( notType, iStatus );
    SetActive();
    
    // Check state
    TDriveInfo drive;
	User::LeaveIfError(iFs.Drive(drive, TInt(iDrive)));
	
	  // Notify Observer
    switch(drive.iType)
        {
        case EMediaNotPresent:
            {
            if (!iDiskRemoved)
                {
                iObserver->HandleMMCEjectEventL( );
                }
                iDiskRemoved = ETrue;
            break;
            }
        default:
            {
            if ( iDiskRemoved &&
        		 ( drive.iMediaAtt & ( KMediaAttLockable|KMediaAttLocked|KMediaAttHasPassword ) ) != 
 				 ( KMediaAttLockable|KMediaAttLocked|KMediaAttHasPassword ) ) 
                {
                //do nothing
                iDiskRemoved = EFalse;
                }
            break;
            }
        }
    
    }
    
// ---------------------------------------------------------------------------
// Cancel NotifyChange request from file system
// ---------------------------------------------------------------------------
//
void CVRMediaRemovalMonitor::DoCancel()
    {
    iFs.NotifyChangeCancel();
    }
    
// ----------------------------------------------------------------------------
// Handles a leave occurring in the request completion event handler RunL()
// Don't care if client has a User::Leave() in RunL(), keep monitoring for events
// ----------------------------------------------------------------------------
//
TInt CVRMediaRemovalMonitor::RunError(TInt aError)
    {
    return KErrNone;
    }           
