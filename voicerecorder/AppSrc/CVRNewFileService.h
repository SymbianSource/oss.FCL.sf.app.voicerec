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


#ifndef __CVRNEWFILESERVICE_H__
#define __CVRNEWFILESERVICE_H__

// INCLUDES
#include <CNewFileServiceBase.h>
#include "MVREmbeddedObserver.h"

// CONSTANTS

// CLASS DECLARATION
/**
* This class is part of new file service framework.
*/
class CVRNewFileService
	: public CNewFileServiceBase, public MVREmbeddedObserver
	{

	public:
		static CVRNewFileService* NewL();
		~CVRNewFileService();

		/*
		* Receive message from service to record new files
		*/
        void HandleNewFileL( MNewFileServiceObserver* aObserver,
        							  CDesCArray& aFilenameArray,
        							  TNewServiceFileType aType,
        							  TBool aMultipleFiles );
		/*
		* Receive message from service to record new file into file handle
		*/
        void HandleNewFileL( MNewFileServiceObserver* aObserver,
        							  RFile& aFile,
        							  TNewServiceFileType aType );

		/*
		* Receive message from service that file service has completed
		* and can be closed
		*/
		void ServiceCompleteL();
		
	public: // MVREmbeddedObserver        							  
		/*
		* Receive comand from application that file named aName has been recorded
		* @param aName The name of the recorded file
		* @return Can the application be closed or should we wait
		*/
		virtual TBool FileCompleteL( const TDesC& aName );
		
		/*
		* Receive command from application that file service must be aborted
		*/
		void AbortL();		

	private:
		CVRNewFileService();
		
	private: // data
		/*
		* Pointer to file service observer
		*/
		MNewFileServiceObserver* iObserver;
		
		/*
		* If file service has already completed
		*/
		TBool iCompleted;

        /*
        * Are we recording to file handle or filenamearray
        */		
		TBool iFileHandleUsed;
	
	    /*
	    * Contains name of the target file. Not owned.
	    */
		CDesCArray* iFilenameArray;
		
		/*
		* Used for asynchronic operations
		*/
		CActiveSchedulerWait iActiveWait;
		
	};

#endif // __CVNEWFILESERVICE_H__

