/*
* Copyright (c) 2002 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*     This class defines the data model interface for CVRButtonPanel.
*
*/


#ifndef __MVRBUTTONPANELMODEL_H__
#define __MVRBUTTONPANELMODEL_H__

// INCLUDES
#include <e32def.h>

// FORWARD DECLARATIONS
class MVRObserver;

// CLASS DEFINITION
/**
* This class defines the data model interface for CVRButtonPanel.
*/
class MVRButtonPanelModel 
	{
	public: // new methods

		/**
		* Returns the number of buttons.
		* @return number of buttons (>=0)
		*/
		virtual TInt ButtonCount() const = 0;
		
		/**
		* Returns the state of a specific button.
		* @param aButtonId Button identifier
		* @return ENormal or EDimmed
		*/
		virtual TInt ButtonState( TInt aButtonId ) const = 0;
		
		/**
		* Returns the command id of a specific button.
		* @param aButtonId Button identifier
		* @return One of TVRCommands
		*/
		virtual TInt CommandId( TInt aButtonId ) const = 0;
				
		/**
		* Returns the id of the button that should have initial focus.
		* @return Button identifier
		*/
		virtual TInt InitialFocusButtonId() const = 0;
		
		/**
		* Registers an observer for receiving
		* update notifications from this interface.
		* @param aObserver The observer object.
		*/
		virtual void SetButtonPanelObserver( MVRObserver* aObserver ) = 0;
		
		/**
		* Checks if button panel model needs to reset the focus
		* for example after a state change
		* @return Is reset needed
		*/
		virtual TBool ResetNeeded() = 0;
		
		/**
		* Returns an ID of the current visual state.
		* @returns Visual state Id.
		*/
		virtual TUint VisualStateId() const = 0; 		
		
		/**
		* Checks if model is in a state where it can take and handle
		* new commands
		* @return Can model handle new commands
		*/
		virtual TBool CanHandleCommands() const = 0;		
		
	};

#endif // __MVRBUTTONPANELMODEL_H__
