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


// INCLUDES
#include "CVRGSPluginContainer.h"
#include "CVRGSPlugin.h"

#include <voicerecorder.rsg>
#include <VoiceRecorderUID.h>

#include "VRConsts.h"
#include "VRUtils.h"
#include <csxhelp/vorec.hlp.hrh>


#include <aknlists.h>               //for CAknSettingStyleListBox
#include <aknsettingitemlist.h>
#include <CAknMemorySelectionSettingItem.h>
#include <aknnotewrappers.h> // for CAknInformationNote
//#include <GSCamcorderPlugin.mbg>
#include <gslistbox.h>              //for listbox clases
#include <AknIconArray.h>   // CAknIconArray
#include <eikclbd.h>        // CColumnListBoxData

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#include <CAknMemorySelectionSettingItemMultiDrive.h>
#include <AknCommonDialogsDynMem.h>
#endif

const TUid KVRAppUID = { KVoiceRecorderAppUID3 };

// ListBox item index
enum TListItemIndex
    {
    EListItemQualityIndex = 0,
    ElistItemMemoStoreIndex,
    };

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CVRGSPluginContainer::ConstructL
// Symbian OS default constructor
// ---------------------------------------------------------------------------
//
void CVRGSPluginContainer::ConstructL( const TRect& aRect )
    {
    // Construct iListBox
    iListBox = new ( ELeave ) CAknSettingStyleListBox;      
    iSettingItemArray = new(ELeave) CAknSettingItemArray(2,0,0);

    if ( VRUtils::FeatureEnabled( EVRFeatureShowQualitySetting ) )
        {
	    iQuality = VRUtils::QualityL();
        }

	// Convert the internal memory enumeration to CFD enumeration
#ifndef RD_MULTIPLE_DRIVE 
	if ( VRUtils::MemoStoreL() == EMemoStorePhoneMemory )
		{
		iMemoStore = CAknMemorySelectionSettingPage::EPhoneMemory;
		}
	else
		{
		iMemoStore = CAknMemorySelectionSettingPage::EMemoryCard;
		}    
#else
    iDrive = (TDriveNumber)VRUtils::MemoDriveL();

#endif

    BaseConstructL( aRect, R_VOREC_GS_VIEW_TITLE, 0 );
    }


// ---------------------------------------------------------------------------
// CVRGSPluginContainer::~CGSTelPluginContainer
// Destructor
// ---------------------------------------------------------------------------
//
CVRGSPluginContainer::~CVRGSPluginContainer()
    {
    if ( iSettingItemArray )
        {
        iSettingItemArray->ResetAndDestroy();
        }
    
    delete iSettingItemArray;
    }

// ---------------------------------------------------------------------------
// CVRGSPluginContainer::CGSTelPluginContainer
// Default constructor
// ---------------------------------------------------------------------------
//
CVRGSPluginContainer::CVRGSPluginContainer()
    : iQuality( EQualityMMSOptimized ),
      iMemoStore( CAknMemorySelectionSettingPage::EPhoneMemory )
    {
    }
    
// ---------------------------------------------------------------------------
// CVRGSPluginContainer::UpdateListBoxL
// Update listbox item
// ---------------------------------------------------------------------------
//
void CVRGSPluginContainer::UpdateListBoxL( TInt /*aFeatureId*/ )
    {
    
    // Update the listbox
    iListBox->HandleItemAdditionL();
    }

    
// ---------------------------------------------------------------------------
// CVRGSPluginContainer::ConstructListBoxL
// Update listbox item
// Items are added manually, instead of loading them from resources
// ---------------------------------------------------------------------------
//
void CVRGSPluginContainer::ConstructListBoxL( TInt /*aResLbxId*/ )
    {
    iListBox->ConstructL( this, EAknListBoxSelectionList );

    // Create items to array
    CreateListBoxItemsL(); 

    // Set array to listbox
	CTextListBoxModel* model = iListBox->Model();
	model->SetItemTextArray( iSettingItemArray );
	// Ownership retained by us
	model->SetOwnershipType( ELbmDoesNotOwnItemArray );

	iSettingItemArray->RecalculateVisibleIndicesL();
	iListBox->CreateScrollBarFrameL( ETrue );
	iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
						CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
	iListBox->HandleItemAdditionL();
	iListBox->UpdateScrollBarsL();
    }


