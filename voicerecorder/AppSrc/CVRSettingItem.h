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
* Description:  Setting item for memory settings
*
*/


#ifndef CCVRSETTINGITEM_H
#define CCVRSETTINGITEM_H

// INCLUDES
#include <akntextsettingpage.h>
#include <aknsettingitemlist.h>

// CLASS DECLARATION

/**
* Setting item for name base setting
*/
class CCVRSettingItem : public CAknTextSettingItem
    {
    public: // Constructors and destructor
        /**
        * C++ constructor
        * @param aIdentifier identifier ID
        * @param aText text related to identifier
        * @since 3.1
        */
        CCVRSettingItem( TInt aIdentifier, TDes& aText );
        
	/**
	*  Access for re-implementors to the internal text 
	*
	* @return	reference to a TPtr holding the internal text
	*/
	TPtr& InternalTextPtr();

	/**
	*  Access for re-implementors to the external text
	*
	* @return	TPtrC pointing to the external text
	*/
	TPtrC ExternalText();

	/**
	* Allows re-implementors to set the external text
	* No change of ownership is implied.
	*
	* @param	 a reference to the external text
	*
	*/
	void SetExternalText( TDesC& aNewExternalText );
        /**
        * Destructor
        */
        virtual ~CCVRSettingItem();

    private: // Functions from base classes
        /**
        * From CAknTextSettingItem.
        * This launches the setting page for text editing. Overridden here
        * to use CCVRSettingPage instead of CAknTextSettingPage.
        *
        * @param        aCalledFromMenu - ignored in this class
        */
        void EditItemL( TBool aCalledFromMenu );

    };


#endif
