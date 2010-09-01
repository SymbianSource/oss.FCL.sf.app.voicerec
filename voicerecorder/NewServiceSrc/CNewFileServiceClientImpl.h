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
* Description:  Implementation for client side API
*
*/



#ifndef NEWFILESERVICECLIENTIMPL_H
#define NEWFILESERVICECLIENTIMPL_H

//  INCLUDES
#include <NewFileService.hrh>
#include <e32std.h>
#include <badesca.h>
#include <AknServerApp.h>
#include <NewFileServiceClient.h>

// FORWARD DECLARATIONS
class RFile;
class CApaServerAppExitMonitor;
class CAiwGenericParamList;

// CLASS DECLARATION

/**
*  Actual implementation class for client side API
*
*  @lib NewService.lib
*  @since Series 60 3.0
*/
class CNewFileServiceClientImpl : public CNewFileServiceClient,
								  public MAknServerAppExitObserver
    {

    public: // New functions

		static CNewFileServiceClientImpl* NewL();

		~CNewFileServiceClientImpl();

    public: // From CNewFileServiceClient
		TBool NewFileL( CDesCArray& aFileNames,
							   CAiwGenericParamList* aParams,
							   TNewServiceFileType aFileType,
							   TBool aMultipleFiles );

		TBool NewFileL( TUid aApplicationUid,
							   CDesCArray& aFileNames,
							   	CAiwGenericParamList* aParams,
							   TNewServiceFileType aFileType,
							   TBool aMultipleFiles );

		TBool NewFileL( RFile& aFileHandle,
							   CAiwGenericParamList* aParams,
							   TNewServiceFileType aFileType );

		TBool NewFileL( TUid aApplicationUid,
							   RFile& aFileHandle,
							   CAiwGenericParamList* aParams,
							   TNewServiceFileType aFileType );

    public: // from MApaServerAppExitObserver
		void HandleServerAppExit( TInt aReason );

	private:

		void ConstructL();

		HBufC8* SerializeGenericParamListLC( CAiwGenericParamList* aParams );

	private:

		CApaServerAppExitMonitor* iServerAppMonitor;

		RNewFileServiceClient iClient;

    };

#endif      // NEWFILESERVICECLIENTIMPL_H

// End of File
