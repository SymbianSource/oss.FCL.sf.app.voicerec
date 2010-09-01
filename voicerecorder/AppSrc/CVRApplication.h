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
*     This class is a part of the standard application framework.
*     The application gets instantiated starting from this class.
*     Provides a factory method for instantiating the document object.
*
*/


#ifndef CVRAPPLICATION_H
#define CVRAPPLICATION_H

// INCLUDES
#include <aknapp.h>
#include <VoiceRecorderUID.h>

// CONSTANTS
const TUid KVRAppUID = { KVoiceRecorderAppUID3 };

// CLASS DECLARATION
/**
* This class is a part of the standard application framework.
* The application gets instantiated starting from this class.
* Provides a factory method for instantiating the document object.
*/
class CVRApplication
	: public CAknApplication
	{

	private: // from CApaApplication

		/**
		* Creates a CVRDocument instance.
		* @return A pointer to the created document object.
		*/
		CApaDocument* CreateDocumentL();

		/**
		* Returns application's UID value.
		* @return KVRAppUID.
		*/
		TUid AppDllUid() const;
		
		/**
		* Returns a pointer to first-stage constructed server
		* @ return A pointer to AppServer object
		*/
		void NewAppServerL( CApaAppServer*& aAppServer );		
	};

#endif // CVRAPPLICATION_H

