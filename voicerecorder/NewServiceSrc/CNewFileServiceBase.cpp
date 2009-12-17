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
* Description:  Base class for app servers
*
*/



// INCLUDE FILES
#include <badesca.h>
#include <s32mem.h>
#include <AiwGenericParam.h>

#include "CNewFileServiceBase.h"
#include "NewFileServiceDefs.h"

// CONSTANTS
const TInt KFilenameArrayGranularity( 2 );


// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// CNewFileServiceBase::CNewFileServiceBase
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
EXPORT_C CNewFileServiceBase::CNewFileServiceBase():iError(0)
    {
    }


// ---------------------------------------------------------------------------
// CNewFileServiceBase::~CNewFileServiceBase
//
// ---------------------------------------------------------------------------
//
// Destructor
EXPORT_C CNewFileServiceBase::~CNewFileServiceBase()
    {
	delete iFilenameArray;
	delete iGenericParams;
    }


// ---------------------------------------------------------------------------
// CNewFileServiceBase::DeserializeGenericParamsL
//
// ---------------------------------------------------------------------------
//
void CNewFileServiceBase::DeserializeGenericParamsL( const RMessage2& aMessage )
	{
	if ( !iGenericParams )
		{
		iGenericParams = CAiwGenericParamList::NewL();
		}
	else
		{
		iGenericParams->Reset();
		}

	TInt dataLength( aMessage.GetDesLengthL( ENewServiceIndexFileName ) );
	if ( dataLength == 0 )
		{
		return;
		}

	HBufC8* buffer = HBufC8::NewLC( dataLength );
	TPtr8 ptr( buffer->Des() );
	aMessage.ReadL( ENewServiceIndexFileName, ptr );

	RDesReadStream stream( ptr );
	iGenericParams->InternalizeL( stream );
	CleanupStack::PopAndDestroy();   // buffer
	}


// ---------------------------------------------------------------------------
// CNewFileServiceBase::ServiceL
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CNewFileServiceBase::ServiceL( const RMessage2& aMessage )
	{
#ifdef _DEBUG
	RDebug::Print( _L("CNewFileServiceBase::ServiceL %d"), 
			aMessage.Function() );
#endif
	switch ( aMessage.Function() )
		{
			case ECreateNewFile:
				{
				TPckgBuf< TNewServiceParamPack > paramPack;
				aMessage.ReadL( ENewServiceIndexParams, paramPack );

				if ( !iFilenameArray )
					{
					iFilenameArray = new ( ELeave )
						CDesCArrayFlat( KFilenameArrayGranularity );
					}
				iFilenameArray->Reset();

				DeserializeGenericParamsL( aMessage );

				// Copy the message handle so we can reply to the message when
				// the service observer gets called
				iPendingMessage = aMessage;

				HandleNewFileL( this, *iFilenameArray, paramPack().iType, 
								paramPack().iMultipleFiles );

    			break;
				}

			case ENewFileToHandle:
				{
				TPckgBuf< TNewServiceParamPack > paramPack;
				aMessage.ReadL( ENewServiceIndexParams, paramPack );

				DeserializeGenericParamsL( aMessage );

				// Adopt file handle from client
				RFile file;
				TInt err = file.AdoptFromClient( aMessage, 
												 ENewServiceIndexFsHandle, 
												 ENewServiceIndexFileHandle );
#ifdef _DEBUG
				RDebug::Print( _L("adopt error: %d" ), err );
#endif
				// Copy the message handle so we can reply to the message when
				// the service observer gets called
				iPendingMessage = aMessage;

				HandleNewFileL( this, file, paramPack().iType );

				break;
				}

			case EGetFilenameArrayLength:
				{
				// Calculate the size of the buffer needed to transfer the 
				//filenames to the client side
				TPckgBuf< TInt > lengthBuf;
				TInt count( iFilenameArray->MdcaCount() );
#ifdef _DEBUG
				RDebug::Print( _L("Filename array count: %d"), count );
#endif
				TInt length = sizeof( TInt ) + count * sizeof( TUint16 );
				for ( TInt i( 0 ); i < count; i++ )
					{
					length += iFilenameArray->MdcaPoint( i ).Size();
					}

				lengthBuf = length;
				aMessage.WriteL( ENewServiceIndexParams, lengthBuf );
				aMessage.Complete( KErrNone );
				break;
				}

			case EGetFilenameArray:
				{
				// Allocate a buffer with the same size as in the client side
				TInt length( aMessage.GetDesMaxLengthL( ENewServiceIndexParams ) );
				HBufC8* buffer = HBufC8::NewLC( length );

				TPtr8 ptr = buffer->Des();
				RDesWriteStream stream( ptr );

				// Stream the filename array to the buffer
				TInt count( iFilenameArray->MdcaCount() );
				stream.WriteInt32L( count );
				for ( TInt i( 0 ); i < count; i++ )
					{
					TPtrC filename = iFilenameArray->MdcaPoint( i );
					stream.WriteUint16L( filename.Length() );
					stream.WriteL( filename );
					}

				stream.CommitL();

				// Return the buffer to the client side
				aMessage.WriteL( ENewServiceIndexParams, ptr );
				aMessage.Complete( KErrNone );
				CleanupStack::PopAndDestroy();	// buffer

				// notify the application that the transfer is complete and
				// it's ok to quit / whatever
				ServiceCompleteL();

				break;
				}
			default:
			    {
			    // Complete old message if still open
                if ( !iPendingMessage.IsNull() )
                    {
                    iPendingMessage.Complete( KErrCancel );	
                    }

				CApaAppServiceBase::ServiceL(aMessage);
				break;
			    }
		}
	}


// ---------------------------------------------------------------------------
// CNewFileServiceBase::HandleCompletedNewServiceL
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CNewFileServiceBase::HandleCompletedNewServiceL( 
														TBool aReturnValue )
	{
	// Message was already completed in ServiceL
	if ( iPendingMessage.IsNull() )
	    {
	    return;
	    }

	TNewServiceParamPack pack;
	TPckgBuf< TNewServiceParamPack > paramPack;
	pack.iReturnValue = aReturnValue;
	paramPack = pack;

	iPendingMessage.WriteL( ENewServiceIndexParams, paramPack );
    if(iError != KErrAbort)
    	{
  	    iPendingMessage.Complete( KErrNone );
    	}
    else
    	{
  	    iPendingMessage.Complete( KErrAbort );
  	    iError = 0;    		
    	}
	}


// ---------------------------------------------------------------------------
// CNewFileServiceBase::GenericParams
//
// ---------------------------------------------------------------------------
//
EXPORT_C CAiwGenericParamList* CNewFileServiceBase::GenericParams()
	{
	return iGenericParams;
	}
	

// ---------------------------------------------------------------------------
// CNewFileServiceBase::GenericParams
//
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CNewFileServiceBase::GetErrorCode()
	{
	return iError;
	}
	
	
// ---------------------------------------------------------------------------
// CNewFileServiceBase::GenericParams
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CNewFileServiceBase::SetErrorCode(TInt aError)
	{
    iError = aError;
	}	


//  End of File
