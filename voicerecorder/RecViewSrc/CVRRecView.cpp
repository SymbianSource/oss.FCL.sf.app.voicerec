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
*      Voice Recorder Record View
*
*
*/


// INCLUDE FILES

#include <aknViewAppUi.h>
#include <AknQueryDialog.h>
#include <avkon.hrh>
#include <barsread.h>
#include <eikbtgpc.h>
#include <eikmenub.h>
#include <StringLoader.h>
#include <apparc.h>
#include <eikenv.h>
#include <eikappui.h>
#include <eikapp.h>
#include <CommonContentPolicy.h>
#include <vwsdef.h>
#include <pathinfo.h>
#include <sendui.h>
#include <avkon.rsg>
#include <featmgr.h>
#include <VoiceRecorderUID.h>
#include <voicerecorder.rsg>
#include <e32property.h> 
#include <coreapplicationuisdomainpskeys.h>
#include <AknLaunchAppService.h>
#include <AiwGenericParam.h>

#include "voicerecorder.hrh"
#include "CVRRecView.h"
#include "CVRRecViewContainer.h"
#include "CVRRecViewActivationContainer.h"
#include "CVRRecViewModel.h"
#include "VRUtils.h"
#include "VRUSBStateHanlder.h"

// CONSTANTS
const TUid KUidMmsEditor =
    {
    0x100058DE
    };
const TUid KUidMailEditor =
    {
    0x101F4CD6
    };

inline TBool ShowDialogForWaitUSBPluggingOutL()
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

inline TInt WaitDialogForUSBPluggingOut(CAknWaitDialog*& aDialog)
    {
    aDialog = new (ELeave) CAknWaitDialog(
            (REINTERPRET_CAST(CEikDialog**,&aDialog)));
    aDialog->SetTone(CAknNoteDialog::EConfirmationTone);
    return aDialog->ExecuteLD(R_VOREC_USB_CONNECTED_DIALOG);
    }

// ================= MEMBER FUNCTIONS ========================================

// ---------------------------------------------------------------------------
// CVRRecView::CVRRecViewModelActivator::CVRRecViewModelActivator
// 
// ---------------------------------------------------------------------------
//
CVRRecView::CVRRecViewModelActivator::CVRRecViewModelActivator(
        CVRRecViewModel* aModel) :
    CAsyncOneShot(EPriorityNormal), iModel(aModel), iContext(
            EContextEmptyNormal)
    {
    }


// ---------------------------------------------------------------------------
// CVRRecView::CVRRecViewModelActivator::~CVRRecViewModelActivator
// 
// ---------------------------------------------------------------------------
//
CVRRecView::CVRRecViewModelActivator::~CVRRecViewModelActivator()
	{
	Cancel();
	}


// ---------------------------------------------------------------------------
// CVRRecView::CVRRecViewModelActivator::RunL
// 
// ---------------------------------------------------------------------------
//
void CVRRecView::CVRRecViewModelActivator::RunL()
	{
	// Activate model in correct context
	iModel->SetMemoNameNewL( iContext == EContextRecordNewForRemote);
	iModel->EnterContextL( iContext );
	iModel->ActivateL();
	
	// Notify recview
	iCallback.CallBack();
	}


// ---------------------------------------------------------------------------
// CVRRecView::CVRRecViewModelActivator::Activate
// 
// ---------------------------------------------------------------------------
//
void CVRRecView::CVRRecViewModelActivator::Activate( 
		TVRRecViewContexts aContext, TCallBack aCallback )
	{
	iContext = aContext;
	iCallback = aCallback;
	Call();

	}


// ---------------------------------------------------------------------------
// CVRRecView::CVRRecViewModelActivator::DoCancel
// 
// ---------------------------------------------------------------------------
//
void CVRRecView::CVRRecViewModelActivator::DoCancel()
    {
    }


// ---------------------------------------------------------------------------
// CVRRecView::NewLC
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CVRRecView* CVRRecView::NewLC( TInt aViewResourceId,
										TInt aModelResourceId )
	{
	CVRRecView* recview = new( ELeave ) CVRRecView;
	CleanupStack::PushL( recview );
	recview->ConstructL( aViewResourceId, aModelResourceId );

	return recview;
	}


