/*
* Copyright (c) 2003, 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Setting item for default image/video name
*
*/


// INCLUDE FILES
#include "CVRSettingItem.h"
#include <aknViewAppUi.h>

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CCVRSettingItem::CCVRSettingItem
// C++ constructor
// ---------------------------------------------------------------------------
//
CCVRSettingItem::CCVRSettingItem( TInt aIdentifier,TDes& aText )
: CAknTextSettingItem( aIdentifier, aText )
    {
    }


// Destructor
CCVRSettingItem::~CCVRSettingItem()
    {
    }


// ---------------------------------------------------------------------------
// CCVRSettingItem::EditItemL
// This launches the setting page for text editing. Overridden here
// to use CCVRSettingPage instead of CAknTextSettingPage.
// ---------------------------------------------------------------------------
//
void CCVRSettingItem::EditItemL( TBool /* aCalledFromMenu */ )
    {
    }


	/**
	*  Access for re-implementors to the internal text 
	*
	* @return	reference to a TPtr holding the internal text
	*/
	 TPtr& CCVRSettingItem::InternalTextPtr()
	 	{
	 	  return CAknTextSettingItem::InternalTextPtr();
	 	}

	/**
	*  Access for re-implementors to the external text
	*
	* @return	TPtrC pointing to the external text
	*/
	TPtrC CCVRSettingItem::ExternalText()
		{
			return CAknTextSettingItem::ExternalText();
		}

	/**
	* Allows re-implementors to set the external text
	* No change of ownership is implied.
	*
	* @param	 a reference to the external text
	*
	*/
	void CCVRSettingItem:: SetExternalText( TDesC& aNewExternalText )
		{
			CAknTextSettingItem::SetExternalText(aNewExternalText);
		}



// End of File
