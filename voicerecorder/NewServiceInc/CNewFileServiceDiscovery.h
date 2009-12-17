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
* Description:  Class implementing the discovery of New file Service servers
*
*/



#ifndef CNEWFILESERVICEDISCOVERY_H
#define CNEWFILESERVICEDISCOVERY_H

//  INCLUDES
#include <e32base.h>

/**
*  Discovery is not supported
*
*  @lib NewService.dll
*  @since Series 60 3.0
*/
class CNewFileServiceDiscovery : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CNewFileServiceDiscovery* NewL();

        /**
        * Destructor.
        */
        virtual ~CNewFileServiceDiscovery();

    public: // New functions

    private:

        /**
        * C++ default constructor.
        */
        CNewFileServiceDiscovery();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    };

#endif      // CNEWFILESERVICEDISCOVERY_H

// End of File
