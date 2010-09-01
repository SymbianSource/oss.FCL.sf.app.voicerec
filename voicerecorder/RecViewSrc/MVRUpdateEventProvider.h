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
*  MVRUpdateEventProvider provides an interface for registering
*  and unregistering several MVRObserver derived observers
*
*/


#ifndef __MVRUPDATEEVENTPROVIDER_H
#define __MVRUPDATEEVENTPROVIDER_H

// INCLUDES
#include <e32def.h>

#include "MVRObserver.h"

// CLASS DECLARATION

/**
*  MVRUpdateEventProvider provides an interface for registering
*  and unregistering several MVRObserver derived observers
*
*/
class MVRUpdateEventProvider
    {
    public:
		/**
		* Registers an observer for receiving update notifications.
		* @param aObserver The observer object.
		*/    
		virtual void RegisterObserver( MVRObserver* aObserver ) = 0;
		
		/**
		* Unregisters a previously registered observer
		* @param aObserver The observer object.
		*/		
		virtual void UnregisterObserver( MVRObserver* aObserver ) = 0;
	};

#endif      // __MVRUPDATEEVENTPROVIDER_H

// End of File