// ---------------------------------------------------------------------------
// CVRRecView::~CVRRecView
// 
// ---------------------------------------------------------------------------
//
CVRRecView::~CVRRecView()
	{
	if ( iContainer )
		{
		AppUi()->RemoveFromViewStack( *this, iContainer );
		}

	if ( iActivationContainer )
		{
		AppUi()->RemoveFromViewStack( *this, iActivationContainer );
		}

    delete iContainer;
    delete iActivationContainer;
    delete iModel;
    delete iSendUi;
    delete iModelActivator;
    delete iLaunchService;
    delete iUSBStateHandler;

    if (iUsbWaitDialog)
        {
        TRAP_IGNORE(iUsbWaitDialog->ProcessFinishedL());
        }
    }

// ---------------------------------------------------------------------------
// CVRRecView::ConstructL
// 
// ---------------------------------------------------------------------------
//
void CVRRecView::ConstructL( TInt aViewResourceId, TInt aModelResourceId )
	{
	BaseConstructL( aViewResourceId );

	iModel = new( ELeave ) CVRRecViewModel;
	TResourceReader reader;
	iEikonEnv->CreateResourceReaderLC( reader, aModelResourceId );
	iModel->ConstructFromResourceL( reader );
	CleanupStack::PopAndDestroy(); // reader;

    // Create SendUi (Send as MMS, E-Mail, etc...)
    iSendUi = CSendUi::NewL();

    iModelActivator = new (ELeave) CVRRecViewModelActivator(iModel);

    iUSBStateHandler = CVRUSBStateHanlder::NewL(this);

    if (CVRUSBStateHanlder::IsUsbActive())
        {
        ShowDialogForWaitUSBPluggingOutL();
        AppUi()->Exit();
        }
    }

// ---------------------------------------------------------------------------
// CVRRecView::Id
// 
// ---------------------------------------------------------------------------
//
TUid CVRRecView::Id() const
    {
    const TUid KVRRecViewUID =
        {
        KVRRecorderViewUID
        };

	return KVRRecViewUID;
	}


