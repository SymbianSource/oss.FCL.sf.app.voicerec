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
 *     This class is a part of the standard application framework.
 *     It instantiates the application views.
 *     It also acts as the default command handler for the application.
 *
*/



#include <ErrorUI.h>
#include <coeutils.h>

#include <AknDef.h> // For layout change event definitions

#include "CVRRecView.h"
#include "MVRSelectionProvider.h"
#include "CVRAppUi.h"
#include <sysutil.h>
#include <StringLoader.h>
#include "CVRDocument.h"
#include <featmgr.h>
#include "VRConsts.h"

#include <VoiceRecorderUID.h>
#include <voicerecorder.rsg>
#include "voicerecorder.hrh"
#include "VRUtils.h"
#include "CVRSettingsDialog.h"
#include <hlplch.h>
#include <AknsConstants.h>
#include <aknnotewrappers.h>
#include <csxhelp/vorec.hlp.hrh>
#include <aknradiobuttonsettingpage.h>
#include <CNewFileServiceBase.h>
#include <e32property.h> 
#include <UsbWatcherInternalPSKeys.h>
#include <usbpersonalityids.h>
#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif
#include <aknwaitdialog.h>
_LIT( KVRCommandRecord, "record" );

// ---------------------------------------------------------------------------
// CVRAppUi::CVRAppUi
// 
// ---------------------------------------------------------------------------
//
CVRAppUi::CVRAppUi()
	{
	}


// ---------------------------------------------------------------------------
// CVRAppUi::~CVRAppUi
// 
// ---------------------------------------------------------------------------
//
CVRAppUi::~CVRAppUi()
    {
    if (iActiveWait.IsStarted())
        {
        iActiveWait.AsyncStop();
        }

    if (iDoorObserver)
        {
        iDoorObserver->NotifyExit(MApaEmbeddedDocObserver::EEmpty);
        }

	delete iErrorUI;
	FeatureManager::UnInitializeLib();
	}

