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
*     Disk space level watcher 
*
*/


#ifndef __CVRDISKSPACEWATCHER_H__
#define __CVRDISKSPACEWATCHER_H__


// CLASS DECLARATION
class CVRMdaRecorder;

/**
 * Database compression timeout timer.
 */
 NONSHARABLE_CLASS(CVRDiskSpaceWatcher) : public CActive
    {
    public:  // Interface
		/**
         * Creates a new instance of this class.
		 * @param aCompressionStrategy compressions strategy
		 * @param aFs file server session
		 * @param aThreshold threshold
		 * @param aDrive disk drive
         */
        static CVRDiskSpaceWatcher* NewL
                (CVRMdaRecorder&, RFs& aFs);

        void RequestNotification(TInt64 aThreshold, TInt aDrive);

		/**
         * Destructor.
         */
        ~CVRDiskSpaceWatcher();

    private:  // from CActive
	    void DoCancel();
        void RunL();
	    TInt RunError(TInt aError);

    private:  // Implementation
        CVRDiskSpaceWatcher(CVRMdaRecorder&, RFs& aFs);

    private:  // Data
		/// Ref. compression strategy
        CVRMdaRecorder& iRecorder;
		/// Ref: file server session
        RFs iFs;
		/// Own: threshold
        TInt64 iThreshold;
		/// Own: disk drive
        TInt iDrive;
    };

#endif // __CVRDISKSPACEWATCHER_H__

// End of File
