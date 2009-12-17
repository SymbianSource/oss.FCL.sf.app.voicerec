/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CActive-derived class for synchronizing server commands
*
*/

#ifndef CNEWFILESERVICESERVEROBSERVER_H
#define CNEWFILESERVICESERVEROBSERVER_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MNewFileServiceServerObserver;

/**
 *  CActive-derived class for synchronizing server commands
 *
 *  @lib newservice.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CNewFileServiceServerObserver )
    : public CActive
    {
public:
    static CNewFileServiceServerObserver* NewL();

    ~CNewFileServiceServerObserver();

    /**
     * Wait until RunL is called from server
     *
     * @since S60 3.1
     * @param aObserver The object that is notified when request completes
     * @return Reference to this objects TRequestStatus so that it can be
     *         supplied to server
     */
    TRequestStatus& IssueWait( MNewFileServiceServerObserver* aObserver );

private: // from base class CActive
    /**
     * From CActive
     */
    void RunL();
    void DoCancel();

protected:


private:
    CNewFileServiceServerObserver();

    void ConstructL();

private: // data
    MNewFileServiceServerObserver* iObserver;
    };

#endif // CNEWFILESERVICESERVEROBSERVER_H
