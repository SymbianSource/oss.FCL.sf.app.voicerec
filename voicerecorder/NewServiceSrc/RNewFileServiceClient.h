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



#ifndef RNEWFILESERVICECLIENT_H
#define RNEWFILESERVICECLIENT_H

//  INCLUDES
#include <AknServerApp.h>
#include <e32base.h>
#include <f32file.h>
#include <badesca.h>
#include <NewFileService.hrh>

// FORWARD DECLARATIONS
class CNewFileServiceServerObserver;
class CNewFileServiceClient;


class MNewFileServiceServerObserver
	{
	public: // New functions

		/**
		* This is a callback function which is called when a IHF mode changes.
		* User must derive his class from MVRIHFObserver
		* and implement this method if he is to use notify services.
		*/
		virtual void ServerRequestReady( TInt aError ) = 0;
    };

/**
*  Class that implements the actual client-server communication
*
*  @lib NewService.lib
*  @since Series 60 3.0
*/
class RNewFileServiceClient : public RAknAppServiceBase,
    public MNewFileServiceServerObserver
    {

   public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        RNewFileServiceClient();

        /**
        * Destructor.
        */
        virtual ~RNewFileServiceClient();

    public: // New functions

        /**
        * Handles the communication between server application.
        * After this synchronic operation the new file service has completed
        * and the new file is available for the client
        * @since Series 60 3.0
        * @param aFile File handle to media file
        * @param aGenericParams Parameters passed to server
        * @param aType Type of the media file
        * @param aMultipleFiles ETrue if multiple files can be created
        * @return Was the creation succesful
        */
        TBool NewFileL( RFile& aFile, TDesC8* aGenericParams, TNewServiceFileType aType );
        TBool NewFileL( CDesCArray& aFilenames, TDesC8* aGenericParams, TNewServiceFileType aType, TBool aMultipleFiles );

        void SetFileServiceClient(CNewFileServiceClient*);
        
        CNewFileServiceClient* iNewFileServiceClient;
        
        TInt iError;

    public: // Functions from base classes

        /**
        * Returns the Uid of the service
        * @since Series 60 3.0
        */
        TUid ServiceUid() const;

        /**
        * From MNewFileServiceServerObserver
        */
        void ServerRequestReady( TInt aError );

    private:
		// Wait object used to synchronise asynchronic server request
        CActiveSchedulerWait iWait;

		// Active object observing server request completion. Owned.
        CNewFileServiceServerObserver* iObserver;
     };

#endif      // RNEWFILESERVICECLIENT_H

// End of File
