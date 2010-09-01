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
* Description:  Implementation for Client side API
*
*/


// INCLUDE FILES

#include <s32mem.h>
#include <e32cmn.h>
#include <AiwGenericParam.h>

#include "NewFileServiceClient.h"
#include "RNewFileServiceClient.h"
#include "CNewFileServiceClientImpl.h"

#include <VoiceRecorderUID.h>
#include <CcorUiConstants.h>
#include <avkon.hrh>


// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// NewFileServiceFactory::NewClientL
//
// ----------------------------------------------------------------------------
//
EXPORT_C CNewFileServiceClient* NewFileServiceFactory::NewClientL()
	{
	return CNewFileServiceClientImpl::NewL();
	}


// ----------------------------------------------------------------------------
// CNewFileServiceClientImpl::NewL
//
// ----------------------------------------------------------------------------
//
CNewFileServiceClientImpl* CNewFileServiceClientImpl::NewL()
	{
	CNewFileServiceClientImpl* self =
					new( ELeave )CNewFileServiceClientImpl;
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}


// ----------------------------------------------------------------------------
// CNewFileServiceClientImpl::~CNewFileServiceClientImpl
//
// ----------------------------------------------------------------------------
//
CNewFileServiceClientImpl::~CNewFileServiceClientImpl()
	{
	delete iServerAppMonitor;
	iClient.Close();
	if(iClient.iError == KErrAbort)
		{
		iClient.iError = 0;
		MAknServerAppExitObserver::HandleServerAppExit(EAknCmdExit);	
		}
	}


// ----------------------------------------------------------------------------
// CNewFileServiceClientImpl::ConstructL
//
// ----------------------------------------------------------------------------
//
void CNewFileServiceClientImpl::ConstructL()
	{
	iClient.SetFileServiceClient(this);
	}


// ----------------------------------------------------------------------------
// CNewFileServiceClientImpl::NewFileL
//
// ----------------------------------------------------------------------------
//
TBool CNewFileServiceClientImpl::NewFileL( CDesCArray& aFileNames,
							   CAiwGenericParamList* aParams,
							   TNewServiceFileType aFileType,
							   TBool aMultipleFiles )
	{
	TUid uid( KNullUid );

	switch ( aFileType )
		{
		case ENewFileServiceAudio:
			{
			uid = TUid::Uid( KVoiceRecorderAppUID3 );
			break;
			}

		case ENewFileServiceVideo:
		case ENewFileServiceImage:
			{
			uid = KUidCamcorder;
			break;
			}
		default:
		    {
			User::Leave( KErrNotSupported );
		    }
		};

	return NewFileL( uid, aFileNames, aParams, aFileType, aMultipleFiles );
	}


// ----------------------------------------------------------------------------
// CNewFileServiceClientImpl::HandleServerAppExit
//
// ----------------------------------------------------------------------------
//
void CNewFileServiceClientImpl::HandleServerAppExit(TInt aReason)
    {
#ifdef _DEBUG    
    RDebug::Print( _L("CNewFileServiceClientImpl::HandleServerAppExit") );
#endif
    iClient.Close();
    MAknServerAppExitObserver::HandleServerAppExit(aReason);
    }


// ----------------------------------------------------------------------------
// CNewFileServiceClientImpl::NewFileL
//
// ----------------------------------------------------------------------------
//
TBool CNewFileServiceClientImpl::NewFileL( TUid aApplicationUid,
							   CDesCArray& aFileNames,
							   CAiwGenericParamList* aParams,
							   TNewServiceFileType aFileType,
							   TBool aMultipleFiles )
	{
	iClient.ConnectChainedAppL( aApplicationUid );

	if ( !iServerAppMonitor )
		{
		iServerAppMonitor = CApaServerAppExitMonitor::NewL( iClient, *this,
												CActive::EPriorityStandard );
		}

	HBufC8* serializedParams = SerializeGenericParamListLC( aParams );
	TBool ret = iClient.NewFileL( aFileNames, serializedParams, 
								  aFileType, aMultipleFiles );
	CleanupStack::PopAndDestroy();	// serializedParams
	return ret;
	}


// ----------------------------------------------------------------------------
// CNewFileServiceClientImpl::NewFileL
//
// ----------------------------------------------------------------------------
//
TBool CNewFileServiceClientImpl::NewFileL( RFile& aFileHandle,
							   CAiwGenericParamList* aParams,
							   TNewServiceFileType aFileType )
	{
	TUid uid( KNullUid );

	switch ( aFileType )
		{
		case ENewFileServiceAudio:
			{
			uid = TUid::Uid( KVoiceRecorderAppUID3 );
			break;
			}

		case ENewFileServiceVideo:
		case ENewFileServiceImage:
			{
			uid = KUidCamcorder;
			break;
			}
		default:
		    {
		    User::Leave( KErrNotSupported );
		    }
		};

	return NewFileL( uid, aFileHandle, aParams, aFileType );
	}


// ----------------------------------------------------------------------------
// CNewFileServiceClientImpl::NewFileL
//
// ----------------------------------------------------------------------------
//
TBool CNewFileServiceClientImpl::NewFileL( TUid aApplicationUid,
							   RFile& aFileHandle,
							   CAiwGenericParamList* aParams,
							   TNewServiceFileType aFileType )
	{

	iClient.ConnectChainedAppL( aApplicationUid );

	if ( !iServerAppMonitor )
		{
		iServerAppMonitor = CApaServerAppExitMonitor::NewL( iClient, *this,
												CActive::EPriorityStandard );
		}

	HBufC8* serializedParams = SerializeGenericParamListLC( aParams );
	TBool ret = iClient.NewFileL( aFileHandle, serializedParams, aFileType );

	CleanupStack::PopAndDestroy();	// serializedParams

	return ret;
	}


// ----------------------------------------------------------------------------
// CNewFileServiceClientImpl::SerializeGenericParamListLC
//
// ----------------------------------------------------------------------------
//
HBufC8* CNewFileServiceClientImpl::SerializeGenericParamListLC( 
												CAiwGenericParamList* aParams )
	{
	if ( !aParams )
		{
		HBufC8* dummy = HBufC8::NewLC( 0 );	// return dummy
		return dummy;
		}

	TInt size( aParams->Size() );
	HBufC8* serialized = HBufC8::NewLC( size );
	TPtr8 ptr = serialized->Des();
	RDesWriteStream stream( ptr );
	aParams->ExternalizeL( stream );
	stream.Close();
	return serialized;
	}

//  End of File
