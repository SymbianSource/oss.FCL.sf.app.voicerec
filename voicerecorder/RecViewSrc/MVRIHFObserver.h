/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Template class for IHF change observers
*
*/


#ifndef __MVRIHFOBSERVER_H__
#define __MVRIHFOBSERVER_H__

//  CLASS DEFINITION
/**
 *
 * Template class for observers of IHF mode changes
 *
 */
class MVRIHFObserver
	{
	public: // New functions

		/**
		* This is a callback function which is called when a IHF mode changes.
		* User must derive his class from MVRIHFObserver
		* and implement this method if he is to use notify services.
		*/
		virtual void HandleIHFChange() = 0;
    };

#endif      //  __MVRIHFOBSERVER_H__
