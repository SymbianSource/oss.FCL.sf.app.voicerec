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
* Description:  Common interface for observing volume events provided by
*                either key presses or side volume keys
*
*/



#ifndef __MVRVOLUMEEVENTOBSERVER_H
#define __MVRVOLUMEEVENTOBSERVER_H

// CLASS DECLARATION

/**
*  Interface for passing volume events to observer
*/
class MVRVolumeEventObserver
    {
    public: // New functions
        
		/**
		* Handles volume change event received from either side volume
		* keys or left-right rocker presses.
		* @param aKeyEvent The key event. Will be forwarded to CAknVolumeControl
		* @param aType The type of key event: EEventKey, EEventKeyUp
		*              or EEventKeyDown. Will be forwarded to CAknVolumeControl
		*/

		virtual TInt HandleVolumeChangeL( const TKeyEvent& aKeyEvent,
									 TEventCode aType ) = 0;		

    };

#endif      // MVRVOLUMEEVENTOBSERVER_H   
            
// End of File
