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
*     This class defines the data model interface for CVRStateInfoPanel.
*
*/


#ifndef __MVRSTATEINFOMODEL_H__
#define __MVRSTATEINFOMODEL_H__

// INCLUDES
#include <AknLayoutDef.h>

// FORWARD DECLARATIONS
class MVRObserver;
class MVRStateInfoObserver;

// CLASS DEFINITION
/**
* This class defines the data model interface for CVRStateInfoPanel.
**/
class MVRStateInfoModel 
	{
	public: // new methods

		/**
		* Returns the length of the current memo
		* @return The duration in microseconds.
		*/
		virtual TTimeIntervalMicroSeconds Duration() const = 0;
		
		/**
		* Returns the playing or recording position of the current memo.
		* @return The position in microseconds.
		*/
		virtual TTimeIntervalMicroSeconds Position() const = 0;

		/**
		* Returns the text content for a label.
		* @param aName Returned text is placed in this descriptor
		* @param aLabelIndex Label identifier (>=0)
		*/
		virtual void GetLabel( TDes& aName, TInt aLabelIndex ) const = 0;

		/**
		* Returns the resource id for a label.
		* @param aLabelIndex Label identifier (>=0)
		* @return The resource id for the specified label
		*/
		virtual TInt ResourceIdForLabel( TInt aLabelIndex, TAknLayoutId aLayout ) const = 0;

		/**
		* Registers an observer for receiving
		* update notifications from this interface.
		* @param aObserver The observer object.
		*/
		virtual void SetStateInfoObserver( MVRObserver* aObserver ) = 0;
		
		/**
		* Determines is view has a progress bar
		* @return ETrue if view has a progress bar, otherwise EFalse
		*/
		virtual TBool HasProgressBar() const = 0;
		
		/**
		* Registers an observer to communicate about the changes in InfoPanel. 
		* @param aObserver The observer object.
		*/
		virtual void SetStateInfoPanelObserver( MVRStateInfoObserver* aObserver ) = 0;
		
	};

#endif // __MVRSTATEINFOMODEL_H__