// ---------------------------------------------------------------------------
//CVRAppUi::ConstructL
// 
// ---------------------------------------------------------------------------
//
void CVRAppUi::ConstructL()
    {
    BaseConstructL( EAknEnableSkin | EAknEnableMSK 
#ifdef SINGLE_CLICK_INCLUDED
    | EAknSingleClickCompatible
#endif
	);

	iErrorUI = CErrorUI::NewL();

	// Exit if video or voip call is ongoing
	if ( VRUtils::UnsupportedCallTypeOngoing( ETrue ) )
		{
		Exit();
		}

	FeatureManager::InitializeLibL();

	if ( FeatureManager::FeatureSupported( KFeatureIdMmc ) )
		{

#ifndef RD_MULTIPLE_DRIVE
		TVRDriveInfo mmcInfo;
		VRUtils::GetMMCInfo( mmcInfo );
		TVRMemoStore memoStore = VRUtils::MemoStoreL();

		if ( memoStore == EMemoStoreMMC )
			{
            // check if USB is connected, if so, then need to switch to phone memory
            if (CVRUSBStateHanlder::IsUsbActive())
                {
                HBufC* noteText = iCoeEnv->AllocReadResourceLC(
                        R_QTN_MEMC_VOREC_NOTE1 );
                CAknInformationNote* infoNote =
                new( ELeave ) CAknInformationNote( ETrue );
                infoNote->ExecuteLD( *noteText );
                CleanupStack::PopAndDestroy( noteText );
                VRUtils::SetMemoStoreL( EMemoStorePhoneMemory );
                }

			// MMC not available or locked
			else if ( memoStore && ( mmcInfo.iDriveLocked || !mmcInfo.iDrivePresent ) )
				{
				HBufC* noteText = iCoeEnv->AllocReadResourceLC(
														 R_QTN_MEMC_VOREC_NOTE1 );
				CAknInformationNote* infoNote =
									   new( ELeave ) CAknInformationNote( ETrue );
				infoNote->ExecuteLD( *noteText );
				CleanupStack::PopAndDestroy( noteText );
				VRUtils::SetMemoStoreL( EMemoStorePhoneMemory );
				}
			else if ( mmcInfo.iDriveReadOnly )
				{
				HBufC* noteText = iCoeEnv->AllocReadResourceLC(
														 R_QTN_MEMC_VOREC_NOTE2 );
				CAknInformationNote* infoNote =
									   new( ELeave ) CAknInformationNote( ETrue );
				infoNote->ExecuteLD( *noteText );
				CleanupStack::PopAndDestroy( noteText );				
				VRUtils::SetMemoStoreL( EMemoStorePhoneMemory );
				}
			else if ( mmcInfo.iDriveCorrupted )
				{
				// MMC corrupted -> use phone memory
				VRUtils::SetMemoStoreL( EMemoStorePhoneMemory );
				}
			
			}
// for multiple drives
#else

        TInt memoDrive = VRUtils::MemoDriveL();
        if (VRUtils::DriveValid(memoDrive))
            {
            VRUtils::SetMemoDriveL((TDriveNumber) memoDrive);
            }
        else
            {
            TInt defaultDrive = VRUtils::DefaultMemoDriveL(); //eMMC
            if (defaultDrive == memoDrive)
                {
                SetDriveL();
                }
            else
                {
                if (VRUtils::DriveValid(defaultDrive))
                    {
                    VRUtils::SetMemoDriveL((TDriveNumber) defaultDrive);
                    }
                else
                    {
                    SetDriveL();
                    }
                }
            }

        /***** check if memory is below min value, if yes, close app*****/
        RFs& fs(CEikonEnv::Static()->FsSession());
        if (SysUtil::DiskSpaceBelowCriticalLevelL(&fs, 0,
                VRUtils::MemoDriveL()))
            {
            HBufC* errorText = StringLoader::LoadLC(
                    R_VR_MEMORY_LOW_STOP_WARNING);
            CAknErrorNote* dlg = new (ELeave) CAknErrorNote(ETrue);
            dlg->ExecuteLD(*errorText);
            CleanupStack::PopAndDestroy(errorText);
            Exit();
            }
        // check memory size end
#endif

		}

    CVRRecView* view = CVRRecView::NewLC(R_VR_RECORDER_VIEW,
            R_VR_RECVIEW_STATES);

	AddViewL( view );
	iLayoutChangeObserver = view;
	CleanupStack::Pop( view );

	SetDefaultViewL( *view );

    // Activate view straight away, if started through new file service	
    if (iEikonEnv->StartedAsServerApp())
        {
        ActivateLocalViewL(TUid::Uid(KVRRecorderViewUID), TUid::Uid(
                EContextRecordNewForRemote), KNullDesC8);
        }

	}

// ---------------------------------------------------------------------------
// Make user insert the SD card, and choose SD card as the memo storage
// ---------------------------------------------------------------------------
//
void CVRAppUi::SetDriveL()
    {
    if ( VRUtils::DriveValid( EDriveF ) )
    	{
    	VRUtils::SetMemoDriveL((TDriveNumber)EDriveF );
    	}
    else
        {
        if (CVRUSBStateHanlder::IsUsbActive())
            {
            ShowDialogForWaitUSBPluggingOutL();
            Exit();

            }
        while (!VRUtils::DriveValid(EDriveF))
            {
            if (!ShowDialogForWaitStorageCardL())
                {
                Exit();
                }
            }
        // Come to here when drive F is valid
        VRUtils::SetMemoDriveL((TDriveNumber) EDriveF);
        }
    }

// ---------------------------------------------------------------------------
// This method show a dialog to warn user to insert the SD card.
// ---------------------------------------------------------------------------
//
TBool CVRAppUi::ShowDialogForWaitStorageCardL()
    {
    HBufC* text = StringLoader::LoadLC( R_QTN_CCOR_INSERT_MMC );
    CAknQueryDialog* dlg = CAknQueryDialog::NewL();
    TInt result( dlg->ExecuteLD( R_INSERT_F_CARD_DIALOG, *text ) );
    CleanupStack::PopAndDestroy( text );

    if ( result )
    	{
    	return ETrue;
    	}

    return EFalse;
    }