// ---------------------------------------------------------------------------
// CVRGSPluginContainer::CreateListBoxItemsL
// Update listbox item
// ---------------------------------------------------------------------------
//
void CVRGSPluginContainer::CreateListBoxItemsL()
    {
    CAknSettingItem* settingItem;
    HBufC* itemTitle;

	// Add quality setting
    if ( VRUtils::FeatureEnabled( EVRFeatureShowQualitySetting ) )
        {
 
    	// Add quality setting
#ifdef  __AAC_ENCODER_PLUGIN    	
    	settingItem = new( ELeave ) CAknEnumeratedTextPopupSettingItem ( 0, ( TInt& ) iQuality );

#else
        settingItem = new( ELeave ) CAknBinaryPopupSettingItem ( 0, ( TInt& ) iQuality );
#endif
    	CleanupStack::PushL( settingItem );

    	itemTitle = iCoeEnv->AllocReadResourceLC( R_QTN_VOREC_SET_QUALITY );
    	settingItem->ConstructL( EFalse, EListItemQualityIndex, *itemTitle, NULL,
    	                     R_VR_SETTINGS_DEFAULT_SPEAKER_PAGE, 
    	                     EAknCtPopupField, NULL,
    						 R_VR_SETTINGS_QUALITY_TEXTS );
    	
    	CleanupStack::PopAndDestroy();		// itemTitle

    	// Add quality item to the settings array
    	iSettingItemArray->AppendL( settingItem );
      	CleanupStack::Pop();	// settingItem        
        }
      if ( VRUtils::MultipleMassStorageAvailable() )
        {
    // Add memo store setting
	// Create the memo store item
#ifndef RD_MULTIPLE_DRIVE	
	settingItem =
				new( ELeave ) CAknMemorySelectionSettingItem( 0, iMemoStore );
   //multiple drives
#else
    settingItem =
                new( ELeave ) CAknMemorySelectionSettingItemMultiDrive(0, iDrive );
       TInt includedMedias = AknCommonDialogsDynMem::EMemoryTypeInternalMassStorage |
                          AknCommonDialogsDynMem::EMemoryTypeMMCExternal;
       static_cast< CAknMemorySelectionSettingItemMultiDrive *> ( settingItem )->SetIncludedMediasL( includedMedias );
#endif

	CleanupStack::PushL( settingItem );

	itemTitle = iCoeEnv->AllocReadResourceLC( R_QTN_VOREC_SET_STORE );

#ifndef RD_MULTIPLE_DRIVE
	// Set memo store item's title
	settingItem->ConstructL( EFalse, id, *itemTitle, NULL,
	    R_VOREC_MEMORY_SELECTION_DIALOG, EAknCtPopupSettingList );
#else
        settingItem->ConstructL( EFalse, ElistItemMemoStoreIndex, *itemTitle, NULL, 0, EAknCtPopupSettingList );
#endif
	    
	CleanupStack::PopAndDestroy();	// itemTitle

        // Add memo store item to the settings array
        iSettingItemArray->AppendL( settingItem );
        CleanupStack::Pop();	// settingItem
    	}
    }


// ---------------------------------------------------------------------------
// CVRGSPluginContainer::CurrentFeatureId
// Get the currently selected feature
// ---------------------------------------------------------------------------
//
TInt CVRGSPluginContainer::CurrentFeatureId() const
    {
    if ( iSettingItemArray )
        {
        return iSettingItemArray->MdcaCount();
        }
    else
        {
        return 0;
        }
    }

// ---------------------------------------------------------------------------
// CVRGSPluginContainer::GetHelpContext
// Get help for this view
// ---------------------------------------------------------------------------
//
void CVRGSPluginContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KVRAppUID; 
    aContext.iContext = KVOREC_HLP_SETTINGS;    
    }
  
