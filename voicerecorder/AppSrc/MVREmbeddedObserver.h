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
*     An observer class used to notify objects that embedded recording
*	  was completed
*
*/


#ifndef MVREMBEDDEDOBSERVER_H
#define MVREMBEDDEDOBSERVER_H

// INCLUDES
#include <w32std.h>

// CLASS DECLARATION
/**
* An observer class used to notify objects that embedded recording
* was completed
*/
class MVREmbeddedObserver 
    {

    public: // new methods

		/**
		* This function is called when application wants to notify
		* the observer that embedded clip was recorded
		* @param aName The name of the file that was recorded
		*              or EEventKeyDown
		* @return Can we close the application
		*/
		virtual TBool FileCompleteL( const TDesC& aName ) = 0;
		
		/**
		* This function may be used to inform server to abort transfer.
		* If operation already has completed, nothing is done.
		*/
		virtual void AbortL() = 0;
    };

#endif // MVREMBEDDEDOBSERVER_H
