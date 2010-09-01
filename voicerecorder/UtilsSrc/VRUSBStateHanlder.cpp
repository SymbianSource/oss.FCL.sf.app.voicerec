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
*     CVRUSBStateHanlder implementation
*
*
*/


// INCLUDES
#include <e32cmn.h>
#include <usbpersonalityids.h>
#include <e32property.h> 
#include <UsbWatcherInternalPSKeys.h>
#include <usbpersonalityids.h>

#include <AknWaitDialog.h>
#include <eikenv.h> 

#include "VRUSBStateHanlder.h"
#include "CVRRecView.h"

CVRUSBStateHanlder::CVRUSBStateHanlder(MVRUSBStateObserver* aObserver) :
    CActive(EPriorityStandard), // Standard priority
            iObserver(aObserver),
            iConnectionStatus(EStateUninitialized)
    {
    }

EXPORT_C CVRUSBStateHanlder* CVRUSBStateHanlder::NewL(
        MVRUSBStateObserver* aObserver)
    {
    CVRUSBStateHanlder* self = new (ELeave) CVRUSBStateHanlder(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self;
    return self;
    }

void CVRUSBStateHanlder::ConstructL()
    {
#ifdef DUMMY_USB_TESTING 
    User::LeaveIfError(iTimer.CreateLocal()); // Initialize timer
#else
    iProperty.Attach(KPSUidUsbWatcher, KUsbWatcherSelectedPersonality);
#endif
    CActiveScheduler::Add(this); // Add to scheduler    
    StartL();
    }

CVRUSBStateHanlder::~CVRUSBStateHanlder()
    {
    Cancel(); // Cancel any request, if outstanding
#ifdef DUMMY_USB_TESTING   
    iTimer.Close();
#else      
    iProperty.Close();
#endif     
    }

void CVRUSBStateHanlder::DoCancel()
    {
#ifdef DUMMY_USB_TESTING   
    iTimer.Cancel();
#else      
    iProperty.Cancel();
#endif  

    }

void CVRUSBStateHanlder::StartL()
    {
    Cancel(); // Cancel any request, just to be sure
#ifdef DUMMY_USB_TESTING
    iTimer.After(iStatus, 10000000); // Set for later
    SetActive(); // Tell scheduler a request is active   
#else   
    SetActive();
    iProperty.Subscribe(iStatus);
#endif
    }

void CVRUSBStateHanlder::RunL()
    {
#ifdef DUMMY_USB_TESTING
    CDummyUSBState::HandleUSBEventL();
#else
    StartL();
#endif
    
 
    TBool isUsbActive (IsUsbActive());
    
    if (isUsbActive)
        {
        if(iConnectionStatus != EStateConnected)
            {
            iObserver->HandleUsbPlugInL();
            iConnectionStatus = EStateConnected;
            }
        }
    else
        {
        if(iConnectionStatus != EStateDisConnected)
            {
            iObserver->HandleUsbPlugOutL();
            iConnectionStatus = EStateDisConnected;
            } 
        }

#ifdef DUMMY_USB_TESTING    
    //    iStatus = KRequestPending;
    SetActive(); // Tell scheduler a request is active    
    iTimer.After(iStatus, 10000000); // Set for later

    
#endif


    }

TInt CVRUSBStateHanlder::RunError(TInt aError)
    {
    return aError;
    }

EXPORT_C TBool CVRUSBStateHanlder::IsUsbActive()
    {
#ifdef DUMMY_USB_TESTING
    //dummy
    return CDummyUSBState::IsUSBActive();
#else

    TInt usbState;
    TInt err = RProperty::Get(KPSUidUsbWatcher,
            KUsbWatcherSelectedPersonality, usbState);

    if (KErrNone == err && KUsbPersonalityIdMS == usbState)
        {
        return true;
        }
    else
        {
        return false;
        }
#endif
    }

