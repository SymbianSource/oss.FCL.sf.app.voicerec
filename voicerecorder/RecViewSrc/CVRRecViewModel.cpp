/*
* Copyright (c) 2002 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*     This class is the data model and state machine of the Recorder View.
*     The state machine is defined by a VR_STATEMACHINE resource structure.
*
*/


// INCLUDE FILES

#include <bldvariant.hrh>
#include <e32cmn.h>
#include <ctsydomainpskeys.h>
#include <UsbWatcherInternalPSKeys.h>
#include <usbpersonalityids.h>
#include <barsread.h>
#include <eikenv.h>
#include <eikappui.h>
#include <avkon.rsg>
#include <StringLoader.h>
#include <bautils.h>
#include <avkon.hrh>		// Key definitions
#include <AknGlobalNote.h>
#include <sendui.h>
#include <AknLayoutDef.h>
#include <AknUtils.h>
#include <featmgr.h>
#include <data_caging_path_literals.hrh>
#include <CMessageData.h>

#include <featmgr.h>	// Feature manager
#include <SendUiConsts.h>          // Disable e-mail  
#include "MessagingDomainCRKeys.h" // Disable e-mail variation flag

#include <centralrepository.h> // link against centralrepository.lib
#include "VoiceRecorderInternalCRKeys.h"

#include <voicerecorder.rsg>
#include "CVRMemo.h"
#include "TVRState.h"
#include "CVRMdaRecorder.h"
#include "VRConsts.h"
#include "CVRSystemEventHandler.h"
#include "CVRUSBEventHandler.h"
#include "VRUtils.h"
#include "CVRRecViewModel.h"
#include <csxhelp/vorec.hlp.hrh>


// CONSTANTS
const TInt KAllowInCallRecording( 1 );

// Shown if maximum recording time is not known
_LIT( KVRBeforeFirstEstimateLabel, "--:--:--" );
// MMS/High text is inserted here
_LIT( placeForQuality, "%U" );


