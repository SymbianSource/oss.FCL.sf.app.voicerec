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
    if ( iActiveWait.IsStarted() )
    	{
    	iActiveWait.AsyncStop();
    	}
    
    if ( iDoorObserver )
         {
         iDoorObserver->NotifyExit( MApaEmbeddedDocObserver::EEmpty );
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
    BaseConstructL( EAknEnableSkin | EAknEnableMSK );

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
			if (IsUsbActive())
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
        TInt defaultDrive = VRUtils::DefaultMemoDriveL();
 
        //not default phone memory
		if ( memoDrive != defaultDrive )
			{
             // check if USB is connected, if so, then need to switch to phone memory
			if (IsUsbActive())
				{
				HBufC* noteText = iCoeEnv->AllocReadResourceLC(
														 R_QTN_MEMC_VOREC_NOTE1 );
				CAknInformationNote* infoNote =
									   new( ELeave ) CAknInformationNote( ETrue );
				infoNote->ExecuteLD( *noteText );
				CleanupStack::PopAndDestroy( noteText );
				VRUtils::SetMemoDriveL((TDriveNumber)defaultDrive );
				}
			else
				{
            	TUint status( 0 );
	        	VRUtils::GetDriveInfo(memoDrive, status);
				
				// Drive not available or locked
				if ((status & DriveInfo::EDriveLocked) || !(status & DriveInfo::EDrivePresent))
			    	{
			    	HBufC* noteText = iCoeEnv->AllocReadResourceLC(R_QTN_MEMC_VOREC_NOTE1 );
					CAknInformationNote* infoNote = new( ELeave ) CAknInformationNote( ETrue );
					infoNote->ExecuteLD( *noteText );
					CleanupStack::PopAndDestroy( noteText );
					VRUtils::SetMemoDriveL((TDriveNumber)defaultDrive );
					}
				else if ( status & DriveInfo::EDriveReadOnly )
			    	{
			    	HBufC* noteText = iCoeEnv->AllocReadResourceLC(R_QTN_MEMC_VOREC_NOTE2 );
					CAknInformationNote* infoNote = new( ELeave ) CAknInformationNote( ETrue );
					infoNote->ExecuteLD( *noteText );
					CleanupStack::PopAndDestroy( noteText );				
					VRUtils::SetMemoDriveL((TDriveNumber)defaultDrive );
					}
		    	else if ( status & DriveInfo::EDriveCorrupt )
			    	{
			    	// Drive corrupted -> use phone memory
			    	VRUtils::SetMemoDriveL((TDriveNumber)defaultDrive );
			    	}
				}
			}


#endif

		}

	CVRRecView* view = CVRRecView::NewLC(
									R_VR_RECORDER_VIEW, R_VR_RECVIEW_STATES );

	AddViewL( view );
	iLayoutChangeObserver = view;
	CleanupStack::Pop( view );

	SetDefaultViewL( *view );

    // Activate view straight away, if started through new file service	
	if ( iEikonEnv->StartedAsServerApp() )
		{
    	ActivateLocalViewL( TUid::Uid( KVRRecorderViewUID ),
        	TUid::Uid( EContextRecordNewForRemote ), KNullDesC8 );        	
		}        

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
			if ( iEmbeddedObserver )
				{
				dynamic_cast<CNewFileServiceBase*>(iEmbeddedObserver)->SetErrorCode(KErrAbort);
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
			if ( iEmbeddedObserver )
				{
				dynamic_cast<CNewFileServiceBase*>(iEmbeddedObserver)->SetErrorCode(KErrAbort);
				iEmbeddedObserver->AbortL();
				}
			
			Exit();
            break;
			}
		case ECmdHelp:
			{
			if ( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
				{
				if ( iIsSettingsOpen )
					{
					CArrayFix<TCoeHelpContext>* contexts = AppHelpContextL();
					contexts->At(0).iContext = KVOREC_HLP_SETTINGS;
					HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
													contexts );
					}
				else
					{
					HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
													AppHelpContextL() );
					}
				}
			break;
			}
		case ECmdSettings:
			{
			if ( FeatureManager::FeatureSupported( KFeatureIdMmc ) ||
                 VRUtils::FeatureEnabled( EVRFeatureShowQualitySetting ) )
				{
				iIsSettingsOpen = ETrue;
				CVRSettingsDialog* settingsDlg = CVRSettingsDialog::NewL();
				settingsDlg->ExecuteLD( R_VR_SETTINGS_DIALOG );
				iIsSettingsOpen = EFalse;
				}
			break;
			}
       	case ECmdNewFileServiceNotify:
   			{
			if ( iActiveWait.IsStarted() )
				{
				iActiveWait.AsyncStop();
				}
			break;
   			}
       	case ECmdEmbeddedRecordingReady:
   			{
   			if ( iEmbeddedObserver )
   				{
   				TBool canExit( ETrue );
   				
				TVRSelectionStyle selStyle;
				TPtrC memoFile( SelectionProviderL()->
								GetSelectedFilesLC( selStyle )->
								MdcaPoint( 0 ) );
				if ( memoFile.Length() > 0 )
					{   		
					TFileName myFilename( memoFile );
					canExit = iEmbeddedObserver->FileCompleteL( myFilename );
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

	if ( aType == KEikDynamicLayoutVariantSwitch ||
		 aType == KAknsMessageSkinChange )
		{
		iLayoutChangeObserver->LayoutChangedL( aType );
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
	const TUint16* ptr16 = reinterpret_cast< const TUint16* >(
												   aTail.Ptr() );
	TPtrC16 tail( ptr16, aTail.Size() >> 1);	

	if ( aDocumentName.Compare( KVRCommandRecord ) == 0
		|| tail.Compare( KVRCommandRecord ) == 0 )
		{
		// Enter recording context
		ActivateLocalViewL( TUid::Uid( KVRRecorderViewUID ),
				TUid::Uid( EContextRecordNewNormal ), KNullDesC8 );
		return EFalse;
		}
    else // No record parameter, start normally
        {
    	ActivateLocalViewL( TUid::Uid( KVRRecorderViewUID ),
	    	TUid::Uid( EContextEmptyNormal ), KNullDesC8 );        
        }
        
	if ( !ConeUtils::FileExists( aDocumentName ) )
		{
		aDocumentName.Copy( aTail );
		}		
	return ConeUtils::FileExists( aDocumentName );
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
    
    
// ---------------------------------------------------------------------------
// CVRAppUi::IsUsbActive  
// ---------------------------------------------------------------------------
//
TBool CVRAppUi::IsUsbActive()
    {
    TInt usbState;
    TInt err = RProperty::Get(  KPSUidUsbWatcher, KUsbWatcherSelectedPersonality,usbState);
    return !err && usbState == KUsbPersonalityIdMS;
    }
// End of file
