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
*     This class is a part of the standard application framework.
*     Declares the document class for this application.
*
*/


#ifndef __CVRDOCUMENT_H__
#define __CVRDOCUMENT_H__

// INCLUDES
#include <MediaSharedDocument.h>

// FORWARD DECLARATIONS
class CEikAppUi;

// CLASS DECLARATION
/**
* This class is a part of the standard application framework.
* Declares the document class for this application.
*/
class CVRDocument
	: public CMediaSharedDocument
	{
	public: // Constructors and destructor

		/**
		* Destructor.
		*/
		~CVRDocument();

		/**
        * Static two-phased constructor.
		* @param aApp Reference to the application object.
        */
		static CVRDocument* NewL( CEikApplication& aApp );

	 private:

		/**
		* Default constructor. Private, not implemented.
		*/
		CVRDocument();

		/**
        * Constructor. Inlined to save a few bytes.
		* @param aApp Reference to the application object.
        */
		inline CVRDocument( CEikApplication& aApp )
							: CMediaSharedDocument( aApp ) {}

	private: // from CEikDocument

		/**
        * Instantiates the application UI class.
		* @return CCnvAppUi object instance.
        */
		CEikAppUi* CreateAppUiL();

	};

#endif // __CVRDOCUMENT_H__

