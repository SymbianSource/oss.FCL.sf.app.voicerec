/*
* Copyright (c) 2005 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class that handles playback of tones in VoiceRec application
*
*/


// INCLUDE FILES

#include <avkon.hrh>
#include <aknappui.h>
#include <AudioPreference.h>
#include <aknsoundsystem.h>
#include <aknSoundinfo.h>

#include "CVRTonePlayer.h"


// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// CVRTonePlayer::CVRTonePlayer
// 
// ---------------------------------------------------------------------------
//
CVRTonePlayer::CVRTonePlayer()
	: iState( EVRToneIdle )
    {
    }


// ---------------------------------------------------------------------------
// CVRTonePlayer::ConstructL
// 
// ---------------------------------------------------------------------------
//
void CVRTonePlayer::ConstructL()
    {
    iPlayer = CMdaAudioToneUtility::NewL( *this );
    
    iSoundSystem = ( static_cast< CAknAppUi* >(
						CEikonEnv::Static()->EikAppUi() )->KeySounds() );
    }


// ---------------------------------------------------------------------------
// CVRTonePlayer::NewL
// 
// ---------------------------------------------------------------------------
//
CVRTonePlayer* CVRTonePlayer::NewL()
    {
    CVRTonePlayer* self = new ( ELeave ) CVRTonePlayer;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CVRTonePlayer::~CVRTonePlayer
// 
// ---------------------------------------------------------------------------
//
CVRTonePlayer::~CVRTonePlayer()
    {
    if ( iPlayer )
        {
        switch ( iPlayer->State() )
            {
            case EMdaAudioToneUtilityNotReady:
                {
                iPlayer->CancelPrepare();
                break;
                }
            case EMdaAudioToneUtilityPlaying:
                {
                iPlayer->CancelPlay();
                break;
                }           
            }
        }
    delete iPlayer;
    
    // if tone play is not finished before, finish waitloop
    if ( iShedulerWait.IsStarted() )
		{
		iShedulerWait.AsyncStop();
		}		
    }


// ---------------------------------------------------------------------------
// CVRTonePlayer::PrepareToneL
// Starts preparing the specified tone, preparing completes with call-back to
// MatoPrepareComplete
// ---------------------------------------------------------------------------
//
void CVRTonePlayer::PrepareToneL( TInt aToneId )
    {
	if ( iState != EVRToneIdle )
		{
		iQueuedTone = aToneId;
		return;
		}

	// Retrieve the tone
	CAknSoundInfo* info = CAknSoundInfo::NewL();
	CleanupStack::PushL( info );
	User::LeaveIfError( iSoundSystem->RequestSoundInfoL(
					                  aToneId, *info ) );
    
    iPreparedTone = aToneId;
    iState = EVRTonePreparing;
    
    iPlayer->PrepareToPlayDesSequence( *info->iSequence );
    CleanupStack::PopAndDestroy( info );
    }


// ---------------------------------------------------------------------------
// CVRTonePlayer::PrepareTone
// Non-leaving version of PrepareToneL
// ---------------------------------------------------------------------------
//
void CVRTonePlayer::PrepareTone( TInt aToneId )
    {
    TRAP_IGNORE( PrepareToneL( aToneId ) );
    }


// ---------------------------------------------------------------------------
// CVRTonePlayer::PlayTone
// Starts playback of specified tone. Completes with call-back to
// MatoPlayComplete
// ---------------------------------------------------------------------------
//
void CVRTonePlayer::PlayTone( TInt aToneId )
    {
    if ( iState != EVRToneIdle || iPreparedTone != aToneId || 
        iPlayer->State() == EMdaAudioToneUtilityPlaying )
    	{
    	// Preparing an unprepared tone synchronously is not supported,
    	// tone playback is skipped
    	return;
    	}
    
    // Preferences need to be set according to tone
    switch( aToneId )
    	{
    	case EAvkonSIDVoiceRecordingStartTone:
    		{
    		iPlayer->SetPriority( KAudioPriorityRecording,
    			TMdaPriorityPreference( KAudioPrefVoiceRecStart ) );
			break;	    		
    		}    
    	case EAvkonSIDVoiceRecordingStopTone:
    		{
    		iPlayer->SetPriority( KAudioPriorityRecording,
    			TMdaPriorityPreference( KAudioPrefVoiceRecStop ) );    		
			break;    	    		    			
    		}
    	default:
    		{
    		break;
    		}
		}
 
    iState = EVRTonePlaying;
    
    iPlayer->Play();
    
    // Do not continue until start tone play is finished
    if( aToneId == EAvkonSIDVoiceRecordingStartTone )
	    {
#ifdef _DEBUG
	    RDebug::Print( _L("VoiceRecorder: Waiting for Start tone play complete") );
#endif	
	    iShedulerWait.Start();
	    }
    }


// ---------------------------------------------------------------------------
// CVRTonePlayer::MatoPrepareComplete
// 
// ---------------------------------------------------------------------------
//
void CVRTonePlayer::MatoPrepareComplete(TInt aError)
	{
	if ( aError )
		{
		iPreparedTone = 0;
		}

	iState = EVRToneIdle;

	// Prepare the next tone in queue
	if ( iQueuedTone )
		{
		TRAP_IGNORE( PrepareToneL( iQueuedTone ) );
		iQueuedTone = 0;
		}
	}
	


// ---------------------------------------------------------------------------
// CVRTonePlayer::MatoPlayComplete
// 
// ---------------------------------------------------------------------------
//
#ifdef _DEBUG
void CVRTonePlayer::MatoPlayComplete(TInt aError )
    {
	RDebug::Print( _L("VoiceRecorder: Tone %d played, error %d"), 
			iPreparedTone, aError );
#else
void CVRTonePlayer::MatoPlayComplete(TInt /*aError*/ )
    {
#endif    	

	iState = EVRToneIdle;

	// Prepare the next tone in queue
	if ( iQueuedTone )
		{
		TRAP_IGNORE( PrepareToneL( iQueuedTone ) );
		iQueuedTone = 0;
		}
	
	// Started only for start-tone	
	if ( iShedulerWait.IsStarted() )
		{
#ifdef _DEBUG
	    RDebug::Print( _L("VoiceRecorder: Start tone play completed -> Recording started") );
#endif		
		// Start tone played -> Recording can be started
		iShedulerWait.AsyncStop();
		}	
			
	}	