// ================= MEMBER FUNCTIONS ========================================
// ---------------------------------------------------------------------------
// CVRRecViewModel::CVRRecViewModel
// 
// ---------------------------------------------------------------------------
//
CVRRecViewModel::CVRRecViewModel()
	: iStateArray( 2 ),
	iLabelSets( 2 ),
	iPreviousCallState( EPSCTsyCallStateNone ),
	iCanHandleCommands( ETrue ),
	iCBAEnabled( ETrue )
	{
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::~CVRRecViewModel
// 
// ---------------------------------------------------------------------------
//
CVRRecViewModel::~CVRRecViewModel()
	{
	// It might be possible to make this method leaving and put it
	// somewhere else than destructor
	SaveVolumeSettings();
	iLabelSets.ResetAndDestroy();
	iStateArray.Reset();

	delete[] iLabelLayoutLinks;
	delete iMemo;
	delete iRecorder;
	if ( iGlobalNote && iIsNoteLaunched )
		{
		// nothing much to do but ignore.
		TRAP_IGNORE( iGlobalNote->CancelNoteL( iNoteId ) );
		}

	delete iGlobalNote;
	
	if (iCurrentCallHandler) 
		{
		delete iCurrentCallHandler;
		}

	delete iCurrentUSBHandler;
	
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::ConstructFromResourceL
// 
// ---------------------------------------------------------------------------
//
void CVRRecViewModel::ConstructFromResourceL( TResourceReader& aReader )
	{

	TRAP_IGNORE( iCurrentCallHandler = CVRSystemEventHandler::NewL() );
	iCurrentCallHandler->Listen( KPSUidCtsyCallInformation, KCTsyCallState, 
								 this );

    // for USB
    iCurrentUSBHandler = CVRUSBEventHandler::NewL();
	iCurrentUSBHandler->Listen( KPSUidUsbWatcher, KUsbWatcherSelectedPersonality,
								 this );


	if ( FeatureManager::FeatureSupported( KFeatureIdKeypadNoVoiceKey ) &&
		FeatureManager::FeatureSupported( 
						KFeatureIdApplicationControllableAudioRouting ) )
		{
		ReadDefaultSpeakerL();
		}
	else
		{
		iIhfState = ETrue;
		}

	LoadVolumeSettingsL();
   // Connecting and initialization:
    CRepository* myRepSession = CRepository::NewLC( KCRUidVoiceRecorder );

    // Get the values from the repository
    User::LeaveIfError( myRepSession->Get( KVRDefaultAudioFormat, 
    									   iVRAudioFormat ) );
    									   
    User::LeaveIfError( myRepSession->Get( KVRAllowInCallRecording, 
    									   iVRAllowInCallRecording ) );
    CleanupStack::PopAndDestroy( myRepSession ); 

	iGlobalNote = CAknGlobalNote::NewL();

	iMemo = new( ELeave ) CVRMemo;
	iMemo->ConstructL( iVRAudioFormat );

	TInt numLabelSets( aReader.ReadInt16() );
	TInt i;

	for ( i = 0; i < numLabelSets; i++ )
		{
		CDesCArray* labelSet;
		TInt numLabels( aReader.ReadInt16() );

		if ( numLabels > 0 )
			{
			// TResourceReader re-reads the length, have to rewind
			aReader.Rewind( sizeof( TInt16 ) );
			labelSet = aReader.ReadDesCArrayL();
			}
		else
			{
			// create an empty array "by hand",
			// TResourceReader is not able to do this
			labelSet = new( ELeave ) CDesCArrayFlat( 1 );
			}

		CleanupStack::PushL( labelSet );
		iLabelSets.AppendL( labelSet );
		CleanupStack::Pop( labelSet);
		}

	TInt linkCount( aReader.ReadInt16() );
	iLabelLayoutLinks = new( ELeave ) TVRLabelLayoutLink[ linkCount ];
	for ( i = 0; i < linkCount; i++ )
		{
		iLabelLayoutLinks[ i ].ReadFromResource( aReader );
		}

	TInt numUiStates( aReader.ReadInt16() );

	TVRState uistate;
	for ( i = 0; i < numUiStates; i++ )
		{
		uistate.ReadFromResource( aReader );
		uistate.iId =  i;
		iStateArray.AppendL( uistate );
		}

	iState = &iStateArray.At( 0 );

	TInt numContexts( aReader.ReadInt16() );
	for ( i = 0; i < numContexts; i++ )
		{
		TInt id( aReader.ReadInt8() );
		TVRContext* context = &iContexts[ id ];
		context->iValid = ETrue;
		context->iInitialTransition.iCommandId = aReader.ReadUint8();
		context->iInitialTransition.iNewState = aReader.ReadUint8();
		context->iInitialTransition.iFunctionId = aReader.ReadUint8();
		context->iInitialTransition.iValid = ETrue;


		TInt numStates( aReader.ReadInt16() );
		for ( TInt j = 0; j < numStates; j++ )
			{
			TInt id( aReader.ReadInt8() );
			TVRContext::TState* state = &context->iStates[ id ];
			state->iValid = ETrue;
			TInt numTransitions( aReader.ReadInt16() );
			__ASSERT_DEBUG( numTransitions <= KVRMaxTransitions,
						User::Panic( KVRPanic, EPanicTooManyTransitions ) );

			for ( TInt k = 0; k < numTransitions; k++ )
				{
				TVRContext::TTransition* trans = &state->iTransitions[ k ];
				trans->iCommandId = aReader.ReadUint8();
				trans->iNewState = aReader.ReadUint8();
				trans->iFunctionId = aReader.ReadUint8();
				trans->iValid = ETrue;
				}
			}
		}

	StringLoader::Load( iDateUsualWithZero, R_QTN_DATE_USUAL_WITH_ZERO );
	StringLoader::Load( iTimeUsualWithZero, R_QTN_TIME_USUAL_WITH_ZERO );
	StringLoader::Load( iTimeDuratLong,	R_QTN_TIME_DURAT_LONG );	
	StringLoader::Load( iTimeDuratMinSecWithZero,
										R_QTN_TIME_DURAT_MIN_SEC_WITH_ZERO );
										
										
	
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::ProcessKeyEventL
// 
// ---------------------------------------------------------------------------
//
TBool CVRRecViewModel::ProcessKeyEventL( const TKeyEvent& aKeyEvent,
										 const TEventCode aType )
	{
	TBool retval( EFalse );
	if ( aType != EEventKey )
		{
		return retval;
		}

	switch ( aKeyEvent.iCode )
		{
		case EKeyPhoneEnd:
			{
			TInt currentState( iCurrentCallHandler->StateL(KPSUidCtsyCallInformation, KCTsyCallState) );
        										
			if ( ( currentState != EPSCTsyCallStateNone ) )
				{
				HandleCommandL( ECmdDisconnectCall );
				retval = EKeyWasNotConsumed;
				}
			else
				{
				// Voice recorder wasn't here =), retval stays EFalse
				iIsEndKey = ETrue;	
				
				// Exit if app is not fully initialized
				if ( !iContextEntered )
					{
					CEikonEnv::Static()->EikAppUi()->
											HandleCommandL( EEikCmdExit );
					}
				}
			break;
			}
		case EKeyBackspace:
			{
			HandleCommandL( ECmdDelete );
			retval = ETrue;
			break;
			}
		case EKeyYes:
			{
			if( aKeyEvent.iRepeats > 0 )
				{
				if( VisualStateId() == EStateRecording ||
						VisualStateId() == EStatePlaying )
					{
					HandleCommandL( ECmdPause );
					}
				}
			break;
			}
		case EKeySide:
            {
            retval = ETrue;
            
            if ( aKeyEvent.iRepeats )
                {
                switch ( iState->iId )
                    {
                    case EStateIdle:
                        {
                        HandleCommandL( ECmdPlay );
                        break;
                        }
                    case EStateRecording:
                    case EStateRecordEmbedded:
                    case EStateRecordingPaused:
                    case EStatePlaying:
                    case EStatePlayingPaused:
                        {
                        HandleCommandL( ECmdStop );
                        break;
                        }
                    case EStateDeleted:
                    default:
                        {
                        retval = EFalse;
                        break;
                        }
                    }
                }
            else
                {
                switch ( iState->iId )
                    {
                    case EStateIdle:
                    case EStateDeleted:
                    case EStateRecordingPaused:
                        {
                        HandleCommandL( ECmdRecord );
                        break;
                        }
                    case EStateRecording:
                    case EStateRecordEmbedded:
                    case EStatePlaying:
                        {
                        HandleCommandL( ECmdPause );
                        break;
                        }
                    case EStatePlayingPaused:
                        {
                        HandleCommandL( ECmdPlay );
                        break;
                        }
                    default:
                        {
                        retval = EFalse;
                        break;
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
	return retval;
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::HandleCommandL
// 
// ---------------------------------------------------------------------------
//
void CVRRecViewModel::HandleCommandL( TInt aCommandId )
	{
	// This command must be acceptable all times.
	if ( aCommandId == ECmdCancelNote )
		{
		if ( iIsNoteLaunched )
			{
			// nothing much to do but ignore.
			TRAP_IGNORE( iGlobalNote->CancelNoteL( iNoteId ) );
			}
		return;
		}

	if ( !CanAcceptCommands() )
		{
		return;
		}

	if ( !CanHandleCommands() )
		{
		return;
		}
	if ( EStateRecording == iState->iId && ECmdRecord == aCommandId ||
			EStateRecording == iState->iId && ECmdRename == aCommandId )
		{
		return;
		}
	
	switch ( aCommandId )
		{
		case ECmdHelp:
			{
			CEikonEnv::Static()->EikAppUi()->HandleCommandL( aCommandId );
			break;
			}	
		case EEikCmdExit:
			{
			CEikonEnv::Static()->EikAppUi()->HandleCommandL( EAknSoftkeyExit);
			break;
			}
		
		case EAknSoftkeyExit:
			{
			CEikonEnv::Static()->EikAppUi()->HandleCommandL( aCommandId );
			break;
			}
		case ECmdSendVia:
			{
			CEikonEnv::Static()->EikAppUi()->HandleCommandL( aCommandId );
			break;
			}
		case ECmdSaveAttachment:
			{
			__ASSERT_DEBUG( iRecorder, User::Panic( KVRPanic, KErrNotFound ) );
			iRecorder->DetachMemo();
			// Save permanently but do not remove the original

			HBufC* label = StringLoader::LoadLC( R_VR_SAVE_WAITNOTE_LABEL );

			iIsNoteLaunched = ETrue;
			iMemo->SavePermanentlyL( iGlobalNote, iNoteId, *label, ETrue );

			if ( iStateInfoObserver )
				{
				iStateInfoObserver->Update();
				}
			iRecorder->AttachToMemoL( iMemo );

			// Remove the waitnote
			iGlobalNote->CancelNoteL( iNoteId );
			iIsNoteLaunched = EFalse;

			CleanupStack::PopAndDestroy( label );
			break;
			}
		// FLOWTHROUGH
		// in case of end key, different command must be given to state
		// machine - recview gives ECmdAutoStopInterrupted
		case ECmdAutoStopInterrupted:
			{
			if ( iIsEndKey )
				{
				aCommandId = ECmdEndKey;
				iIsEndKey = EFalse;
				}
			}
		// Other commands are given to the state machine
		default:
			{
			TVRContext::TTransition transition;
			GetTransition( transition, aCommandId );
			TBool functionOK( ETrue );
			TRAPD( err, functionOK = 
								CallFunctionByIdL( transition.iFunctionId ) );
			
			if ( err != KErrNone )
				{
				// This is done to enable button events
				iCanHandleCommands = ETrue;
				}
				
			// We may have detached memo and we must reattach
			if ( err == KErrInUse )
				{
				__ASSERT_DEBUG( iRecorder, 
								User::Panic( KVRPanic, KErrNotFound ) );
				iRecorder->AttachToMemoL( iMemo );
				}

			if ( err == KErrNotReady && iIsNoteLaunched && iGlobalNote )
				{
				// nothing much to do but ignore.
				TRAP_IGNORE( iGlobalNote->CancelNoteL( iNoteId ) );
				}

			// If the disk gets full while saving, the next state is deleted
			if ( err == KErrDiskFull || err == KErrArgument )
				{
				if ( iContext == EContextRecordNewNormal ||
					 iContext == EContextNormal ||
					 iContext == EContextEmptyNormal )
					{
					ChangeState( EStateDeleted, ETrue );
					}
				else
					{
					// In record new embedded context the UI is left in
					// idle state, so the memo must be attached
					// (EFunctionOK detaches it)
					__ASSERT_DEBUG( iRecorder, 
									User::Panic( KVRPanic, KErrNotFound ) );
					if(!dynamic_cast<CVRMdaRecorder*>(iRecorder)->IsAttachedToMemo())
						{
					    iRecorder->AttachToMemoL( iMemo );	
						}
					}

				if ( err == KErrDiskFull )
					{
					// Maybe an error note should be shown
					return;
					}

				// Propagate to the error handler to show a note.
				User::Leave( err );
				}
			else
				{
				// The LeaveIfError must be here because CallFunctionByIdL
				// might have called Exit() (leaving with -1002) and the
				// framework might be half-dead leading to access violation,
				// if the UI state is changed
				User::LeaveIfError( err );
				if ( functionOK )
					{
					// If the in-call recording disabling feature is active,
					// then the state of the UI will have to reflect that
					TUint newState(transition.iNewState);

					if ( iVRAllowInCallRecording != KAllowInCallRecording )
						{
						TInt currentState( iCurrentCallHandler->StateL(	KPSUidCtsyCallInformation, KCTsyCallState ) );
												
						if ( currentState != EPSCTsyCallStateNone )
							{
							currentState = iCurrentCallHandler->StateL(
												KPSUidCtsyCallInformation, KCTsyCallType );

							switch ( currentState )
								{
								case EPSCTsyCallTypeCSVoice:
								case EPSCTsyCallTypeHSCSD:
								case EPSCTsyCallTypeH324Multimedia:
								case EPSCTsyCallTypeVoIP:
									{
									newState = EStateDisableButtons;										
									break;
									}
								default:
									{
									break;										
									}
								}
														
							}
						}

					// Function didn't want to cancel the state change
					ChangeState( newState );
					}
				}

			break;
			}
		}
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::GetTransition
// 
// ---------------------------------------------------------------------------
//
void CVRRecViewModel::GetTransition( TVRContext::TTransition& aTransition,
									 TUint aCommandId ) const
	{
	__ASSERT_DEBUG( iContexts[ iContext ].iValid, User::Panic( KVRPanic,
					EPanicInvalidContext ) );
	__ASSERT_DEBUG( iContexts[ iContext ].iStates[ iState->iId ].iValid,
					User::Panic( KVRPanic, EPanicInvalidState ) );

	const TVRContext::TState* state =
							&(iContexts[ iContext ].iStates[ iState->iId ]);
	for ( TInt i = 0; i < KVRMaxTransitions; i++ )
		{
		const TVRContext::TTransition* transition = &state->iTransitions[ i ];
		if ( ( transition->iCommandId == aCommandId ) &&
			( transition->iValid ) )
			{
			aTransition = *transition;
			return;
			}
		}

	User::Panic( KVRPanic, EPanicInvalidTransition );
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::GetTransition
// 
// ---------------------------------------------------------------------------
//
void CVRRecViewModel::GetTransition(
								TVRContext::TTransition& aTransition ) const
	{
	aTransition = iContexts[ iContext ].iInitialTransition;
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::ChangeState
// 
// ---------------------------------------------------------------------------
//
void CVRRecViewModel::ChangeState( TUint aNewState, TBool aForced )
	{
	if ( !aForced )
		{
		if ( ( aNewState == iState->iId ) || ( aNewState == EStateInitial ) )
			{
			return;
			}
		}

	iState = &iStateArray.At( aNewState );

	if ( iStateInfoObserver )
		{
		iStateInfoObserver->Update();
		}
	if ( iButtonPanelObserver )
		{
		iResetButtons = ETrue;
		iButtonPanelObserver->Update( EVRUpdateStateChange );
		}
	if ( iDecoratorObserver )
		{
		iDecoratorObserver->Update();
		}
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::EnterContextL
// 
// ---------------------------------------------------------------------------
//
void CVRRecViewModel::EnterContextL( const TVRRecViewContexts& aContext )
	{
	iContextEntered = EFalse;
	if ( iRecorder )
		{
		delete iRecorder;
		iRecorder = NULL;
		}

	CVRMdaRecorder* recorder = new( ELeave ) CVRMdaRecorder();
	CleanupStack::PushL( recorder );
	recorder->ConstructL();
	CleanupStack::Pop();
	iRecorder = recorder;

	iRecorder->RegisterObserver( iStateInfoObserver );
	iRecorder->SetAutoStopObserver( this );

	LeaveContext();
	iContext = aContext;

	iRecorder->AttachToMemoL(iMemo);

	TVRContext::TTransition transition;
	GetTransition( transition );

	ChangeState( transition.iNewState, ETrue );
	CallFunctionByIdL( transition.iFunctionId );

	iContextEntered = ETrue;
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::LeaveContext
// 
// ---------------------------------------------------------------------------
//
void CVRRecViewModel::LeaveContext()
	{
	iContextEntered = EFalse;
	if( iRecorder )
		{
		iRecorder->UnregisterObserver( iStateInfoObserver );
		iRecorder->UnregisterObserver( iButtonPanelObserver );

		iRecorder->DetachMemo();

		iRecorder->RegisterObserver( iStateInfoObserver );
		iRecorder->RegisterObserver( iButtonPanelObserver );
		}
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::CbaResourceId
// 
// ---------------------------------------------------------------------------
//
TInt CVRRecViewModel::CbaResourceId() const
	{
	return iState->iCbaResourceId;
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::GetHelpContext
// 
// ---------------------------------------------------------------------------
//
void CVRRecViewModel::GetHelpContext( TCoeHelpContext& aContext ) const
	{
       if (FeatureManager::FeatureSupported(KFeatureIdHelp))
  	   {   
		  if ( iState->iId == EStateDeleted )
			{
			aContext.iContext = KVOREC_HLP_EMPTY_RECORDER;
			}
		   else
			{
			aContext.iContext = KVOREC_HLP_MAIN;
			}
    	}
	}

// ---------------------------------------------------------------------------
// CVRRecViewModel::SetDecoratorObserver
// 
// ---------------------------------------------------------------------------
//
void CVRRecViewModel::SetDecoratorObserver( MVRObserver* aObserver )
	{
	iDecoratorObserver = aObserver;
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::MenuBarResourceId
// 
// ---------------------------------------------------------------------------
//
TInt CVRRecViewModel::MenuBarResourceId() const
	{
	return iState->iMenubarResourceId;
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::ButtonCount
// 
// ---------------------------------------------------------------------------
//
TInt CVRRecViewModel::ButtonCount() const
	{

	return iState->iButtonCount;
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::CBAEnabled
// from MVRDecoratorModel
// ---------------------------------------------------------------------------
//	
TBool CVRRecViewModel::CBAEnabled() const
	{
	return iCBAEnabled;	
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::ButtonState
// Checks and returns whether the state of the button should be in active or 
// dimmed state. aButtonId tells the index of the button to be checked. Few 
// special cases (end and the beginning of the clip) can be handled here.
// ---------------------------------------------------------------------------
//	
TInt CVRRecViewModel::ButtonState( TInt aButtonId ) const
	{
	// Disable rewind button if there's nothing to rewind
	if ( aButtonId == EButtonRewind &&
		( Position().Int64() / KVRSecondAsMicroSeconds ) < 1 )
		{
		return EDimmed;
		}
	
	// Disable forward button if position is at the end or the length of the
	// clip is smaller than 1	
	if ( aButtonId == EButtonForward && ( Position() == Duration() || 
			( Duration().Int64() / KVRSecondAsMicroSeconds ) < 1 ) )
		{
		return EDimmed;
		}

    // Jeffery: the following ifs are temperary.  they need to be removed after "Pause" is supported in plugin
#ifdef    __AAC_ENCODER_PLUGIN 
    if  ( iState->iId == EStateRecording &&  aButtonId == EButtonRecord  && iMemo->Quality() == EQualityHigh)
		{
		return EDimmed;
		}
#endif
		
	return iState->ButtonState( aButtonId );
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::CommandId
// 
// ---------------------------------------------------------------------------
//
TInt CVRRecViewModel::CommandId( TInt aButtonId ) const
	{
	return iState->ButtonCommandId( aButtonId );
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::InitialFocusButtonId
// 
// ---------------------------------------------------------------------------
//
TInt CVRRecViewModel::InitialFocusButtonId() const
	{
	return iState->iFocusButtonId;
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::SetButtonPanelObserver
// 
// ---------------------------------------------------------------------------
//
void CVRRecViewModel::SetButtonPanelObserver( MVRObserver* aObserver )
	{
	if ( iRecorder )
		{
		if ( iButtonPanelObserver )
			{
			iRecorder->RegisterObserver( aObserver );
			}
		else
			{
			iRecorder->UnregisterObserver( iButtonPanelObserver );
			}
		}
		
		iButtonPanelObserver = aObserver;	
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::ResetNeeded
// 
// ---------------------------------------------------------------------------
//
TBool CVRRecViewModel::ResetNeeded()
	{
	if ( iResetButtons )
		{
		iResetButtons = EFalse;
		return ETrue;		
		}

	return iResetButtons; // EFalse always
	}

	
// ---------------------------------------------------------------------------
// CVRRecViewModel::VisualStateId()
// Returns the id of VR current (visual) state
// ---------------------------------------------------------------------------
//	
TUint CVRRecViewModel::VisualStateId() const
	{
	return iState->iStateId;	
	}

	
// ---------------------------------------------------------------------------
// CVRRecViewModel::CanHandleCommands()
// Returns ETrue if model can handle new commands, EFalse otherwise
// ---------------------------------------------------------------------------
//	
TBool CVRRecViewModel::CanHandleCommands() const
	{
	return iCanHandleCommands;
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::Duration
// 
// ---------------------------------------------------------------------------
//	
TTimeIntervalMicroSeconds CVRRecViewModel::Duration() const
	{
	if ( ( iState->iId == EStateRecording ) ||
		 ( iState->iId == EStateRecordingPaused ) )
		{
		return iMemo->MaxDuration();
		}
	return iMemo->Duration();
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::Position
// 
// ---------------------------------------------------------------------------
//
TTimeIntervalMicroSeconds CVRRecViewModel::Position() const
	{
	if ( ( iState->iId == EStateRecording ) ||
		 ( iState->iId == EStateRecordingPaused ) )
		{
		return iMemo->Duration();
		}
	return iMemo->Position();
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::GetLabel
// Non-leaving version
// ---------------------------------------------------------------------------
//
void CVRRecViewModel::GetLabel( TDes& aName, TInt aLabelIndex ) const
	{
	TRAP_IGNORE( GetLabelL( aName, aLabelIndex ) );
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::GetLabelL
// 
// ---------------------------------------------------------------------------
//
void CVRRecViewModel::GetLabelL( TDes& aName, TInt aLabelIndex ) const
	{
	if ( !iMemo->IsValid() )
		{
		aName.SetLength( 0 );
		return;
		}

	CDesCArray* labels = iLabelSets.At( iState->iLabelSetId );
	User::LeaveIfNull( labels );

	if ( aLabelIndex < labels->Count() )
		{
		TInt index = aLabelIndex;

		TBuf< VRLABELMAXLENGTH > formatStr(
										labels->MdcaPoint( index ) );
		TBuf< VRLABELMAXLENGTH > formattedTime;

		switch ( aLabelIndex )
			{
			case ELabelMemoName:
				{
				//StringLoader::Format( aName, formatStr, -1, iMemo->Name() );
                                aName.Copy(iMemo->Name());
				break;
				}
			case ELabelMemoDate:
				{
				iMemo->DateCreated().FormatL( formattedTime,
											  iDateUsualWithZero );
				StringLoader::Format( aName, formatStr, 0, formattedTime );

				iMemo->DateCreated().FormatL( formattedTime,
											  iTimeUsualWithZero );
				formatStr.Copy( aName );
				StringLoader::Format( aName, formatStr, 1, formattedTime );
				AknTextUtils::LanguageSpecificNumberConversion( aName );
				break;
				}
			case ELabelMemoQuality:
				{
				// Display empty row if feature is not on
                if ( !VRUtils::FeatureEnabled( EVRFeatureShowQualitySetting ) )
                    {
                    aName.Copy( KNullDesC );
                    break;
                    }
				
				aName.Copy( formatStr );
				break;
				}
			case ELabelQualityText:	
				{
					// Display empty row if feature is not on
                if ( !VRUtils::FeatureEnabled( EVRFeatureShowQualitySetting ) )
                    {
                    aName.Copy( KNullDesC );
                    break;
                    }
                    
				formatStr.Copy( placeForQuality );
					
					switch( iMemo->Quality() )
				    {
				    case EQualityMMSOptimized:
				        {
				        StringLoader::Load( formattedTime, R_VOREC_QUALITY_MMS_OPTIMIZED );
				        StringLoader::Format( aName, formatStr, -1, formattedTime );
				        break;
				        }
				        
				        // EQualityNormal is used only when mp4 is supported.  In this case the file is saved as wav.
#ifdef  __AAC_ENCODER_PLUGIN
				    case EQualityNormal:
				        {
				        StringLoader::Load( formattedTime, R_VOREC_QUALITY_NORMAL );				        
				        StringLoader::Format( aName, formatStr, -1, formattedTime );				        
				        break;
				        }
#endif

				    case EQualityHigh:
				        {
				        StringLoader::Load( formattedTime, R_VOREC_QUALITY_HIGH );				        
				        StringLoader::Format( aName, formatStr, -1, formattedTime );				        
				        break;
				        }
				    default:
				        {
				        aName.Copy( KNullDesC );
				        break;
				        }
				    }
				break;
				}
			case ELabelBarMin:
				{
				aName.Copy( KNullDesC );
				break;
				}
			case ELabelBarMax:
				{
				// This label layout is nowadays taking automatically care of 
				// string mirroring in the case of mirrored layout. So use of 
				// AknLayoutUtils::LayoutMirrored() isn't needed anymore
				
				TTimeIntervalMicroSeconds max( 0 );
				if ( ( iState->iId == EStateRecording ) ||
				 ( iState->iId == EStateRecordingPaused ) )
					{
					max = iMemo->MaxDuration();
					}
				else
					{
					max = iMemo->Duration().Int64();

					if ( max.Int64() < 0 )
						{
						max = 0;
						}
					}

				TTime lengthAsTime( max.Int64() );
				TTime positionAsTime( iMemo->Position().Int64() );
				TBuf< VRLABELMAXLENGTH > formattedPosition;

				// current memo quality 
				TVRQuality quality( iMemo->Quality() );
				
				// Show --:--:-- if estimate for maximum recording time is not
				// known yet. Done only when memo quality is High
				if ( ( ( iState->iId == EStateRecording ) || 
					   ( iState->iId == EStateRecordingPaused )) && 
					   ( Position() < KVRFirstEstimateTime ) && 
					   ( quality == EQualityHigh ) &&
					   ( max.Int64() < 0) )
					{
					formattedTime.Append( KVRBeforeFirstEstimateLabel );
					}
				// Show normal maximum time	
				else	
					{
					// Show longer format if time is more than 1 hour
					FormatTimeL( lengthAsTime, formattedTime );		
					}
	
				// Show longer format if time is more than 1 hour
				FormatTimeL( positionAsTime, formattedPosition );
						
				StringLoader::Format( aName, formatStr, 0, formattedPosition );
				formatStr.Copy( aName );
				StringLoader::Format( aName, formatStr, 1, formattedTime );
				AknTextUtils::LanguageSpecificNumberConversion( aName );

				break;						
				}
			case ELabelStatus:
				{
				aName.Copy( iState->iStatus );
				break;
				}
			default:
				{
				break;
				}
			}
		}
	else
		{
		aName.SetLength( 0 );
		}
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::ResourceIdForLabel
// 
// ---------------------------------------------------------------------------
//
TInt CVRRecViewModel::ResourceIdForLabel( TInt aLabelIndex,
										  TAknLayoutId aLayout ) const
	{
	switch ( aLayout )
		{
		case EAknLayoutIdELAF:
		default:
			return iLabelLayoutLinks[ aLabelIndex ].iEuroId;

		case EAknLayoutIdABRW:
			return iLabelLayoutLinks[ aLabelIndex ].iArabicHebrewId;

		case EAknLayoutIdAPAC:
			return iLabelLayoutLinks[ aLabelIndex ].iApacId;

		}

	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::SetStateInfoObserver
// 
// ---------------------------------------------------------------------------
//
void CVRRecViewModel::SetStateInfoObserver( MVRObserver* aObserver )
	{
	iStateInfoObserver = aObserver;
	if( iRecorder )
		{
		iRecorder->RegisterObserver( iStateInfoObserver );
		}
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::SetStateInfoPanelObserver
// Observes changes in state info panel
// ---------------------------------------------------------------------------
//	
void CVRRecViewModel::SetStateInfoPanelObserver( MVRStateInfoObserver* 
																	aObserver )
	{
	iStateInfoPanelObserver = aObserver;
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::HasProgressBar
// 
// ---------------------------------------------------------------------------
//	
TBool CVRRecViewModel::HasProgressBar() const
	{
	return iState->iHasProgressBar;
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::GetSelectedFilesLC
// 
// ---------------------------------------------------------------------------
//
MDesCArray* CVRRecViewModel::GetSelectedFilesLC() const
	{
	CDesCArray* array = new( ELeave ) CDesCArrayFlat( 1 ); // granularity = 1
	CleanupStack::PushL( array );
	array->AppendL( iMemo->Filename() );
	return array;
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::SetVolume
// 
// ---------------------------------------------------------------------------
//
void CVRRecViewModel::SetVolume( TInt aStep, TInt aMaxSteps )
	{
	iIhfVolume = aStep;
    iRecorder->SetVolume( aStep, aMaxSteps );
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::SetMemoName
// 
// ---------------------------------------------------------------------------
//
void CVRRecViewModel::SetMemoName( const TDesC& aFilename )
	{
	iMemo->SetName( aFilename );
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::SetMemoNameNewL
// 
// ---------------------------------------------------------------------------
//
void CVRRecViewModel::SetMemoNameNewL( TBool aEmbedded )
	{
	iMemo->SetTemporaryNameL( aEmbedded );
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::Filename
// 
// ---------------------------------------------------------------------------
//
const TDesC& CVRRecViewModel::Filename() const
	{
	return iMemo->Filename();
	}

void CVRRecViewModel::SetSavingLocationL( const TDesC& aPath )
	{
	iMemo->SetSavingLocationL( aPath );
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::Update
// 
// ---------------------------------------------------------------------------
//
void CVRRecViewModel::Update( TVRUpdateCommand /*aCommand*/ )
	{
	// Error is propagated to error handling method
	TRAPD( err, HandleCommandL( ECmdAutoStopAtEnd ) );

	// Ignore further errors
	TRAP_IGNORE( HandleUpdateErrorL( err ) );
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::IsEmbedded
// 
// ---------------------------------------------------------------------------
//
TBool CVRRecViewModel::IsEmbedded() const
	{
	return ( iContext == EContextRecordNewForRemote )
			|| ( iContext == EContextViewer );
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::CanAcceptCommands
// 
// ---------------------------------------------------------------------------
//
TBool CVRRecViewModel::CanAcceptCommands() const
	{
	return ( iContextEntered != EFalse ) && iRecorder->CanAcceptCommands();
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::SendViaL
// 
// ---------------------------------------------------------------------------
//
void CVRRecViewModel::SendViaL( CSendUi* aSendUi )
	{
	// Don't accept commands during sending operation
	iContextEntered = EFalse;
	
	iRecorder->DetachMemo();
	// Detaching resets the clip position so button panel needs to be updated
	iButtonPanelObserver->Update( EVRUpdatePositionChange );
	
	TSendingCapabilities capabilities
			(0, KVRSendUiMsgSize, TSendingCapabilities::ESupportsAttachments);
	CMessageData* messageData = CMessageData::NewL();
	CleanupStack::PushL( messageData );

	messageData->AppendAttachmentHandleL( iMemo->File() );
	
	// Get an array of the services that should be dimmed
	CArrayFixFlat< TUid >* servicesToDim = SendViaServicesToDimLC();

	// For some reason this function leaves when 'NO' is selected in bluetooth 
	// case's activation question. If this is not Trapped iContextEntered stays
	// EFalse and no commands can be executed
	TRAPD( err, aSendUi->ShowQueryAndSendL( messageData, 
											capabilities, 
											servicesToDim ) );
	
	CleanupStack::PopAndDestroy( servicesToDim );
	CleanupStack::PopAndDestroy( messageData );

	iRecorder->AttachToMemoL( iMemo );
	
	iContextEntered = ETrue;
	
	// This is done so we get possible "Feature not supported" 
	// infonote from the framework
	User::LeaveIfError( err );
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::SendViaServicesToDimLC
// Generates an array of the services (Uids) that should be dimmed from Send
// via menu. Returns a pointer to that array. Configuration in Messaging 
// central repository defines services to dim.
// ---------------------------------------------------------------------------
//	
CArrayFixFlat<TUid>* CVRRecViewModel::SendViaServicesToDimLC()
	{
	CArrayFixFlat< TUid >* servicesToDim = 
									new( ELeave ) CArrayFixFlat< TUid >( 1 );
    CleanupStack::PushL( servicesToDim );
     
    CRepository* repository = 
    					CRepository::NewL( KCRUidMuiuMessagingConfiguration );
    TInt configuration = 0;
    if ( !repository->Get( KMuiuSendUiConfiguration, configuration ) )
        {
        configuration = configuration & KMuiuEmailDisabledInMultimedia;
        }
    delete repository;

  	if ( configuration )
    	{
    	servicesToDim->AppendL( KSenduiMtmSmtpUid );		// normal email
    	servicesToDim->AppendL( KSenduiMtmSyncMLEmailUid );	// sync email
    	}
    	
    return servicesToDim;	
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::CallFunctionByIdL
// 
// ---------------------------------------------------------------------------
//	
/*
IMPLEMENTATION NOTE:
This method needs refactoring with CVRMemo and CVRMdaRecorder.
Probably most of the logic should be transferred to CVRMemo.
*/
TBool CVRRecViewModel::CallFunctionByIdL( TInt aFunctionId )
	{
	switch ( aFunctionId )
		{
		case EFunctionPlay:
			{
			CVRMdaRecorder* iRecorderUtility = NULL;
			iRecorderUtility = dynamic_cast<CVRMdaRecorder*>(iRecorder);
			if ( VRUtils::UnsupportedCallTypeOngoing( ETrue ) || iRecorderUtility == NULL || !iRecorderUtility->IsAttachedToMemo())
				{
				return EFalse;
				}

			iIgnoreNextIhfChange = EFalse;
			SetRecorderVolume();
			SetIHFIfCallNotActive( iIhfState );
			TInt mediaservErr( KErrNone );
			
			// Don't accept CBA commands
			iCBAEnabled = EFalse;
			// Update CBAs
			iDecoratorObserver->Update( EVRUpdateCBA );
			TRAPD( leaveErr, mediaservErr = iRecorder->PlayL() );
			
			// Accept CBAs again. CBAs are updated when state is changed
			iCBAEnabled = ETrue;
			
			if ( leaveErr == KErrNotReady )
				{
				ShowNoteL( R_VR_CORRUPT_INFONOTE_LABEL, EAknGlobalErrorNote );
				return EFalse;
				}

			if ( mediaservErr != KErrNone )
				{
				// cancel the state change
				// the error note is shown in CVRMdaRecorder
				return EFalse;
				}
			User::LeaveIfError( leaveErr );
			break;
			}
		case EFunctionStop:
			{
			// Turn off the IHF after playing
			SetIHFIfCallNotActive( EFalse );
			iRecorder->Stop();
			CheckIhfState();
			break;
			}
		case EFunctionStopWithNote:
			{
			// No need to do anything to IHF here since this is
			// called after recording (IHF is not used)
			iRecorder->Stop();
			ShowNoteL( R_VR_STOP_INFONOTE_LABEL, EAknGlobalInformationNote );
			break;
			}
		case EFunctionStopAndSave:
			{
			// No need to do anything to IHF here since this is
			// called after recording (IHF is not used)
			StopAndSaveL( 0 );
			break;
			}
		case EFunctionAutoStopAndSave:
			{
			// No need to do anything to IHF here since this is
			// called after recording (IHF is not used)
			StopAndSaveL( R_VR_STOP_INFONOTE_LABEL );
			break;
			}
		case EFunctionSilentStopAndSave:
			{
			// No need to do anything to IHF here since this is
			// called after recording (IHF is not used)
			StopAndSaveL( 0 );
			break;
			}

		case EFunctionAutoStop:
			{
			// Turn off the IHF after playing
			SetIHFIfCallNotActive( EFalse );
			iRecorder->Stop();
			CheckIhfState();
			break;
			}
		case EFunctionSaveBeforeExit:
			{
			StopAndCloseL( R_VR_STOP_SAVE_WAITNOTE_LABEL );
			break;
			}
		case EFunctionRecord:
			{
			CVRMdaRecorder* iRecorderUtility = NULL;
			if ( VRUtils::UnsupportedCallTypeOngoing( ETrue ) )
				{
				return EFalse;
				}			
         
			//if ( iRecorder->RecordL() != KErrNone )
			//	{
			//	return EFalse;
			//	}
			
			if(iCurrentCallHandler->StateL(KPSUidCtsyCallInformation,KCTsyCallState )==EPSCTsyCallStateConnected)
				{
				iRecorderUtility = dynamic_cast<CVRMdaRecorder*>(iRecorder);
				if(iRecorderUtility != NULL)
					{
					iRecorderUtility->SetAudioInputL(CAudioInput::EVoiceCall);	
					iRecorderUtility->ConfigSampleRateOfVoiceCallL();
					}
				}
			//Audioinput can't be changed after Record
			if ( iRecorder->RecordL() != KErrNone )
				{
				return EFalse;
				}	
			break;
			}
		case EFunctionPause:
			{
			
			iRecorder->Pause();
			// Turn off the IHF after playing
			SetIHFIfCallNotActive( EFalse );         
			break;
			}
		case EFunctionForward:
			{
			TInt result = iRecorder->ForwardL();

			// The volume setting is lost when the media server client object
			// is stopped.
			SetRecorderVolume();

			if ( result )
				{
				return EFalse;
				}
			break;
			}
		case EFunctionRewind:
			{
			iRecorder->RewindL();
			if ( iButtonPanelObserver )
			    {
			    iButtonPanelObserver->Update();
			    }

			// The volume setting is lost when the media server client object
			// is stopped
			SetRecorderVolume();
			break;
			}
		case EFunctionRecordNew:
			{
			CVRMdaRecorder* iRecorderUtility = NULL;
			if ( VRUtils::UnsupportedCallTypeOngoing( ETrue ) )
				{
				return EFalse;
				}
			iRecorderUtility = dynamic_cast<CVRMdaRecorder*>(iRecorder);
			if(iRecorderUtility == NULL)
				{
				return EFalse;
				}
			iRecorderUtility->SetInRecordingFlag(ETrue);
            
			iCanHandleCommands = EFalse;
			// Don't accept CBA commands
			iCBAEnabled = EFalse;
			// Update CBAs
			iDecoratorObserver->Update( EVRUpdateCBA );
            
		    iRecorder->DetachMemo();
			
			TRAPD( leaveErr1, SetMemoNameNewL( 
					iContext == EContextRecordNewForRemote ) );
			
			// Can't attach if memo name couldn't be generated
			TInt leaveErr2( KErrNone );
			if( !leaveErr1 )
				{
				TRAP( leaveErr2, iRecorder->AttachToMemoL( iMemo ) );	
				}
			
			TBool ifAttach = iRecorderUtility->IsAttachedToMemo();		
	
			if ( leaveErr1 || leaveErr2 || !ifAttach)
				{
				iCanHandleCommands = ETrue;
				iCBAEnabled = ETrue;
				if ( leaveErr1 == KErrDiskFull || leaveErr2 == KErrDiskFull )	
					{
					if ( iMemo->StorageDrive() == EDriveF)
						{
		 				VRUtils::ShowMemoryFullConfirmationQuery(ETrue);
						}
					else
						{
				 		VRUtils::ShowMemoryFullConfirmationQuery();
						}
					}
				}
			
			User::LeaveIfError( leaveErr1 );
			User::LeaveIfError( leaveErr2 );
			if(!ifAttach)
				{
			    User::Leave(KErrNotReady);	
				}
				
			// Indicates that a new memo is attached but not yet recording 
			// started -> handle can be reused and memo can be deleted in exit
			iMemo->SetRecorded( EFalse );
			
			TInt recError( KErrNone );

			//TRAPD( recLeaveErr, recError = iRecorder->RecordL() );
			if(iCurrentCallHandler->StateL(KPSUidCtsyCallInformation,KCTsyCallState )==EPSCTsyCallStateConnected)
				{
				iRecorderUtility->SetAudioInputL(CAudioInput::EVoiceCall);
				iRecorderUtility->ConfigSampleRateOfVoiceCallL();
				}
			//Change for CS call:Audio input can't be changed after Record
            TRAPD( recLeaveErr, recError = iRecorder->RecordL() );
            
            if (GetInRecordingFlag())
            	{
            	iRecorderUtility->SetInRecordingFlag(EFalse);
            	}
            	
			if ( recError != KErrNone || recLeaveErr )
				{
				iCanHandleCommands = ETrue;
				iCBAEnabled = ETrue;
				if ( recLeaveErr )
					{
					User::LeaveIfError( recLeaveErr );					
					}
				else
					{
					return EFalse;	
					}
				}
			
			
			iCanHandleCommands = ETrue;	
			// Accept CBAs again. CBAs are updated when state is changed
			iCBAEnabled = ETrue;
			break;
			}
		case EFunctionDelete:
			{
			iRecorder->DetachMemo();
			// Detaching resets the clip position so button panel 
			// needs to be updated
			iButtonPanelObserver->Update( EVRUpdatePositionChange );
			if ( !iMemo->QueryAndDeleteL() )
				{
				iRecorder->AttachToMemoL( iMemo );
				return EFalse;
				}
			break;
			}
		case EFunctionRename:
			{
			iRecorder->DetachMemo();
			// Detaching resets the clip position so button panel 
			// needs to be updated
			iButtonPanelObserver->Update( EVRUpdatePositionChange );
			if ( iMemo->QueryAndRenameL() )
				{
				if ( iStateInfoObserver )
					{
					iStateInfoObserver->Update();
					}
				}
			iRecorder->AttachToMemoL( iMemo );
			break;
			}
		case EFunctionOk:
			{

			HBufC* label = StringLoader::LoadLC( R_VR_SAVE_WAITNOTE_LABEL );

			iRecorder->DetachMemo();
			iIsNoteLaunched = ETrue;
			iMemo->SavePermanentlyL( iGlobalNote, iNoteId, *label );

			CEikonEnv::Static()->EikAppUi()->HandleCommandL( 
												ECmdEmbeddedRecordingReady );

			iGlobalNote->CancelNoteL( iNoteId );
			iIsNoteLaunched = EFalse;
#ifdef _DEBUG
			RDebug::Print( _L( 
						"VoiceRecorder: CVRRecViewModel Note cancelled" ) );
#endif
			CleanupStack::PopAndDestroy( label );

			HandleCommandL( EEikCmdExit );
			break;
			}
		case EFunctionCancel:
			{
			iRecorder->DetachMemo();
			iMemo->DeleteL();
			HandleCommandL( EEikCmdExit );
			break;
			}
		case EFunctionNop:
			{
			// dummy command, do nothing
			break;
			}
		case EFunctionNRTStop:
			{
			// Turn off the IHF after playing
			SetIHFIfCallNotActive( EFalse );

			iRecorder->Stop();
			CheckIhfState();
			break;
			}
		case EFunctionNRTAutoStop:
			{
			// Turn off the IHF after playing
			SetIHFIfCallNotActive( EFalse );
			CheckIhfState();
			break;
			}
		case EFunctionSetNewPosition:	
			{
			TInt newValue( 0 );
			if ( iStateInfoPanelObserver )
				{
				newValue = iStateInfoPanelObserver->ProgressBarPosition();		
				}

			iRecorder->SetPositionL( newValue );
			
			//Inform button Panel
			if ( iButtonPanelObserver )
			    {
			    iButtonPanelObserver->Update( EVRUpdateStateChange );
			    }
			break;
			}	
		default:
			{
			User::Panic( KVRPanic, EPanicFunctionUnknown );
			break;
			}
		}
		return ETrue;
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::StopAndSaveL
// 
// ---------------------------------------------------------------------------
//
void CVRRecViewModel::StopAndSaveL( TInt aResourceId )
	{
	HBufC* label = NULL;

	// Don't show the note if aResourceId was not defined
	if ( aResourceId )
		{
		label = StringLoader::LoadLC( aResourceId );
		}

	iRecorder->DetachMemo();

	if ( label )
		{
		iIsNoteLaunched = ETrue;
		if(iMemo)
			{
		    iMemo->SavePermanentlyL( iGlobalNote, iNoteId, *label );	
			}
		
		// There is no more note showing in SavePermanentlyL() -function
		// the note is shown here
		ShowNoteL( aResourceId, EAknGlobalInformationNote );
		}
	else
		{
		iIsNoteLaunched = ETrue;
		if(iMemo)
			{
		    iMemo->SavePermanentlyL( iGlobalNote, iNoteId, KNullDesC );
			}
		}

	TRAP_IGNORE(iRecorder->AttachToMemoL( iMemo ));

	iGlobalNote->CancelNoteL( iNoteId );
	iIsNoteLaunched = EFalse;

	if ( aResourceId )
		{
		CleanupStack::PopAndDestroy( label );
		}
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::StopAndCloseL
// 
// ---------------------------------------------------------------------------
//
void CVRRecViewModel::StopAndCloseL( TInt aResourceId )
	{
	HBufC* label = NULL;

	// Don't show the note if aResourceId was not defined
	if ( aResourceId )
		{
		label = StringLoader::LoadLC( aResourceId );
		}

	iRecorder->DetachMemo();

	if ( label )
		{
		iIsNoteLaunched = ETrue;
		if(iMemo)
			{
		    iMemo->SavePermanentlyL( iGlobalNote, iNoteId, *label );	
			}
		}
	else
		{
		iIsNoteLaunched = ETrue;
		if(iMemo)
			{
		    iMemo->SavePermanentlyL( iGlobalNote, iNoteId, KNullDesC );	
			}
		}

	iGlobalNote->CancelNoteL( iNoteId );
	iIsNoteLaunched = EFalse;

	if ( aResourceId )
		{
		CleanupStack::PopAndDestroy( label );
		}
	}

void CVRRecViewModel::ShowNoteL( TInt aResourceId,
								 TAknGlobalNoteType aNoteType )
	{
	HBufC* label = StringLoader::LoadLC( aResourceId );
	iGlobalNote->ShowNoteL( aNoteType, *label );
	CleanupStack::PopAndDestroy( label );
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::GetVolumeControlState
// 
// ---------------------------------------------------------------------------
//
void CVRRecViewModel::GetVolumeControlState( TVRVolumeControlState& aState,
											TInt& aVolume )
	{
	// ignored
	TRAP_IGNORE( ReadDefaultSpeakerL() );
	if ( iState->iHasVolumeControl )
		{
		if ( iIhfState )
			{
			aState = EIhf;

			}
		else
			{
			aState = EEarPiece;
			}
		aVolume = iIhfVolume;
		}
	else
		{
		aState = EDisabled;
		}
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::LoadVolumeSettingsL
// 
// ---------------------------------------------------------------------------
//
void CVRRecViewModel::LoadVolumeSettingsL()
	{
	iIhfVolume = VRUtils::DefaultVolumeL( ESpeakerIhf );
	// Prevent using incorrect values
	if ( iIhfVolume < KVRVolumeControlMin ||
		iIhfVolume > KVRVolumeControlMax )
		{
		iIhfVolume = KVRDefaultVolume;
		}		
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::SaveVolumeSettings
// 
// ---------------------------------------------------------------------------
//
void CVRRecViewModel::SaveVolumeSettings()
	{
	// Ignore all errors, because this will be called in destructor. It isn't 
	// disastrous if new values aren't saved, the previous values will be there
	TRAP_IGNORE( VRUtils::SetDefaultVolumeL( ESpeakerIhf, iIhfVolume ); );
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::HandleSystemEventL
// 
// ---------------------------------------------------------------------------
//
void CVRRecViewModel::HandleSystemEventL()
	{
	// Fetch the changed value from Pubsub
	TInt currentState( iCurrentCallHandler->StateL(KPSUidCtsyCallInformation, KCTsyCallState ) );

	if ( !iActive )
		{
		iPreviousCallState = currentState;
		return;
		}


	// Actions to take when playing
	if ( iState->iId == EStatePlaying )
		{
		switch ( currentState )
			{
			case EPSCTsyCallStateAlerting:
			case EPSCTsyCallStateRinging:
			case EPSCTsyCallStateAnswering:
				{
				HandleCommandL( ECmdAutoStopAtEnd );
				break;
				}
			default:
				{
				break;
				}
			}			
		}

	// Actions to take when recording
	if ( iState->iId == EStateRecording )
		{
		switch ( currentState )
			{
			case EPSCTsyCallStateAlerting:
			case EPSCTsyCallStateRinging:
			case EPSCTsyCallStateAnswering:
			case EPSCTsyCallStateDisconnecting:
				{
				HandleCommandL( ECmdAutoStopAtEnd );
				break;
				}
			case EPSCTsyCallStateNone:
				{
				if( iPreviousCallState == EPSCTsyCallStateConnected )
					{
					CVRMdaRecorder* iRecorderUtility = NULL;
					iRecorderUtility = dynamic_cast<CVRMdaRecorder*>(iRecorder);
					if(iRecorderUtility != NULL)
						{
						iRecorderUtility->SetAudioInputL(CAudioInput::EDefaultMic);					
						}
					HandleCommandL( ECmdAutoStopAtEnd );
					}
				break;										
				}
			default:
				{
				break;
				}
			}			
		}		

	if ( iState->iId == EStateIdle && 
		 currentState == EPSCTsyCallStateNone )
		{
		iIhfState = ETrue;
		}

	/* -------------------------
	 * Disable in-call recording
	 * -------------------------
	 */
	if ( iVRAllowInCallRecording != KAllowInCallRecording )
		{
		if (( iState->iId == EStateIdle || iState->iId == EStateDeleted ) &&
			 currentState == EPSCTsyCallStateAnswering)
			{
			// Disable record button
			ChangeState( EStateDisableButtons, ETrue );
			}
		if (( iState->iId == EStateDisableButtons ) &&
			 ((currentState == EPSCTsyCallStateDisconnecting)|| (currentState == EPSCTsyCallStateNone)))
			{
			if (iContext != EContextRecordNewForRemote)
				{
			// Enable record button
			ChangeState( EStateDeleted, ETrue );
				}
		     else
		        {
		        if (!(iMemo->Duration() > TTimeIntervalMicroSeconds(0)))
		        	{
		        	ChangeState( EStateRecordEmbedded, ETrue );
		        	}
		        else
		        	{
		        	ChangeState( EStateIdleRecordEmbedded, ETrue );
		        	}
		        }
			}
		}
		
	iPreviousCallState = currentState;
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::SetIHFIfCallNotActive
// 
// ---------------------------------------------------------------------------
//
void CVRRecViewModel::SetIHFIfCallNotActive( TBool aIhf )
	{
	if ( FeatureManager::FeatureSupported( 
							KFeatureIdApplicationControllableAudioRouting ) )
		{
		TInt currentState( EPSCTsyCallStateNone );
        // Ignore the error, ihf is not set
		TRAP_IGNORE( currentState = iCurrentCallHandler->StateL( 
									KPSUidCtsyCallInformation, KCTsyCallState ) );

		if ( currentState == EPSCTsyCallStateNone )
			{
			if ( iRecorder->SetIhf( aIhf ) == KErrNone )
				{
				// So that the model knows that the next notify from shared 
				// data client is not a voice key activated IHF change.
				iIgnoreNextIhfChange = ETrue;
				}
			}
		}
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::CheckIhfState
// 
// ---------------------------------------------------------------------------
//
void CVRRecViewModel::CheckIhfState()
	{
	if ( FeatureManager::FeatureSupported( KFeatureIdKeypadNoVoiceKey ) &&
		FeatureManager::FeatureSupported( 
							KFeatureIdApplicationControllableAudioRouting ) )
		{
		if ( iDefaultSpeaker == ESpeakerIhf )
			{
			iIhfState = ETrue;
			}
		else
			{
			iIhfState = EFalse;
			}
		}
	else
		{
		// IHF is default on phones with voice key
		iIhfState = ETrue;
		}
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::SetRecorderVolume
// 
// ---------------------------------------------------------------------------
//
void CVRRecViewModel::SetRecorderVolume()
	{
	TInt activeVolume = iIhfVolume;
	iRecorder->SetVolume( activeVolume, KVRMaxVolumeSteps );
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::ActivateL
// 
// ---------------------------------------------------------------------------
//
void CVRRecViewModel::ActivateL()
	{
	iActive = ETrue;

	if ( FeatureManager::FeatureSupported( KFeatureIdKeypadNoVoiceKey ) &&
		FeatureManager::FeatureSupported( 
				KFeatureIdApplicationControllableAudioRouting ) )
		{
		// The user has visited another view and possibly altered the
		// settings
		ReadDefaultSpeakerL();
		}

	// If the in-call disable recording feature has been enabled, then check 
	// current status and enable/disable UI accordingly
	if ( iVRAllowInCallRecording != KAllowInCallRecording )
		{
		TInt currentState( iCurrentCallHandler->StateL(
			KPSUidCtsyCallInformation, KCTsyCallState ) );
		if ( currentState != EPSCTsyCallStateNone )
			{
			ChangeState (EStateDisableButtons, ETrue);
			}
        }
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::Deactivate
// 
// ---------------------------------------------------------------------------
//
void CVRRecViewModel::Deactivate()
	{
	iActive = EFalse;
	}

// ---------------------------------------------------------------------------
// CVRRecViewModel::ReadDefaultSpeakerL
// 
// ---------------------------------------------------------------------------
//
void CVRRecViewModel::ReadDefaultSpeakerL()
	{
	iDefaultSpeaker = VRUtils::DefaultSpeakerL();

	if ( iDefaultSpeaker == ESpeakerIhf )
		{
		iIhfState = ETrue;
		}
	else
		{
		iIhfState = EFalse;
		}
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::TVRLabelLayoutLink::ReadFromResource
// 
// ---------------------------------------------------------------------------
//
void CVRRecViewModel::TVRLabelLayoutLink::ReadFromResource
										( TResourceReader& aReader )
	{
	iEuroId = aReader.ReadUint32();
	iApacId = aReader.ReadUint32();
	iArabicHebrewId = aReader.ReadUint32();
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::CanSetVolume
// 
// ---------------------------------------------------------------------------
//
TBool CVRRecViewModel::CanSetVolume()
	{
	return ETrue;
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::HandleUpdateErrorL
// 
// ---------------------------------------------------------------------------
//
void CVRRecViewModel::HandleUpdateErrorL( TInt aErr )
	{
	// Show error note.
	if ( aErr == KErrDiskFull )
		{

#ifndef RD_MULTIPLE_DRIVE    
		if ( VRUtils::MemoStoreL() == EMemoStorePhoneMemory )
			{
			ShowNoteL( R_VR_MEMORY_WARNING, EAknGlobalWarningNote );
			}
		else
			{
			ShowNoteL( R_VR_MEMORY_MMC_WARNING, EAknGlobalWarningNote );
			}
			
//multiple drive
#else
		if (VRUtils::MemoDriveL() == VRUtils::DefaultMemoDriveL())
			{
			ShowNoteL( R_VR_MEMORY_WARNING, EAknGlobalWarningNote );
			}
		else
			{
			ShowNoteL( R_VR_MEMORY_MMC_WARNING, EAknGlobalWarningNote );
			}
#endif


		}
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::MemoFileHandle
// 
// ---------------------------------------------------------------------------
//
RFile& CVRRecViewModel::MemoFileHandle()
	{
	return iMemo->File();
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::SetFileHandle
// 
// ---------------------------------------------------------------------------
//	
void CVRRecViewModel::SetFileHandle( RFile& aFile )
	{
	// Use app startup type as embbedding flag
	iMemo->SetFileHandle( aFile, CEikonEnv::Static()->StartedAsServerApp() );
	if ( iRecorder )
		{
		// First detach if already attached
		iRecorder->DetachMemo();
		// The media server error will rise again somewhere where it's
		// easier to handle if needed
		TRAP_IGNORE( iRecorder->AttachToMemoL( iMemo ) );		
		}

	}	


// ---------------------------------------------------------------------------
// CVRRecViewModel::FormatTimeL
// 
// ---------------------------------------------------------------------------
//
void CVRRecViewModel::FormatTimeL( const TTime& aTime, TDes& aString ) const
	{

	// Show longer format if duration more than 1 hour
	if ( aTime.DateTime().Hour() >= 1 )
		{
		aTime.FormatL( aString,
			  iTimeDuratLong );						
		}
	else
		{
		aTime.FormatL( aString,
			  iTimeDuratMinSecWithZero );						
		}	
	}

// ---------------------------------------------------------------------------
// CVRRecViewModel::EnableVolumeControl
// Enables/Disables the navi pane Volume Control drawing in the VR current 
// visual state
// ---------------------------------------------------------------------------
//	
void CVRRecViewModel::EnableVolumeControl( TBool aVolumeControl )
	{
	iState->iHasVolumeControl = aVolumeControl;
	}

// ---------------------------------------------------------------------------
// CVRRecViewModel::HasVolumeControl
// Returns ETrue if volume setting is enabled in this VoiceRecorder state.
// Otherwise returns EFalse
// ---------------------------------------------------------------------------
//
TBool CVRRecViewModel::HasVolumeControl() const
	{
	if( iState->iHasVolumeControl )
		{
		return ETrue;
		}
	return EFalse;	
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::IncomingCall
// Returns ETrue if a phone call is coming.
// Otherwise returns EFalse
// ---------------------------------------------------------------------------
//
TBool CVRRecViewModel::IncomingCall() const
	{
#ifdef _DEBUG
	RDebug::Print( _L("CVRRecViewModel::IncomingCall enter") );
#endif	
	
	// Fetch the current value from Pubsub
	TInt currentState = 0;
	TRAP_IGNORE(currentState = iCurrentCallHandler->StateL( KPSUidCtsyCallInformation, KCTsyCallState ));
	
	if (currentState == EPSCTsyCallStateAlerting ||
		currentState == EPSCTsyCallStateRinging ||
		currentState == EPSCTsyCallStateAnswering )
		{
		return ETrue;
		}
	else
		{
		return EFalse;	
		}	
	}


// ---------------------------------------------------------------------------
//Returns the active quality setting.
//return Quality setting of the active memo
// ---------------------------------------------------------------------------
TVRQuality CVRRecViewModel::Quality() const
	{
		return iMemo->Quality();
	}


// ---------------------------------------------------------
// GetInRecordingFlag
// 
// ---------------------------------------------------------
//		
TBool CVRRecViewModel::GetInRecordingFlag()
	{
	if(iRecorder)
		{
		CVRMdaRecorder* iRecorderUtility = NULL;
		iRecorderUtility = dynamic_cast<CVRMdaRecorder*>(iRecorder);	
		if(iRecorderUtility != NULL)
			{
			return iRecorderUtility->GetInRecordingFlag();
			}
		return EFalse;
		}
	else
		{
		return EFalse;
		}
		
	}
	

// ---------------------------------------------------------
// GetIfStopCalled
// 
// ---------------------------------------------------------
//		
TBool CVRRecViewModel::GetIfStopCalled()
	{
	if(iRecorder)
		{
		CVRMdaRecorder* iRecorderUtility = NULL;
		iRecorderUtility = dynamic_cast<CVRMdaRecorder*>(iRecorder);	
		if(iRecorderUtility != NULL)
			{
			return iRecorderUtility->GetIfStopCalled();
			}
		return EFalse;
		}
	else
		{
		return EFalse;
		}
		
	}

	
// ---------------------------------------------------------
// SetInRecordingFlag
// ---------------------------------------------------------
//
void CVRRecViewModel::SetInRecordingFlag(TBool aFlag)
	{
	if(iRecorder)
		{
		CVRMdaRecorder* iRecorderUtility = NULL;
		iRecorderUtility = dynamic_cast<CVRMdaRecorder*>(iRecorder);	
		if(iRecorderUtility != NULL)
			{
			return iRecorderUtility->SetInRecordingFlag(aFlag);
			}		
		}
	}


// ---------------------------------------------------------------------------
// CVRRecViewModel::HandleUSBEventL
// 
// ---------------------------------------------------------------------------
//
void CVRRecViewModel::HandleUSBEventL()
	{
	// Fetch the changed value from Pubsub
    TInt usbState( 0 );
    RProperty::Get(KPSUidUsbWatcher, KUsbWatcherSelectedPersonality, usbState );
	if ( !iActive )
		{
		return;
		}

	// Actions to take when playing
    if ( usbState == KUsbPersonalityIdMS )
		{
		ShowNoteL( R_QTN_MEMC_VOREC_NOTE1, EAknGlobalInformationNote );		
//  when not support multiple drives
#ifndef RD_MULTIPLE_DRIVE    
		if (iMemo->MemoStore() != EMemoStorePhoneMemory) 
			{
            HandleCommandL( EEikCmdExit);
			}		
//multiple drive
#else
		if (iMemo->StorageDrive() != VRUtils::DefaultMemoDriveL()) 
			{
			
	        HandleCommandL( EEikCmdExit );
			}
#endif			
		}

	}


// End of file
