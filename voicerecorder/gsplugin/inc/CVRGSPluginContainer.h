/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Voice Recorder sub-menu in General Settings.
*
*/


#ifndef VOICERECORDERGSPLUGINCONTAINER_H
#define VOICERECORDERGSPLUGINCONTAINER_H

// INCLUDES
#include "voicerecorder.hrh"

// System includes
#include <gsbasecontainer.h>
#include <CAknMemorySelectionDialog.h>

// FORWARD DECLARATIONS
class CAknSettingItemArray;

// CLASS DECLARATION
/**
*  CVRGSPluginContainer class 
*/
class CVRGSPluginContainer  : public CGSBaseContainer
    {
    public: // Constructors and destructor
    
        /**
        * Default C++ constructor
        */
        CVRGSPluginContainer();
                
        /**
        * Symbian OS default constructor.
        *
        * @param aRect gives the correct TRect for construction.
        */
        void ConstructL( const TRect& aRect );

        /**
        * Destructor.
        */
        ~CVRGSPluginContainer();

    public:

        /**
        * Updates list box
        *
        * @param aFeatureId is a updated list box item
        */
        virtual void UpdateListBoxL( TInt aFeatureId );

        /**
        * Creates list box
        *
        * @param aResLbxId is resource number to create.
        */  
        void ConstructListBoxL( TInt aResLbxId );

        /**
        * Retrieves the currently selected listbox feature id
        * @return feature id.
        * @since 3.1
        */
        TInt CurrentFeatureId() const;
        
        /**
        * From CCoeControl changes the size of the list box
        */
        void SizeChanged();
        
        /**
        * Handle listbox selection event
        * @param aCommand Command id to tell origin of event
        */
        void HandleListBoxSelectionL( TInt aCommand );      
        
        /**
        * Store all values from setting items to permanent storage
        */
        void StoreAllL();
        
    protected: //new

        /**
        * Creates list box items
        */  
        virtual void CreateListBoxItemsL();
        
        /**
        * Required for help.
        */
        void GetHelpContext( TCoeHelpContext& aContext ) const;

        
    private:
		/**
		* Array of setting items contained in the listbox. Owned.
		*/
		CAknSettingItemArray* iSettingItemArray;        

        /**
        * Selected memo quality
        */		
		TVRQuality iQuality;	
		
		/**
		* Selected memo store
		*/
		CAknMemorySelectionDialog::TMemory iMemoStore;

		/**
		* Selected memo store
		*/		
		TDriveNumber iDrive;

    };

#endif // VOICERECORDERGSPLUGINCONTAINER_H

// End of File
