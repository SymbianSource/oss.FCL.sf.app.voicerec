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
* Description:  Service implementation class
*
*/



// INCLUDE FILES
#include    "RNewFileServiceClient.h"
#include    "NewFileServiceDefs.h"
#include    "cnewfileserviceserverobserver.h"
#include <NewFileServiceClient.h>

#include <s32mem.h>


// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// RNewFileServiceClient::RNewFileServiceClient
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
RNewFileServiceClient::RNewFileServiceClient()
    {
    }


// ---------------------------------------------------------------------------
// RNewFileServiceClient::~RNewFileServiceClient
// Destructor
// ---------------------------------------------------------------------------
//
RNewFileServiceClient::~RNewFileServiceClient()
    {
    delete iObserver;
    
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }

    }


// ---------------------------------------------------------------------------
// RNewFileServiceClient::ServiceUid
// 
// ---------------------------------------------------------------------------
//
TUid RNewFileServiceClient::ServiceUid() const
	{
	return TUid::Uid( NEWSERVICEUID );

	}


// ---------------------------------------------------------------------------
// RNewFileServiceClient::NewFileL
// 
// ---------------------------------------------------------------------------
//
TBool RNewFileServiceClient::NewFileL( RFile& aFile, TDesC8* aGenericParams,
													TNewServiceFileType aType )
	{
	TIpcArgs args;

	TNewServiceParamPack pack;
	pack.iType = aType;
	pack.iMultipleFiles = EFalse;

	TPckgBuf< TNewServiceParamPack > paramPack;
	paramPack = pack;
	args.Set( 0, &paramPack );


	if ( aGenericParams )
	{
		args.Set( ENewServiceIndexFileName, aGenericParams );
	}

	TInt err = aFile.TransferToServer( args, ENewServiceIndexFsHandle,
	                                   ENewServiceIndexFileHandle );
#ifdef _DEBUG	
	RDebug::Print( _L("transfer to server error: %d"), err );
#endif	

    iObserver = CNewFileServiceServerObserver::NewL();

  	// Send through IPC, pass iObservers TRequestStatus to server
	SendReceive( ENewFileToHandle, args, iObserver->IssueWait( this ) );

    // Wait until server notifies iObserver
    if ( !iWait.IsStarted() )
        {
        iWait.Start(); // CSI: 10 #
        }

	return paramPack().iReturnValue;
	}


// ---------------------------------------------------------------------------
// RNewFileServiceClient::NewFileL
// 
// ---------------------------------------------------------------------------
//
TBool RNewFileServiceClient::NewFileL( CDesCArray& aFilenames, 
									   TDesC8* aGenericParams,
									   TNewServiceFileType aType, 
									   TBool aMultipleFiles )
	{
#ifdef _DEBUG	
	RDebug::Print( _L("RNewFileServiceClient::NewFileL") );
#endif

	TIpcArgs args;

	TNewServiceParamPack pack;
	pack.iType = aType;
	pack.iMultipleFiles = aMultipleFiles;

	TPckgBuf< TNewServiceParamPack > paramPack = pack;
	args.Set( ENewServiceIndexParams, &paramPack );

	if ( aGenericParams )
	{
		args.Set( ENewServiceIndexFileName, aGenericParams );
	}
	else
	{
		args.Set( ENewServiceIndexFileName, &KNullDesC() );
	}

    iObserver = CNewFileServiceServerObserver::NewL();

	// Send through IPC, pass iObservers TRequestStatus to server
	SendReceive( ECreateNewFile, args, iObserver->IssueWait( this )  );

    // Wait until server notifies iObserver
    if ( !iWait.IsStarted() )
        {
        iWait.Start(); // CSI: 10 #
        }

	pack = paramPack();

	if ( !pack.iReturnValue )
		{
#ifdef _DEBUG
		RDebug::Print( _L("RNewFileServiceClient::NewFileL - return value received: False") );
#endif	
		// don't bother with filename array if the user canceled	
		return EFalse;		
		}

#ifdef _DEBUG
	RDebug::Print( _L("RNewFileServiceClient::NewFileL - return value received: True") );
#endif	

	TPckgBuf< TInt > lengthBuf;
	args.Set( ENewServiceIndexParams, &lengthBuf );
	User::LeaveIfError( SendReceive( EGetFilenameArrayLength, args ) );

	TInt length( lengthBuf() );

	HBufC8* buffer = HBufC8::NewLC( length );
	TPtr8 ptr = buffer->Des();
	args.Set( ENewServiceIndexParams, &ptr );
	User::LeaveIfError( SendReceive( EGetFilenameArray, args ) );

	// Unpack filename array
	RDesReadStream stream;
	stream.Open( ptr );

	TInt count( stream.ReadInt32L() );

	for ( TInt i( 0 ); i < count; i++ )
		{
		TInt length( stream.ReadInt16L() );
		TFileName filename;
		stream.ReadL( filename, length );
#ifdef _DEBUG
		RDebug::Print( filename );
#endif		
		aFilenames.AppendL( filename );
		}

	CleanupStack::PopAndDestroy(); // buffer


	return ETrue;
	}


// ----------------------------------------------------------------------------
// RNewFileServiceClient::ServerRequestReady
// 
// ----------------------------------------------------------------------------
//
void RNewFileServiceClient::ServerRequestReady( TInt aError )
    {

    if (aError == KErrAbort)
    	{
    		iError = KErrAbort;
    	}
    // Server processed command and active object was notified and
    // client can continue now
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    }


void RNewFileServiceClient::SetFileServiceClient(CNewFileServiceClient* aClient)
	{
	iNewFileServiceClient = aClient;	
	}


//  End of File
