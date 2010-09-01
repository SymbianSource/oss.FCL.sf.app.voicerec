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
* Description: 
* This class can be used to observe changes that has been happened in
* CVRSystemEventHandler class 
*
*/


#ifndef __MVRSYSTEMEVENTOBSERVER_H
#define __MVRSYSTEMEVENTOBSERVER_H

// INCLUDES
#include <e32def.h>
#include <bldvariant.hrh>

// CLASS DECLARATION
/**
*  MVRSystemEventObserver
*/
class MVRSystemEventObserver
    {
    public:  // Constructors and destructor		
    
    virtual void HandleSystemEventL() = 0;

	};

#endif      // __MVRSYSTEMEVENTOBSERVER_H

// End of File
