/*
* Copyright (c) 2005 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*  phone call related events from Publish&Subscribe and forward them
*  to an MVRUSBEventObserver instance.
*  Has to be attached to certain P&S -key, so it means having multiple
*  
*
*
*/


// INCLUDE FILES
#include <e32base.h>

#include "CVRUSBEventHandler.h"
#include "MVRUSBEventObserver.h"


// ================= MEMBER FUNCTIONS ========================================

// ---------------------------------------------------------------------------
// CVRUSBEventHandler::CVRUSBEventHandler
// 
// ---------------------------------------------------------------------------
//
CVRUSBEventHandler::CVRUSBEventHandler() : CActive( EPriorityHigh )
    {
	CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------------------------
// CVRUSBEventHandler::ConstructL
// 
// ---------------------------------------------------------------------------
//
void CVRUSBEventHandler::ConstructL()
    {
	
    }


// ---------------------------------------------------------------------------
// CVRUSBEventHandler::NewL
// 
// ---------------------------------------------------------------------------
//
// Two-phased constructor.
CVRUSBEventHandler* CVRUSBEventHandler::NewL()
    {
    CVRUSBEventHandler* self = new (ELeave) CVRUSBEventHandler;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// ---------------------------------------------------------------------------
// CVRUSBEventHandler::~CVRUSBEventHandler
// 
// ---------------------------------------------------------------------------
//
// Destructor
CVRUSBEventHandler::~CVRUSBEventHandler()
    {
	Cancel();
    }


// ---------------------------------------------------------------------------
// CVRUSBEventHandler::IssueRequest
// 
// ---------------------------------------------------------------------------
//
void CVRUSBEventHandler::IssueRequest()
	{
	SetActive();
	iProperty.Subscribe( iStatus );
	}


// ---------------------------------------------------------------------------
// CVRUSBEventHandler::RunL
// 
// ---------------------------------------------------------------------------
//
void CVRUSBEventHandler::RunL()
	{
	// Event received

	if ( iStatus != KErrCancel )
		{
		// Request a new event
		IssueRequest();
		// Notify the observer
		iObserver->HandleUSBEventL();
		}
	}


// ---------------------------------------------------------------------------
// CVRUSBEventHandler::Listen
// 
// ---------------------------------------------------------------------------
//
void CVRUSBEventHandler::Listen( TUid aUid, 
									TUint aKey, 
									MVRUSBEventObserver* aObserver )
	{
	iProperty.Attach( aUid, aKey );
	
	iObserver = aObserver;
	IssueRequest();
	}


// ---------------------------------------------------------------------------
// CVRUSBEventHandler::DoCancel
// 
// ---------------------------------------------------------------------------
//
void CVRUSBEventHandler::DoCancel()
	{
	iProperty.Cancel();	
	}


// ---------------------------------------------------------------------------
// CVRUSBEventHandler::StateL
// 
// ---------------------------------------------------------------------------
//
TInt CVRUSBEventHandler::StateL( TUid aUid, TUint aKey )
	{
	TInt value;
	User::LeaveIfError( RProperty::Get( aUid, aKey, value ) );
	return value;
	}
	
//  End of File
