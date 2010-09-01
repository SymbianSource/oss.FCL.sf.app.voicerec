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
*  to an MVRSystemEventObserver instance.
*  Has to be attached to certain P&S -key, so it means having multiple
*  
*
*
*/


// INCLUDE FILES
#include <e32base.h>

#include "CVRSystemEventHandler.h"
#include "MVRSystemEventObserver.h"


// ================= MEMBER FUNCTIONS ========================================

// ---------------------------------------------------------------------------
// CVRSystemEventHandler::CVRSystemEventHandler
// 
// ---------------------------------------------------------------------------
//
CVRSystemEventHandler::CVRSystemEventHandler() : CActive( EPriorityLow )
    {
	CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------------------------
// CVRSystemEventHandler::ConstructL
// 
// ---------------------------------------------------------------------------
//
void CVRSystemEventHandler::ConstructL()
    {
	
    }


// ---------------------------------------------------------------------------
// CVRSystemEventHandler::NewL
// 
// ---------------------------------------------------------------------------
//
// Two-phased constructor.
CVRSystemEventHandler* CVRSystemEventHandler::NewL()
    {
    CVRSystemEventHandler* self = new (ELeave) CVRSystemEventHandler;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// ---------------------------------------------------------------------------
// CVRSystemEventHandler::~CVRSystemEventHandler
// 
// ---------------------------------------------------------------------------
//
// Destructor
CVRSystemEventHandler::~CVRSystemEventHandler()
    {
	Cancel();
    }


// ---------------------------------------------------------------------------
// CVRSystemEventHandler::IssueRequest
// 
// ---------------------------------------------------------------------------
//
void CVRSystemEventHandler::IssueRequest()
	{
	SetActive();
	iProperty.Subscribe( iStatus );
	}


// ---------------------------------------------------------------------------
// CVRSystemEventHandler::RunL
// 
// ---------------------------------------------------------------------------
//
void CVRSystemEventHandler::RunL()
	{
	// Event received

	if ( iStatus != KErrCancel )
		{
		// Request a new event
		IssueRequest();
		// Notify the observer
		iObserver->HandleSystemEventL();
		}
	}


// ---------------------------------------------------------------------------
// CVRSystemEventHandler::Listen
// 
// ---------------------------------------------------------------------------
//
void CVRSystemEventHandler::Listen( TUid aUid, 
									TUint aKey, 
									MVRSystemEventObserver* aObserver )
	{
	iProperty.Attach( aUid, aKey );
	
	iObserver = aObserver;
	IssueRequest();
	}


// ---------------------------------------------------------------------------
// CVRSystemEventHandler::DoCancel
// 
// ---------------------------------------------------------------------------
//
void CVRSystemEventHandler::DoCancel()
	{
	iProperty.Cancel();	
	}


// ---------------------------------------------------------------------------
// CVRSystemEventHandler::StateL
// 
// ---------------------------------------------------------------------------
//
TInt CVRSystemEventHandler::StateL( TUid aUid, TUint aKey )
	{
	TInt value;
	User::LeaveIfError( RProperty::Get( aUid, aKey, value ) );
	return value;
	}
	
//  End of File
