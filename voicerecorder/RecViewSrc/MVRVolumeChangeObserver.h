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
*     This class defines a audio volume change observer interface.
*
*/


#ifndef __MVRVOLUMECHANGEOBSERVER_H__
#define __MVRVOLUMECHANGEOBSERVER_H__

// INCLUDES
#include <e32def.h>

// CLASS DEFINITION
/**
* This class defines a audio volume change observer interface.
*/
class MVRVolumeChangeObserver 
	{
	public: // new methods

		/**
		* Called to notify a change in volume settings.
		* @param aStep Volume level in steps
		* @param aMaxSteps How many steps there are (10 usually).
		*/
		virtual void SetVolume( TInt aStep, TInt aMaxSteps ) = 0;
		
		/***
		* Indicates if volume can be set, because rinto tone
		* volume change has no effect if the tone is playing.
		*/
		virtual TBool CanSetVolume() = 0;
	};

#endif // __MVRVOLUMECHANGEOBSERVER_H__