TBool CVRAppUi::ShowDialogForWaitUSBPluggingOutL()
{
    HBufC* text = StringLoader::LoadLC(R_QTN_USB_MODE_NOTE_MODE);
    CAknQueryDialog* dlg = CAknQueryDialog::NewL();
    TInt result(dlg->ExecuteLD(R_INSERT_F_USB_PLUG_IN_DIALOG, *text));
    CleanupStack::PopAndDestroy(text);
    if (result)
        {
        return ETrue;
        }
    return EFalse;    
}

// ---------------------------------------------------------------------------
// CVRAppUi::HandleCommandL
// 
// ---------------------------------------------------------------------------
//
void CVRAppUi::HandleCommandL( TInt aCommand )
    {
	switch ( aCommand )
        {
		case EAknSoftkeyExit:	// Fall through
			{
			if ( iView )
				{
				iView->HandleCommandL( ECmdCancelNote );				
				}
            Exit();
			break;
			}
        case EEikCmdExit:
		   	{
			if ( iView )
				{
				iView->HandleCommandL( ECmdCancelNote );				
				}

            // The file service needs to abort if it's still going on
            if (iEmbeddedObserver)
                {
                CNewFileServiceBase * iNewFileService = NULL;
                iNewFileService
                        = dynamic_cast<CNewFileServiceBase*> (iEmbeddedObserver);
                if (iNewFileService != NULL)
                    {
                    iNewFileService->SetErrorCode(KErrAbort);
                    }
                iEmbeddedObserver->AbortL();
                }

            Exit();
			break;
            }
		case EAknCmdExit:
			{
			if ( iView )
				{
				iView->HandleCommandL( ECmdCancelNote );				
				}

            // The file service needs to abort if it's still going on
            if (iEmbeddedObserver)
                {
                CNewFileServiceBase * iNewFileService = NULL;
                iNewFileService
                        = dynamic_cast<CNewFileServiceBase*> (iEmbeddedObserver);
                if (iNewFileService != NULL)
                    {
                    iNewFileService->SetErrorCode(KErrAbort);
                    }
                iEmbeddedObserver->AbortL();
                }

            Exit();
            break;
            }
        case ECmdHelp:
            {
            if (FeatureManager::FeatureSupported(KFeatureIdHelp))
                {
                if (iIsSettingsOpen)
                    {
                    CArrayFix<TCoeHelpContext>* contexts = AppHelpContextL();
                    contexts->At(0).iContext = KVOREC_HLP_SETTINGS;
                    HlpLauncher::LaunchHelpApplicationL(
                            iEikonEnv->WsSession(), contexts);
                    }
                else
                    {
                    HlpLauncher::LaunchHelpApplicationL(
                            iEikonEnv->WsSession(), AppHelpContextL());
                    }
                }
            break;
            }
        case ECmdSettings:
            {
            if (FeatureManager::FeatureSupported(KFeatureIdMmc)
                    || VRUtils::FeatureEnabled(EVRFeatureShowQualitySetting))
                {
                iIsSettingsOpen = ETrue;
                CVRSettingsDialog* settingsDlg = CVRSettingsDialog::NewL();
                settingsDlg->ExecuteLD(R_VR_SETTINGS_DIALOG);
                iIsSettingsOpen = EFalse;
                }
            break;
            }
        case ECmdNewFileServiceNotify:
            {
            if (iActiveWait.IsStarted())
                {
                iActiveWait.AsyncStop();
                }
            break;
            }
        case ECmdEmbeddedRecordingReady:
            {
            if (iEmbeddedObserver)
                {
                TBool canExit(ETrue);

                TVRSelectionStyle selStyle;
                TPtrC memoFile(SelectionProviderL()-> GetSelectedFilesLC(
                        selStyle)-> MdcaPoint(0));
                if (memoFile.Length() > 0)
                    {
                    TFileName myFilename(memoFile);
                    canExit = iEmbeddedObserver->FileCompleteL(myFilename);
                    }

				// destroy array from GetSelectedFilesLC()
				CleanupStack::PopAndDestroy();

				if ( canExit )
					{
					HandleCommandL( EEikCmdExit );
					}
				else
					{
					// Wait until file service has completed
					// transfering filename array back to client
					if( !iActiveWait.IsStarted() )
					    {
					    iActiveWait.Start(); // CSI: 10 #
					    }
					}
   				}
			break;
  			}
       	
        default:
			{
			break;
			}
        }
    }

