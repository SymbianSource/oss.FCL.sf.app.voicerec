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
*     This class is part of new file service framework.
*
*/


#include "CVRNewFileService.h"
#include "CVRAppUi.h"
#include <f32file.h>
#include <eikenv.h>
#include <eikappui.h>
#include <eikapp.h>

#include "voicerecorder.hrh"

CVRNewFileService* CVRNewFileService::NewL()
	{
	return new( ELeave ) CVRNewFileService;
	}

CVRNewFileService::CVRNewFileService()
: iCompleted( EFalse ), iFileHandleUsed( EFalse )
	{
		
	}

CVRNewFileService::~CVRNewFileService()
	{
	// Make sure that appui wont use this deleted class
	CVRAppUi* appUi( static_cast<CVRAppUi*> (CEikonEnv::Static()
	                ->EikAppUi() ) );
    if ( appUi )
        {
        appUi->SetEmbeddedObserver( NULL );
        }
          
	if ( !iCompleted)
		{
		if ( iObserver )
			{
			// Can't leave in destructor
			TRAP_IGNORE( iObserver->HandleCompletedNewServiceL( EFalse ) );				
			iCompleted = ETrue;
			}
		}
	}
	
void CVRNewFileService::HandleNewFileL( MNewFileServiceObserver* aObserver,
							  CDesCArray& aFilenameArray,
							  TNewServiceFileType aType,
							  TBool /*aMultipleFiles*/ )
	{
	RFile dummyFile;

	iFilenameArray = &aFilenameArray;
	iObserver = aObserver;

	if ( aType != ENewFileServiceAudio )
		{
		iCompleted = ETrue;
		aObserver->HandleCompletedNewServiceL( EFalse );

		// Send NULL to AppUi as observer, AppUi exits

		( static_cast<CVRAppUi*> (CEikonEnv::Static()->EikAppUi() ) )
			->RecordNewFileL( dummyFile, NULL );		
		return;
		}

	// Send message to AppUi to fully construct application
	( static_cast<CVRAppUi*> (CEikonEnv::Static()->EikAppUi() ) )
		->RecordNewFileL( dummyFile, this );

	}

void CVRNewFileService::HandleNewFileL( MNewFileServiceObserver* aObserver,
							  RFile& aFile,
							  TNewServiceFileType aType )
	{
	iObserver = aObserver;

	if ( aType != ENewFileServiceAudio )
		{
		iCompleted = ETrue;
		aObserver->HandleCompletedNewServiceL( EFalse );

		// Send NULL to AppUi as observer, AppUi exits
		RFile dummyFile;
		( static_cast<CVRAppUi*> (CEikonEnv::Static()->EikAppUi() ) )
			->RecordNewFileL( dummyFile, NULL );		
		return;
		}

	iFileHandleUsed = ETrue;

	// Send the file handle to application and finish ui construction
	( static_cast<CVRAppUi*> (CEikonEnv::Static()->EikAppUi() ) )
	                        ->RecordNewFileL( aFile, this );
	}

TBool CVRNewFileService::FileCompleteL( const TDesC& aName )
	{
	if ( iFilenameArray )
		{
		iFilenameArray->AppendL( aName );
		}

	iCompleted = ETrue;
	iObserver->HandleCompletedNewServiceL( ETrue );	

	// Notify the appui
	if ( iFileHandleUsed )
		{
		// Application can be closed immediately
		return ETrue;
		}
	
	// Application waits for ECmdNewFileServiceNotify before it quits
	// File service needs to transfer filenames back to client first
	return EFalse;
	}

void CVRNewFileService::ServiceCompleteL()
	{
	CEikonEnv::Static()->EikAppUi()->HandleCommandL( ECmdNewFileServiceNotify );	
	}

void CVRNewFileService::AbortL()
	{
	if ( !iCompleted )
		{
		iCompleted = ETrue;
		iObserver->HandleCompletedNewServiceL( EFalse );
		}
	}

// End of File
