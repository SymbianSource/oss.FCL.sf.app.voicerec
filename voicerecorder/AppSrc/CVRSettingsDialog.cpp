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
*     This class defines a settings dialog for the Voice Recorder application.
*
*/


#include <bldvariant.hrh>

#include <AknLists.h>
#include <aknform.h>
#include <featmgr.h>
#include "CVRSettingsDialog.h"
#include "VoiceRecorder.hrh"
#include <VoiceRecorder.rsg>
#include "VRConsts.h"
#include "VRUtils.h"

CVRSettingsDialog::~CVRSettingsDialog()
    {
	iSettingItemArray.ResetAndDestroy();
    }

CVRSettingsDialog* CVRSettingsDialog::NewL()
    {
    CVRSettingsDialog* self = new( ELeave ) CVRSettingsDialog;
	CleanupStack::PushL( self );
	self->ConstructL( R_VR_SETTINGS_MENUBAR );
	CleanupStack::Pop();
    return self;
    }

TBool CVRSettingsDialog::OkToExitL( TInt aButtonId )
	{
	if ( aButtonId == EAknSoftkeyOptions )
        {
        DisplayMenuL();
        return EFalse;
        }

	TInt settingItemCount( iSettingItemArray.Count() );

	for ( TInt i( 0 ); i < settingItemCount; i++ )
		{
		iSettingItemArray.At( i )->StoreL();
		}

    if ( VRUtils::FeatureEnabled( EVRFeatureShowQualitySetting ) )
        {
        // Save the quality setting	
	    VRUtils::SetQualityL( iQuality );
        }

	return ETrue;
	}

void CVRSettingsDialog::PreLayoutDynInitL()
	{
    if ( VRUtils::FeatureEnabled( EVRFeatureShowQualitySetting ) )
        {
	    iQuality = VRUtils::QualityL();
        }

	CAknSettingStyleListBox* listBox = static_cast< CAknSettingStyleListBox* >
										( Control( EVRSettingsListboxId ) );
	User::LeaveIfNull( listBox );

	CAknSettingItem* settingItem;
	HBufC* itemTitle = NULL;
	TInt id( 0 );

    if ( VRUtils::FeatureEnabled( EVRFeatureShowQualitySetting ) )
        {
    	// Add quality setting
    	settingItem = new( ELeave ) CAknBinaryPopupSettingItem( 0, ( TInt& ) iQuality );
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

void CVRSettingsDialog::HandleListBoxEventL( CEikListBox* aListBox,
											 TListBoxEvent aEventType )
	{
	TInt index( aListBox->CurrentItemIndex() );
		if ( index >=0 )	// index is -1 if there are no items in the list
			{
			iSettingItemArray.At( index )->EditItemL(
				aEventType == EEventEditingStarted );
			aListBox->DrawItem( index );
			}
	}

void CVRSettingsDialog::ProcessCommandL( TInt aCommandId )
	{
	HideMenu();
	switch ( aCommandId )
		{
		case ECmdChange:
			{
			CEikListBox* listBox = static_cast< CEikListBox* >( Control( EVRSettingsListboxId ) );
			HandleListBoxEventL( listBox, EEventEditingStarted );
			break;
			}
		case EEikCmdExit:
			{
			TryExitL( EAknSoftkeyBack );
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
	