// ---------------------------------------------------------------------------
// CVRAppUi::SelectionProviderL
// 
// ---------------------------------------------------------------------------
//
MVRSelectionProvider* CVRAppUi::SelectionProviderL()
	{
    return reinterpret_cast< CVRRecView* >( iView );
	}

// ---------------------------------------------------------------------------
// CVRAppUi::HandleResourceChangeL
// 
// ---------------------------------------------------------------------------
//
void CVRAppUi::HandleResourceChangeL( TInt aType )
	{
	CAknViewAppUi::HandleResourceChangeL( aType );

    if (aType == KEikDynamicLayoutVariantSwitch || aType
            == KAknsMessageSkinChange)
        {
        iLayoutChangeObserver->LayoutChangedL(aType);
        }
    }

// ---------------------------------------------------------------------------
// CVRAppUi::RecordNewFileL
// 
// ---------------------------------------------------------------------------
//
void CVRAppUi::RecordNewFileL( RFile& aFile, MVREmbeddedObserver* aObserver )
	{
	if ( aObserver )
		{
		iEmbeddedObserver = aObserver;

		if ( aFile.SubSessionHandle() != 0)
			{
			// Pass file handle to recview
			SelectionProviderL()->SetFileHandle( aFile );
			}				
		}
	}

// ---------------------------------------------------------------------------
// CVRAppUi::ProcessCommandParametersL
// 
// ---------------------------------------------------------------------------
//
TBool CVRAppUi::ProcessCommandParametersL( TApaCommand /*aCommand*/,
    TFileName& aDocumentName, const TDesC8& aTail )
    {

    // Check if we got "-record" as command line parameter
    // convert 8-bit descriptor to a 16-bit one without altering data.
    const TUint16* ptr16 = reinterpret_cast<const TUint16*> (aTail.Ptr());
    TPtrC16 tail(ptr16, aTail.Size() >> 1);

    if (aDocumentName.Compare(KVRCommandRecord) == 0 || tail.Compare(
            KVRCommandRecord) == 0)
        {
        // Enter recording context
        ActivateLocalViewL(TUid::Uid(KVRRecorderViewUID), TUid::Uid(
                EContextRecordNewNormal), KNullDesC8);
        return EFalse;
        }
    else // No record parameter, start normally
        {
        ActivateLocalViewL(TUid::Uid(KVRRecorderViewUID), TUid::Uid(
                EContextEmptyNormal), KNullDesC8);
        }
        
	if ( !ConeUtils::FileExists( aDocumentName ) )
		{
		aDocumentName.Copy( aTail );
		}		
	return ConeUtils::FileExists( aDocumentName );
    }

// ---------------------------------------------------------------------------
// CVRAppUi::HandleApplicationSpecificEventL
// 
// ---------------------------------------------------------------------------
//
void CVRAppUi::HandleApplicationSpecificEventL(TInt aType,
        const TWsEvent& aEvent)
    {
    if( aType == EAknSoftkeyExit || aType == KAknShutOrHideApp)
    	{
    	Exit();
    	}
    	CEikAppUi::HandleApplicationSpecificEventL(aType, aEvent);
    }

// ---------------------------------------------------------------------------
// CVRAppUi::SetEmbeddedObserver
// 
// ---------------------------------------------------------------------------
//
void CVRAppUi::SetEmbeddedObserver( MVREmbeddedObserver* aObserver )
    {
    iEmbeddedObserver = aObserver;
    }

// End of file
