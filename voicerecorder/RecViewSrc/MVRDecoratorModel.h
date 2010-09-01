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
*     This class defines the data model interface for CVRViewContainer.
*
*/


#ifndef __MVRDECORATORMODEL_H__
#define __MVRDECORATORMODEL_H__

// INCLUDES
#include <e32def.h>
#include <bldvariant.hrh>

// FORWARD DECLARATIONS
class MVRObserver;
class TCoeHelpContext;

// ENUMERATIONS
enum TVRVolumeControlState{ EDisabled, EIhf, EEarPiece };

// CLASS DEFINITION
/**
* This class defines the data model interface for CVRViewContainer.
*/
class MVRDecoratorModel
	{
	public: // new methods

		/**
		* Returns the resource id for current softkeys
		* @return Softkey resource identifier
		*/
		virtual TInt CbaResourceId() const = 0;

		/**
		* Returns the resource id for current menu bar
		* @return Menu bar resource identifier.
		*/
		virtual TInt MenuBarResourceId() const = 0;

		/**
		* Used to get the state of the volume control component.
		*/
		virtual void GetVolumeControlState( TVRVolumeControlState& aState,
											TInt& aVolume ) = 0;
		/**
		* Specifies the current help context.
		* @param aContext the context IDs are placed here
		*/
		virtual void GetHelpContext( TCoeHelpContext& aContext ) const = 0;

		/**
		* Registers an observer for receiving
		* update notifications from this interface.
		* @param aObserver The observer object.
		*/
		virtual void SetDecoratorObserver( MVRObserver* aObserver ) = 0;
		
		/**
		* Tells in return value if CBA Buttons should be enabled or not
		* @return ETrue if buttons should be enabled, EFalse otherwise
		*/
		virtual TBool CBAEnabled() const = 0;

	};

#endif // __MVRDECORATORMODEL_H__
