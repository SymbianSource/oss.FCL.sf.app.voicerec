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
* Description:  CActive-derived class for synchronizing server commands.
*
*/


// INCLUDE FILES
#include "cnewfileserviceserverobserver.h"
#include "RNewFileServiceClient.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CNewFileServiceServerObserver::CNewFileServiceServerObserver
//
// ---------------------------------------------------------------------------
//
CNewFileServiceServerObserver::CNewFileServiceServerObserver()
    : CActive( EPriorityNormal )
    {
    CActiveScheduler::Add( this );    
    }


// ---------------------------------------------------------------------------
// CNewFileServiceServerObserver::ConstructL
//
// ---------------------------------------------------------------------------
//
void CNewFileServiceServerObserver::ConstructL()
    {

    }


// ---------------------------------------------------------------------------
// CNewFileServiceServerObserver::NewL
//
// ---------------------------------------------------------------------------
//
CNewFileServiceServerObserver* CNewFileServiceServerObserver::NewL()
    {
    CNewFileServiceServerObserver* self = new( ELeave ) CNewFileServiceServerObserver;
    CleanupStack::PushL( self );
    self->ConstructL();    
    CleanupStack::Pop( self );
    return self;
    }



// ---------------------------------------------------------------------------
// CNewFileServiceServerObserver::~CNewFileServiceServerObserver
//
// ---------------------------------------------------------------------------
//
CNewFileServiceServerObserver::~CNewFileServiceServerObserver()
    {
    iObserver = NULL;
RDebug::Print( _L("VoiceRecorder::delete cnewfileserviceobserver") );	    
    Cancel();
    }


// ---------------------------------------------------------------------------
// CNewFileServiceServerObserver::RunL
//
// ---------------------------------------------------------------------------
//
void CNewFileServiceServerObserver::RunL()
    {
    RDebug::Print( _L("VoiceRecorder CNewFileServiceServerObserver::RunL") );
    if ( iObserver )
        {
        iObserver->ServerRequestReady( iStatus.Int() );
        }
    }


// ---------------------------------------------------------------------------
// CNewFileServiceServerObserver::DoCancel
//
// ---------------------------------------------------------------------------
//
void CNewFileServiceServerObserver::DoCancel()
    {
    RDebug::Print( _L("VoiceRecorder CNewFileServiceServerObserver::DoCancel") );            
    if ( iObserver )
        {
        iObserver->ServerRequestReady( KErrCancel );
        }

    }


// ---------------------------------------------------------------------------
// CNewFileServiceServerObserver::IssueWait
//
// ---------------------------------------------------------------------------
//
TRequestStatus& CNewFileServiceServerObserver::IssueWait( 
									MNewFileServiceServerObserver* aObserver )
    {
    iObserver = aObserver;
    
    SetActive();
    RDebug::Print( _L("VoiceRecorder CNewFileServiceServerObserver::IssueWait") );    
    return iStatus;
    }
