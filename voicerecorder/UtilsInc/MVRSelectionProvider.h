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
*      Interface for file selection
*
*
*/


#ifndef __MVRSELECTIONPROVIDER_H__
#define __MVRSELECTIONPROVIDER_H__

// INCLUDES
#include <bamdesca.h>
#include <f32file.h>

// ENUMERATIONS
enum TVRSelectionStyle { EEmptyList, EFocusedItem, ESingleMarkedItem, EMarkedItems };

// CLASS DEFINITION
/**
*
*/
class MVRSelectionProvider
	{
	public: // new methods

		/**
		* Ownership is transferred. Caller must destroy the returned array.
		* @return array of file names
		*/
		virtual MDesCArray* GetSelectedFilesLC( TVRSelectionStyle& aStyle ) const = 0;

		/**
		* Transfer the file handle to view
		* @param aFile File handle
		*/
		virtual void SetFileHandle( RFile& aFile ) = 0;
		
		/**
		* Get the handle to the recorded clip.
		* @return Open file handle
		*/
		virtual RFile& GetFile() = 0;
	};

#endif // __MVRSELECTIONPROVIDER_H__