// ---------------------------------------------------------------------------
// CVRGSPluginContainer::SizeChanged
// Set the size and position of component controls.
// ---------------------------------------------------------------------------
//
void CVRGSPluginContainer::SizeChanged()
    {
    if ( iListBox )
        {
        iListBox->SetRect( Rect() );
        }
    }
  
void CVRGSPluginContainer::HandleListBoxSelectionL( TInt aCommand ) 
    {
	TInt index( iListBox->CurrentItemIndex() );
	TInt driveDefaultMassStorage = VRUtils::DefaultMemoDriveL();
	TInt driveRemovableMassStorage = VRUtils::GetRemovableMassStorageL();
	
	if ( index >=0 )	// index is -1 if there are no items in the list
		{
        if(index == ElistItemMemoStoreIndex &&
		        !VRUtils::DriveValid(driveDefaultMassStorage) && 
		            !VRUtils::DriveValid(driveRemovableMassStorage) )
            {
            return;
            }
		// Open edit dialog if EAknCmdOpen, invert the value otherwise
		iSettingItemArray->At( index )->EditItemL( aCommand == EAknCmdOpen );
		iListBox->DrawItem( index );
		//Store all settings after change
		StoreAllL();
		}
    }  

// ---------------------------------------------------------------------------
// CVRGSPluginContainer::StoreAll
// Stores values in settings list to permanent storage
// ---------------------------------------------------------------------------
//
void CVRGSPluginContainer::StoreAllL()
    {
    // Retrieve values to member variables
	TInt settingItemCount( iSettingItemArray->Count() );

	for ( TInt i( 0 ); i < settingItemCount; i++ )
		{
		iSettingItemArray->At( i )->StoreL();
		}

    // Save memo store setting
#ifndef RD_MULTIPLE_DRIVE    
	if ( iMemoStore == CAknMemorySelectionSettingPage::EPhoneMemory )
		{
		VRUtils::SetMemoStoreL( EMemoStorePhoneMemory );
		}
	else
		{
		// check if MMC is read-only
		TVRDriveInfo mmcInfo;
		VRUtils::GetMMCInfo( mmcInfo );

		if ( mmcInfo.iDriveReadOnly )
			{
			HBufC* noteText = iCoeEnv->AllocReadResourceLC(
													 R_QTN_MEMC_READONLY );
			CAknInformationNote* infoNote =
								new( ELeave ) CAknInformationNote( ETrue );
			infoNote->ExecuteLD( *noteText );
			CleanupStack::PopAndDestroy();	// noteText
			VRUtils::SetMemoStoreL( EMemoStorePhoneMemory );
			}
		else
			{
			VRUtils::SetMemoStoreL( EMemoStoreMMC );
			}
		}
#else
     TUint status( 0 );
	 TInt defaultDrive = VRUtils::DefaultMemoDriveL();
	 if (VRUtils::MemoDriveL() != defaultDrive)
	 	{
 	    VRUtils::GetDriveInfo(iDrive, status);
     	// check if drive status is ok 
	 	if ( status & DriveInfo::EDriveReadOnly )
			{
			HBufC* noteText = iCoeEnv->AllocReadResourceLC(R_QTN_MEMC_READONLY );
			CAknInformationNote* infoNote =	new( ELeave ) CAknInformationNote( ETrue );
			infoNote->ExecuteLD( *noteText );
			CleanupStack::PopAndDestroy();	// noteText
        
        	// use default device drive to save files
        	iDrive = (TDriveNumber)defaultDrive;
  			}
	 	}
    if(iDrive == defaultDrive || iDrive == VRUtils::GetRemovableMassStorageL())
        {
        VRUtils::SetMemoDriveL( iDrive );
        }
#endif 

    // Save quality setting
    if ( VRUtils::FeatureEnabled( EVRFeatureShowQualitySetting ) )
        {
        // Save the quality setting	
	    VRUtils::SetQualityL( iQuality );
        }
        
    }
  
// End of File
