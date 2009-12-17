/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface for recording new media files using platform
                 applications
*
*/



#ifndef CNEWFILESERVICEBASE_H
#define CNEWFILESERVICEBASE_H

//  INCLUDES
#include <aknserverapp.h>
#include <badesca.h>
#include <f32file.h>
#include <NewFileService.hrh>

// FORWARD DECLARATIONS
class CAiwGenericParamList;


// CLASS DECLARATION

/**
*  Observer interface to get notification from server application
*
*  @lib NewService.lib
*  @since Series 60 3.0
*/
class MNewFileServiceObserver
	{
	public:
		virtual void HandleCompletedNewServiceL( TBool aReturnValue ) = 0;
	};

/**
*  Base class for server applications
*
*  @lib NewService.lib
*  @since Series 60 3.0
*/
class CNewFileServiceBase : public CAknAppServiceBase, public MNewFileServiceObserver
    {
    public:  // Constructors and destructor

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CNewFileServiceBase();

        /**
        * return iErrorCode.
        */
        IMPORT_C TInt GetErrorCode();
        
        /**
        * set iErrorCode.
        */        
        IMPORT_C void SetErrorCode(TInt);


    protected:  // New functions

        /**
        * From CAknAppServiceBase
        * @since Series 60 3.0
        * @param aMessage The client message
        */
        IMPORT_C void ServiceL( const RMessage2& aMessage );

        /**
        * From MNewFileServiceObserver
        * @since Series 60 3.0
        * @param aReturnValue ETrue if the media file was succesfully created
        */
		IMPORT_C void HandleCompletedNewServiceL( TBool aReturnValue );

        /**
        * Getter for member variable
        * @since Series 60 3.0
        * @return Pointer to iGenericParams
        */        
		IMPORT_C CAiwGenericParamList* GenericParams();

        /**
        * Create a new media file
        * @since Series 60 3.0
        * @param aObserver Object that is notified when recording is ready
        * @param aFilenameArray Array for the filename(s) of the recorded files
        * @param aType Required media type
        * @param aMultiplefiles ETrue if multiples files can be recorded
        */
        virtual void HandleNewFileL( MNewFileServiceObserver* aObserver,
        							  CDesCArray& aFilenameArray,
        							  TNewServiceFileType aType,
        							  TBool aMultipleFiles ) = 0;
        /**
        * Create a new media file
        * @since Series 60 3.0
        * @param aObserver Object that is notified when recording is ready
        * @param aFile File handle that the media file is to be recorded
        * @param aType Required media type
        */
        virtual void HandleNewFileL( MNewFileServiceObserver* aObserver,
        							  RFile& aFile,
        							  TNewServiceFileType aType ) = 0;

        /**
        * Notifies the server application that service is complete
        * and it may safely close
        * @since Series 60 3.0
        */
		virtual void ServiceCompleteL() = 0;

	private:

		void DeserializeGenericParamsL( const RMessage2& aMessage );


    protected:

        /**
        * C++ default constructor.
        */
        IMPORT_C CNewFileServiceBase();

    private:    // Data
        // Array for the filenames
        CDesCArrayFlat* iFilenameArray;

        // Last received message
        RMessage2 iPendingMessage;

        // Generic parameter array
        CAiwGenericParamList* iGenericParams;
        
        // Error code that send back to client
        TInt iError;

    };

#endif      // CNewFileServiceBase_H

// End of File