// ---------------------------------------------------------------------------
// CVRRecView::HandleCommandL
// 
// ---------------------------------------------------------------------------
//
void CVRRecView::HandleCommandL( TInt aCommandId )
	{
	// This command must be acceptable all times.
	if ( aCommandId == ECmdCancelNote )
		{
        iCommand = aCommandId;
		if( iModel )
			{
			iModel->HandleCommandL( aCommandId );			
			}
		return;
		}

	if ( !iModel->CanAcceptCommands() )
		{
		return;
		}
	
	switch ( aCommandId )
		{
		// Don't do anything in case of a Task Swapper command
		case EAknCmdTaskSwapper:
			{
			break;	
			}
		// No operation	
		case ECmdNOP: 
			{
			break;	
			}	
		// Go back to voice memo list view
		case EAknSoftkeyBack:
			{
			AppUi()->HandleCommandL( EEikCmdExit );
			break;
			}
		case ECmdSettings:
			{
			// Saving the volume control state before entering settings
			TBool hasVolumeControl( iModel->HasVolumeControl() );
			// Disabling volume control 
			iModel->EnableVolumeControl( EFalse );
			iContainer->Update();
			
			AppUi()->HandleCommandL( aCommandId );
			
			// Set volume control as it was before settings dialog
			iModel->EnableVolumeControl( hasVolumeControl );
			iContainer->Update();
			break;
			}
	     case ECmdGoToMyClips:
	        {
            delete iLaunchService;
            iLaunchService = NULL;

	        TFileName path;
	        VRUtils::MemoStoreDirectoryL( path );
	        TInt sortMode = 0;

            CAiwGenericParamList* inParams = CAiwGenericParamList::NewLC();
            inParams->AppendL(TAiwGenericParam(EGenericParamDir, TAiwVariant(
                    path)));
            inParams->AppendL(TAiwGenericParam(EGenericParamDir, TAiwVariant(
                    sortMode)));
            iLaunchService = CAknLaunchAppService::NewL(
                    TUid::Uid(0x101F84EB), NULL, inParams);
            CleanupStack::PopAndDestroy(inParams);
            break;
            }
        case ECmdSendVia:
            {
            iModel->SendViaL(iSendUi);
            break;
            }

            // Handles Middle SoftKey presses in this view
        case ECmdMSK:
            {
            // Don't forward the command if dimmed button was clicked or 
            // if model can't handle commands
            if (iModel->ButtonState(iContainer->FocusedButton()) == EDimmed
                    || !iModel->CanHandleCommands())
                {
                break;
                }

            // Fetch the command to execute 
            TInt commandId(iModel->CommandId(iContainer->FocusedButton()));

            iModel->HandleCommandL(commandId);
            AppUi()->HandleCommandL(commandId);

            iContainer->UpdateButtonPanel(EVRUpdateStateChange);
            break;
            }
        case ECmdUSBChange:
            if (iUsbWaitDialog)
                {
                iUsbWaitDialog->ProcessFinishedL();
                }
            iContainer->UpdateButtonPanel(EVRUpdateStateChange);
            break;
        default:
            {
            iModel->HandleCommandL(aCommandId);
            AppUi()->HandleCommandL(aCommandId);
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CVRRecView::DynInitMenuPaneL
// 
// ---------------------------------------------------------------------------
//
void CVRRecView::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
	{

	switch ( aResourceId )
		{
		case R_VR_REC_MENUPANE_DEFAULT:
			{
            // Check if HELP is enabled in FeatureManager, if not, disable the Help menu item
            if (!FeatureManager::FeatureSupported(KFeatureIdHelp))
                {
                aMenuPane->SetItemDimmed(ECmdHelp, ETrue);
                }
            if (CVRUSBStateHanlder::IsUsbActive())
                {
                aMenuPane->SetItemDimmed(ECmdRecord, ETrue);
                aMenuPane->SetItemDimmed(ECmdDelete, ETrue);
                aMenuPane->SetItemDimmed(ECmdRename, ETrue);
                aMenuPane->SetItemDimmed(ECmdGoToMyClips, ETrue);
                }

			AddSendToMenuL( aResourceId, aMenuPane );
			break;
			}

		case R_VR_REC_MENUPANE_VIEWER:
			{
			TUid appuid( KNullUid );
			CEikAppUi* container = iEikonEnv->EikAppUi()->ContainerAppUi();
			if ( container )
				{
                // Check if HELP is enabled in FeatureManager, if not, disable the Help menu item
                if (!FeatureManager::FeatureSupported(KFeatureIdHelp))
                  {
                  aMenuPane->SetItemDimmed(ECmdHelp, ETrue);
                  }

                if (CVRUSBStateHanlder::IsUsbActive())
                    {
                    aMenuPane->SetItemDimmed(ECmdSaveAttachment, ETrue);
                    }

                appuid = container->Application()->AppDllUid();
                if ((appuid == KUidMmsEditor) || (appuid == KUidMailEditor))
                    {
                    aMenuPane->SetItemDimmed(ECmdSaveAttachment, ETrue);
                    }
                else
                    {
                    AddSendToMenuL(aResourceId, aMenuPane);
                    }
                }

			break;
			}
		case R_VR_SETTINGS_MENUPANE:
			{
			// Check if HELP is enabled in FeatureManager, if not, disable the Help menu item
			if (!FeatureManager::FeatureSupported(KFeatureIdHelp))
				{
				aMenuPane->SetItemDimmed(ECmdHelp, ETrue);
				}
            break; 
			}

		case R_VR_REC_MENUPANE_DELETED:
			{
			// Check if HELP is enabled in FeatureManager, if not, disable the Help menu item
  			if (!FeatureManager::FeatureSupported(KFeatureIdHelp))
				{
				aMenuPane->SetItemDimmed(ECmdHelp, ETrue);
				}

            if (CVRUSBStateHanlder::IsUsbActive())
                {
                aMenuPane->SetItemDimmed(ECmdRecord, ETrue);
                aMenuPane->SetItemDimmed(ECmdGoToMyClips, ETrue);

                }

            // if recording already started, not allow to start recording again.    
            if (iModel->GetInRecordingFlag())
                {
                aMenuPane->SetItemDimmed(ECmdRecord, ETrue);
                aMenuPane->SetItemDimmed(ECmdSettings, ETrue);
                iModel->SetInRecordingFlag(EFalse);
                }

            if (CVRUSBStateHanlder::IsUsbActive())
                {
                aMenuPane->SetItemDimmed(ECmdRecord, ETrue);
                aMenuPane->SetItemDimmed(ECmdGoToMyClips, ETrue);
                }

            break;
            }

        case R_VR_REC_MENUPANE_INCALL_RECORDING_DISABLED:
            {
            // Check if HELP is enabled in FeatureManager, if not, disable the Help menu item
            if (!FeatureManager::FeatureSupported(KFeatureIdHelp))
                {
                aMenuPane->SetItemDimmed(ECmdHelp, ETrue);
                }

            if (CVRUSBStateHanlder::IsUsbActive())
                {
                aMenuPane->SetItemDimmed(ECmdGoToMyClips, ETrue);
                }
            break;
            }

		case R_VOREC_GS_SETTINGS_MENUPANE:
			{
			// Check if HELP is enabled in FeatureManager, if not, disable the Help menu item
			if (!FeatureManager::FeatureSupported(KFeatureIdHelp))
				{
				aMenuPane->SetItemDimmed(EAknCmdHelp, ETrue);
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
// CVRRecView::GetSelectedFilesLC
// 
// ---------------------------------------------------------------------------
//
MDesCArray* CVRRecView::GetSelectedFilesLC( TVRSelectionStyle& aStyle ) const
	{
	aStyle = EFocusedItem;
	return iModel->GetSelectedFilesLC();
	}


// ---------------------------------------------------------------------------
// CVRRecView::DoActivateL
// 
// ---------------------------------------------------------------------------
//
void CVRRecView::DoActivateL(const TVwsViewId& aPrevViewId,
        TUid aCustomMessageId, const TDesC8& aCustomMessage)
    {
    TRAP_IGNORE( ReallyDoActivateL( aPrevViewId,
                    aCustomMessageId,
                    aCustomMessage ) );

    }

// ---------------------------------------------------------------------------
// CVRRecView::DoDeactivate
// 
// ---------------------------------------------------------------------------
//
void CVRRecView::DoDeactivate()
	{
	iModel->Deactivate();

	iModel->LeaveContext();

	CAknViewAppUi* appUi = AppUi();
	if ( iContainer )
		{
		appUi->RemoveFromViewStack( *this, iContainer );
		delete iContainer;
		iContainer = NULL;
		}

	if ( iActivationContainer )
		{
		appUi->RemoveFromViewStack( *this, iActivationContainer );
		delete iActivationContainer;
		iActivationContainer = NULL;	
		}

	}


// ---------------------------------------------------------------------------
// CVRRecView::ViewDeactivated
// 
// ---------------------------------------------------------------------------
//
void CVRRecView::ViewDeactivated()
	{	
#ifdef _DEBUG    
	RDebug::Print( _L( "CVRRecView::ViewDeactivated" ));
#endif

    TInt err = 0;

    if (!iModel->IncomingCall() && (iModel->VisualStateId() == EStatePlaying
            || iModel->VisualStateId() == EStateRecording))
        {

        if (iModel->VisualStateId() == EStateRecording && iCommand
                == ECmdCancelNote)
            {
            TRAP_IGNORE (iModel->HandleCommandL( ECmdSaveAttachment ));
            }

        // Is autolock activating? 
        RProperty prop;
        TInt val;
        prop.Get( KPSUidCoreApplicationUIs, KCoreAppUIsAutolockStatus, val );
                
        if ( val == EAutolockOff && //Do not pause when focus is lost because of autolock
             !iModel->GetIfStopCalled())
    		{
    		
		//messy, if it is ACC format, stop it. in the future when Pause is supported, it needs change 
#ifdef    __AAC_ENCODER_PLUGIN 
            if ((iModel->Quality() == EQualityHigh) && (GetStateId()
                    == EStateRecording))
                {
                TRAP_IGNORE (iModel->HandleCommandL( ECmdAutoStopInterrupted ));
                }
            else
                {
                TRAP( err, iModel->HandleCommandL( ECmdPause ) );
                }

#else			
	    		TRAP( err, iModel->HandleCommandL( ECmdPause ) );	
#endif

		
				if( err )
					{	
#ifdef _DEBUG
                RDebug::Print(
                        _L("CVRRecView::ViewDeactivated, PauseError Id: %d"),
                        err);
#endif      
                }
            }

		}
	else
		{
			TRAP_IGNORE( iModel->HandleCommandL( ECmdAutoStopInterrupted ) );		
		}	

	CAknView::ViewDeactivated();
	}


// ---------------------------------------------------------------------------
// CVRRecView::ViewActivatedL
// The model's ActivateL() function is called to invoke any functionality 
// required when the UI is activated (e.g., for In-call recording diabling 
// feature)
// ---------------------------------------------------------------------------
//
void CVRRecView::ViewActivatedL(const TVwsViewId& aPrevViewId,
        TUid aCustomMessageId, const TDesC8& aCustomMessage)
    {
    // dismiss sound player / file manager if it was active so that recording
    // view becomes visible
    delete iLaunchService;
    iLaunchService = NULL;

	iModel->ActivateL();

	CAknView::ViewActivatedL(aPrevViewId, aCustomMessageId, aCustomMessage);
    }


// ---------------------------------------------------------------------------
// CVRRecView::Cba
// 
// ---------------------------------------------------------------------------
//
CEikButtonGroupContainer* CVRRecView::Cba() const
	{
	return CAknView::Cba();
	}


// ---------------------------------------------------------------------------
// CVRRecView::AddSendToMenuL
// Use CSendAppUi to display the Send cascade
// ---------------------------------------------------------------------------
//
void CVRRecView::AddSendToMenuL(TInt aResourceId, CEikMenuPane* aMenuPane)
    {
    // If the focused file is closed content (e.g. a DRM file or a protected
    // MIDI file), the whole Send option mustn't be shown.
    CCommonContentPolicy* ccp = CCommonContentPolicy::NewLC();
    MDesCArray* filenames = iModel->GetSelectedFilesLC();
    if (filenames->MdcaCount() > 0 && !ccp->IsClosedFileL(
            filenames->MdcaPoint(0)))
        {
        // Set proper capabilities
        TSendingCapabilities capabilities(0, KVRSendUiMsgSize,
                TSendingCapabilities::ESupportsAttachments);

        // We want the send menuitem after a specific item
        TInt itemPosForSend(0);
        if (aResourceId == R_VR_REC_MENUPANE_DEFAULT)
            {
            aMenuPane->ItemAndPos(ECmdRename, itemPosForSend);
            }
        else if (aResourceId == R_VR_REC_MENUPANE_VIEWER)
            {
            aMenuPane->ItemAndPos(ECmdSaveAttachment, itemPosForSend);
            }
        itemPosForSend++;
        if (!CVRUSBStateHanlder::IsUsbActive())
            {
            iSendUi->AddSendMenuItemL(*aMenuPane, itemPosForSend,
                    ECmdSendVia, capabilities);
            }
        }
    CleanupStack::PopAndDestroy(2); // ccp, filenames
    }

// ---------------------------------------------------------------------------
// CVRRecView::LayoutChangedL
// 
// ---------------------------------------------------------------------------
//
void CVRRecView::LayoutChangedL( TInt aType )
	{
	if ( iContainer )
	    {
    	iContainer->SetRect( ClientRect() );
    	iContainer->HandleResourceChangeL( aType );
	    }
	}


// ---------------------------------------------------------------------------
// CVRRecView::ReallyDoActivateL
// 
// ---------------------------------------------------------------------------
//
void CVRRecView::ReallyDoActivateL(const TVwsViewId& /*aPrevViewId*/,
        TUid aCustomMessageId, const TDesC8& /*aCustomMessage*/)
    {
    // Don't do anything if activation is ongoing
    if (iActivationContainer)
        {
        return;
        }

    // If container exists, we have already been activated
    // Deactivate needed before new activation.
    if (iContainer)
        {
        DoDeactivate();
        }

	// Display an empty UI container until model has been fully activated
	iActivationContainer = new( ELeave ) CVRRecViewActivationContainer;
	iActivationContainer->ConstructL( ClientRect() );
	CAknViewAppUi* appUi = AppUi();
	appUi->AddToViewStackL( *this, iActivationContainer );
	iActivationContainer->ActivateL();

    // Activate model in correct context asynchronically.
    // iContainer will be activated trough callback after model activation
    TCallBack cb(ActivationCallBack, this);
    iModelActivator->Activate(
            static_cast<TVRRecViewContexts> (aCustomMessageId.iUid), cb);

    // Construct the real container
    iContainer = new (ELeave) CVRRecViewContainer;
    iContainer->ConstructL(ClientRect(), iModel, iModel, iModel, this);
    iContainer->SetKeyObserver(iModel);
    iContainer->SetVolumeChangeObserver(iModel);
    }


// ---------------------------------------------------------------------------
// CVRRecView::SetFileHandle
// 
// ---------------------------------------------------------------------------
//
void CVRRecView::SetFileHandle( RFile& aFile )
	{
	iModel->SetFileHandle( aFile );
	}


// ---------------------------------------------------------------------------
// CVRRecView::GetFile
// 
// ---------------------------------------------------------------------------
//	
RFile& CVRRecView::GetFile()
	{
	return iModel->MemoFileHandle();
	}	


// ---------------------------------------------------------------------------
// CVRRecView::ActivationCallBack
// 
// ---------------------------------------------------------------------------
//
TInt CVRRecView::ActivationCallBack( TAny* aRecView )
	{
	CVRRecView* view = reinterpret_cast< CVRRecView* >( aRecView );
	TRAP_IGNORE( view->ActivateContainerL() );
	return ETrue;	
	}


// ---------------------------------------------------------------------------
// CVRRecView::ActivateContainerL
// 
// ---------------------------------------------------------------------------
//	
void CVRRecView::ActivateContainerL()
	{
	// Activate the actual container and remove
	// the one used during model construction
	CAknViewAppUi* appUi = AppUi();
	appUi->AddToViewStackL( *this, iContainer );	
	iContainer->ActivateL();	

	// Delete the empty container, it's not needed anymore
	if ( iActivationContainer )
		{
		appUi->RemoveFromViewStack( *this, iActivationContainer );
		delete iActivationContainer;
		iActivationContainer = NULL;	
		}

	}

TUint CVRRecView::GetStateId() const
	{
//	    if (iModel)
		return iModel->VisualStateId();
	}


// ---------------------------------------------------------------------------
// CVRRecView::IsEmbedded
// 
// ---------------------------------------------------------------------------
//
TBool CVRRecView::IsEmbedded() const
	{
	
	return iModel->IsEmbedded();
	
	}


// ---------------------------------------------------------------------------
//Returns the active quality setting.
//return Quality setting of the active memo
// ---------------------------------------------------------------------------
TVRQuality CVRRecView::Quality() const
	{
		return iModel->Quality();
	}

TInt CVRRecView::HandleUsbPlugInL()
    {
    if (EStateRecording == iModel->VisualStateId() || EStateRecordingPaused
            == iModel->VisualStateId() || EStatePlaying
            == iModel->VisualStateId() || EStatePlayingPaused
            == iModel->VisualStateId())
        {
        HandleCommandL(ECmdStop);
        }

    if (EStateIdleEmbedded == iModel->VisualStateId() || EStateRecordEmbedded
            == iModel->VisualStateId() || EStateIdleRecordEmbedded
            == iModel->VisualStateId())
        {
        ShowDialogForWaitUSBPluggingOutL();
        AppUi()->Exit();
        }

    HandleCommandL(ECmdUSBChange);
    WaitDialogForUSBPluggingOut(iUsbWaitDialog);
    return KErrNone;
    }

TInt CVRRecView::HandleUsbPlugOutL()
    {
    if (iUsbWaitDialog)
        {
        iUsbWaitDialog->ProcessFinishedL();
        }

    HandleCommandL(ECmdUSBChange);
    return KErrNone;
    }

void CVRRecView::DialogDismissedL(TInt /*aButtonId*/)
    {
    HandleCommandL(ECmdUSBChange);
    }

// ---------------------------------------------------------------------------
// Called by UI Framework when view switches to and from foreground
// ---------------------------------------------------------------------------
/*
void CVRRecView::HandleForegroundEventL( TBool aForeground )
    {
    // If the view is going to background, pause recording for stand alone
    // application mode
    if ( !aForeground && !iModel->IsEmbedded() && iCommand != ECmdCancelNote )
        {
        TInt err(0);
	    if ( !iModel->IncomingCall() && ( iModel->VisualStateId() == EStatePlaying ||iModel->VisualStateId() == EStateRecording ) )
		    {
		//messy, if it is ACC format, stop it. in the future when Pause is supported, it needs change 
#ifdef    __AAC_ENCODER_PLUGIN 
                if  ((iModel->Quality() == EQualityHigh) && (GetStateId() == EStateRecording))
		        {
		        TRAP_IGNORE (iModel->HandleCommandL( ECmdAutoStopInterrupted ));
		        }
                else
    	        {
		        TRAP( err, iModel->HandleCommandL( ECmdPause ) );	    		
    	        }

#else			
	            TRAP( err, iModel->HandleCommandL( ECmdPause ) );	
#endif

	 	        if( err )
			    {	
#ifdef _DEBUG
		        RDebug::Print( _L("CVRRecView::ViewDeactivated, PauseError Id: %d"), err );
#endif		
			     }
		     }
	    else
		     {
		    TRAP_IGNORE( iModel->HandleCommandL( ECmdAutoStopInterrupted ) );		
		     }	
                }

  //   iCommand = 0;

    // Forward to handle default event handling.
    CAknView::HandleForegroundEventL(aForeground);
    }
*/
