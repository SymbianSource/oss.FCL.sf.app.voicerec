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
*     This class defines a generic observer interface.
*
*/


#ifndef __MVROBSERVER_H__
#define __MVROBSERVER_H__

// INCLUDES
#include "VRConsts.h"

// CLASS DEFINITION
/**
* This class defines a generic observer interface.
*/
class MVRObserver 
	{
	public: // new methods

		/**
		* Called to notify a change in the observed subject's state.
		*/
		virtual void Update( TVRUpdateCommand aCommand = EVRUpdate ) = 0;
		
	};

#endif // __MVROBSERVER_H__
