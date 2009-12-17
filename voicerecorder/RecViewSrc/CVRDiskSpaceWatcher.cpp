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
*       Observe the Disk Space
*
*/


// INCLUDE FILES
#include "CVRMdaRecorder.h"
#include "CVRDiskSpaceWatcher.h" 
#include "CVRMemo.h"



// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------------------------
// CVRDiskSpaceWatcher::CVRDiskSpaceWatcher
// 
// ---------------------------------------------------------------------------
//
inline CVRDiskSpaceWatcher::CVRDiskSpaceWatcher
        (CVRMdaRecorder& aRecorder, RFs& aFs) :
    CActive(CActive::EPriorityIdle),
    iRecorder(aRecorder), iFs(aFs)
    {
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------------------------
//CVRDiskSpaceWatcher::NewL
// 
// ---------------------------------------------------------------------------
//
CVRDiskSpaceWatcher* CVRDiskSpaceWatcher::NewL
        (CVRMdaRecorder& aRecorder, RFs& aFs)
    {
    CVRDiskSpaceWatcher* self = new(ELeave) CVRDiskSpaceWatcher(aRecorder, aFs);
    return self;
    }

// ---------------------------------------------------------------------------
// CVRDiskSpaceWatcher::~CVRDiskSpaceWatcher
// 
// ---------------------------------------------------------------------------
//
CVRDiskSpaceWatcher::~CVRDiskSpaceWatcher()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// CVRDiskSpaceWatcher::DoCancel
// 
// ---------------------------------------------------------------------------
//
void CVRDiskSpaceWatcher::DoCancel()
    {
    iFs.NotifyDiskSpaceCancel();
    }

// ---------------------------------------------------------------------------
// CVRDiskSpaceWatcher::RunL()
// 
// ---------------------------------------------------------------------------
//
void CVRDiskSpaceWatcher::RunL()
    {
    // if error returns, restart AO
    if (iStatus.Int())
    	{
   	    if (iRecorder.iMemo->Quality() == EQualityHigh)
   	    	{
    	    RequestNotification( iThreshold, iDrive);	
   	    	}
    	return;	
    	}

    // Disk space is below CL -> initiate a compress
    if(iRecorder.iAudioRecorder->State() == CMdaAudioClipUtility::ERecording)
    	{
        iRecorder.StopAndNotify();	
    	}
 
    // Renew notification request
    if (iRecorder.iMemo)
    	{
    	if (iRecorder.iMemo->Quality() == EQualityHigh)
    		{
   	    	RequestNotification( iThreshold, iDrive);	
    		}    		
    	}

    }

// ---------------------------------------------------------------------------
// CVRDiskSpaceWatcher::RunError
// 
// ---------------------------------------------------------------------------
//
TInt CVRDiskSpaceWatcher::RunError(TInt /*aError*/)
    {
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CVRDiskSpaceWatcher::RequestNotification
// 
// ---------------------------------------------------------------------------
//
void CVRDiskSpaceWatcher::RequestNotification( TInt64 aThreshold, TInt aDrive)
    {
    iThreshold = aThreshold;
    iDrive = aDrive;
    Cancel();
    iFs.NotifyDiskSpace(iThreshold,iDrive,iStatus);
    SetActive();
    }


//  End of File  
