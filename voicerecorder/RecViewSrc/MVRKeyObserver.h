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
*     An observer class used to hand over key events to classes
*     that can't receive them by themselves (i.e. not CCoeControl derived
*     classes)
*
*/


#ifndef __MVRKEYOBSERVER_H__
#define __MVRKEYOBSERVER_H__

// INCLUDES
#include <w32std.h>

// CLASS DECLARATION
/**
* An observer class used to hand over key events to classes
* that can't receive them by themselves (i.e. not CCoeControl derived
* classes)
*/
class MVRKeyObserver 
    {

    public: // new methods

		/**
		* This function is called when a subject wants to report a key event.
		* @param aKeyEvent The key event.
		* @param aType The type of key event: EEventKey, EEventKeyUp
		*              or EEventKeyDown
		* @return Indicates whether or not the key event was used.
		*/
		virtual TBool ProcessKeyEventL( const TKeyEvent& aKeyEvent,
										const TEventCode aType ) = 0;

    };

#endif // __MVRKEYOBSERVER_H__
