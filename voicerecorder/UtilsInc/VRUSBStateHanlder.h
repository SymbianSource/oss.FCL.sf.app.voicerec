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
*     CVRUSBStateHanlder declaration
*     
*
*/


#ifndef VRUSBSTATEHANLDER_H
#define VRUSBSTATEHANLDER_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>		// For RTimer, link against: euser.lib
#include <e32property.h> 

//#include "DummyUSBState.h" // for dummy testing

class MVRUSBStateObserver
    {
public:
    // Constructors and destructor

    virtual TInt HandleUsbPlugInL() = 0;
    virtual TInt HandleUsbPlugOutL() = 0;

    };

class CVRUSBStateHanlder : public CActive
    {
public:
    // Cancel and destroy
    ~CVRUSBStateHanlder();

    IMPORT_C static CVRUSBStateHanlder* NewL(MVRUSBStateObserver* aObserver);

public:

    IMPORT_C static TBool IsUsbActive();

private:
    // C++ constructor
    CVRUSBStateHanlder(MVRUSBStateObserver* aObserver);

    // Second-phase constructor
    void ConstructL();

private:
    // From CActive
    // Handle completion
    void RunL();

    // How to cancel me
    void DoCancel();

    // Override to handle leaves from RunL(). Default implementation causes
    // the active scheduler to panic.
    TInt RunError(TInt aError);

    void StartL();

private:
#ifdef DUMMY_USB_TESTING
    RTimer iTimer; // Provides async timing service, for dummy testing
    TInt isUSBActive;
#else
    RProperty iProperty;
#endif
    MVRUSBStateObserver* iObserver;    
    enum TUSBConnectionStatus 
        { 
        EStateUninitialized, 
        EStateConnected, 
        EStateDisConnected
        }; 
    TUSBConnectionStatus iConnectionStatus;
    };

#endif // VRUSBSTATEHANLDER_H
