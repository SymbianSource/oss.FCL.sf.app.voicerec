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
*     This class defines a settings dialog for the Voice Recorder application.
*
*/

#include <bldvariant.hrh>

#include <aknlists.h>
#include <featmgr.h>
#include "CVRSettingsDialogMMCSupported.h"
#include "voicerecorder.hrh"
#include <voicerecorder.rsg>
#include "VRConsts.h"
#include "VRUtils.h"
#include <hlplch.h>

#include <CAknMemorySelectionSettingItem.h>
#include <aknnotewrappers.h>
#include <StringLoader.h>
#include <AknCommonDialogsDynMem.h>

#ifdef RD_MULTIPLE_DRIVE
#include <AknCommonDialogsDynMem.h>
#include <driveinfo.h>
#include <CAknMemorySelectionDialogMultiDrive.h>
#endif

// ---------------------------------------------------------------------------
// CVRSettingsDialog::~CVRSettingsDialog
// 
// ---------------------------------------------------------------------------
//
CVRSettingsDialog::~CVRSettingsDialog()
    {
	iSettingItemArray.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CVRSettingsDialog::CVRSettingsDialog
// 
// ---------------------------------------------------------------------------
//
CVRSettingsDialog* CVRSettingsDialog::NewL()
    {
    CVRSettingsDialog* self = new( ELeave ) CVRSettingsDialog;
	CleanupStack::PushL( self );
	self->ConstructL( R_VR_SETTINGS_MENUBAR );
	CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
// CVRSettingsDialog::OkToExitL
// 
// ---------------------------------------------------------------------------
//
TBool CVRSettingsDialog::OkToExitL( TInt aButtonId )
	{
	if ( aButtonId == EAknSoftkeyOptions )
        {
        DisplayMenuL();
        return EFalse;
        }
	
	// If want to exit settings afterwards
	TBool exitAfter( ETrue );
	
	// MSK button pressed in settings view
	if ( aButtonId == ECmdMSK )
		{
		// A new key event is simulated
		TKeyEvent key;
        key.iRepeats = 0;
        key.iCode = EKeyEnter;
        key.iModifiers = 0;
        CEikonEnv::Static()->SimulateKeyEventL( key, EEventKey );
		

		exitAfter = EFalse;	// No Exit
		}
	// Do not store settings anymore when exit
	return exitAfter;
	}



// ---------------------------------------------------------------------------
// CVRSettingsDialog::PreLayoutDynInitL
// Intialize layout
// ---------------------------------------------------------------------------
//
void CVRSettingsDialog::PreLayoutDynInitL()
	{
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
    
    // for multiple drives
#else
    iDrive = (TDriveNumber)VRUtils::MemoDriveL();
    
#endif

	CAknSettingStyleListBox* listBox = static_cast< CAknSettingStyleListBox* >
										( Control( EVRSettingsListboxId ) );
	User::LeaveIfNull( listBox );
    CAknSettingItem* settingItem;
  
    HBufC* itemTitle;
   	TInt id( 0 );
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
    	settingItem->ConstructL( EFalse, id, *itemTitle, NULL,
    	                     R_VR_SETTINGS_DEFAULT_SPEAKER_PAGE, EAknCtPopupField, NULL,
    						 R_VR_SETTINGS_QUALITY_TEXTS );
    	CleanupStack::PopAndDestroy();		// itemTitle

    	// Add quality item to the settings array
    	iSettingItemArray.AppendL( settingItem ); 
    	CleanupStack::Pop();	// settingItem
    	id++;
        }


	// Create the memo store item
	// Set memo store item's title
	itemTitle = iCoeEnv->AllocReadResourceLC( R_QTN_VOREC_SET_STORE );

#ifndef RD_MULTIPLE_DRIVE
	settingItem = new( ELeave ) CAknMemorySelectionSettingItem( 0, iMemoStore );
	CleanupStack::PushL( settingItem ); 
    settingItem->ConstructL( EFalse, id, *itemTitle, NULL,
       R_VOREC_MEMORY_SELECTION_DIALOG, EAknCtPopupSettingList );
	// Add memo store item to the settings array
	iSettingItemArray.AppendL( settingItem );
	id++;
	CleanupStack::Pop();	// settingItem

#else    //multiple drives

	CAknMemorySelectionDialogMultiDrive* dlg = CAknMemorySelectionDialogMultiDrive::NewL(
    ECFDDialogTypeNormal, 
    R_VOREC_MEMORY_SELECTION_DIALOG, // Default resource Id
    EFalse,
    AknCommonDialogsDynMem::EMemoryTypePhone |
    AknCommonDialogsDynMem::EMemoryTypeInternalMassStorage |
    AknCommonDialogsDynMem::EMemoryTypeMMCExternal);

    CleanupStack::PushL( dlg );
    TBool value = dlg->ExecuteL( iDrive );
    CleanupStack::PopAndDestroy( dlg );
    if (value)
     	{
        VRUtils::SetMemoDriveL( iDrive );
       	}
 
#endif


	CleanupStack::PopAndDestroy();	// itemTitle

	CTextListBoxModel* model = listBox->Model();
	model->SetItemTextArray( &iSettingItemArray );
	// Ownership retained by us
	model->SetOwnershipType( ELbmDoesNotOwnItemArray );

	iSettingItemArray.RecalculateVisibleIndicesL();
	listBox->CreateScrollBarFrameL( ETrue );
	listBox->ScrollBarFrame()->SetScrollBarVisibilityL(
						CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
	listBox->HandleItemAdditionL();
	listBox->SetListBoxObserver( this );
	listBox->UpdateScrollBarsL();

	}

// ---------------------------------------------------------------------------
// CVRSettingsDialog::HandleListBoxEventL
// 
// ---------------------------------------------------------------------------
//
void CVRSettingsDialog::HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType)
	{

	switch ( aEventType )
		{
#ifdef SINGLE_CLICK_INCLUDED
        case EEventItemSingleClicked:
#endif
		case EEventItemDoubleClicked: // Pointer events (highlighted item before pressing) 
		case EEventEnterKeyPressed:	// Normal button presses
		case EEventEditingStarted: // From ProcessCommand->ECmdChange
			{
			TInt index( aListBox->CurrentItemIndex() );
			if ( index >=0 )	// index is -1 if there are no items in the list
				{
			    iSettingItemArray.At( index )->EditItemL( 
				aEventType == EEventEditingStarted );
			    aListBox->DrawItem( index );
				}
			StoreAllSettingsL();	
			break;
			}
		default:
			{
			break;	
			}
		}

	}

// ---------------------------------------------------------------------------
// CVRSettingsDialog::ProcessCommandL
// 
// ---------------------------------------------------------------------------
//
void CVRSettingsDialog::ProcessCommandL( TInt aCommandId )
	{
	HideMenu();
	switch ( aCommandId )
		{
		case ECmdChange:
			{
			CEikListBox* listBox = static_cast< CEikListBox* >( Control( EVRSettingsListboxId ) );
			HandleListBoxEventL( listBox, EEventEditingStarted );
			StoreAllSettingsL();
			break;
			}
		case EEikCmdExit:
			{
			TryExitL( EEikCmdExit );
			CEikonEnv::Static()->EikAppUi()->HandleCommandL( aCommandId );
			break;
			}
		default:
			{
			CEikonEnv::Static()->EikAppUi()->HandleCommandL( aCommandId );
			break;
			}
		}
	}

// ---------------------------------------------------------------------------
// CVRSettingsDialog::StoreAllSettingsL
// Stores values in settings list to permanent storage
// ---------------------------------------------------------------------------
//
void CVRSettingsDialog::StoreAllSettingsL()
    {

    TInt settingItemCount( iSettingItemArray.Count() );

	for ( TInt i( 0 ); i < settingItemCount; i++ )
		{
		iSettingItemArray.At( i )->StoreL();
		}

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

    // for multiple drives
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
        VRUtils::SetMemoDriveL( iDrive );
#endif 
     
    if ( VRUtils::FeatureEnabled( EVRFeatureShowQualitySetting ) )
        {
        // Save the quality setting	
	    VRUtils::SetQualityL( iQuality );
        }
    }

// ---------------------------------------------------------------------------
// CVRSettingsDialog::DynInitMenuPaneL
// 
// ---------------------------------------------------------------------------
//
void CVRSettingsDialog::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
	{

	switch ( aResourceId )
		{
		case R_VR_SETTINGS_MENUPANE:
			{
			// Check if HELP is enabled in FeatureManager, if not, disable the Help menu item
			if (!FeatureManager::FeatureSupported(KFeatureIdHelp))
				{
				aMenuPane->SetItemDimmed(ECmdHelp, ETrue);
				}
            break; 
			}
		default:
			{
			break;
			}
		}
	}


// End of file    
