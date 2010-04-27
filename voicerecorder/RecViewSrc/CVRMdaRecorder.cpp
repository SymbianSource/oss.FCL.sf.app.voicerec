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
*     This class acts as a high level wrapper around the media server
*     playback and recording functionalities.
*
*/


// INCLUDE FILES
#include "CVRSystemEventHandler.h"
#include <ctsydomainpskeys.h>

#include <e32svr.h>
#include <bautils.h>
#include <eikenv.h>
#include <aknappui.h>
#include <e32property.h>
#include <AudioPreference.h>
#include <aknsoundsystem.h>
#include <ScreensaverInternalPSKeys.h>
#include <AknGlobalNote.h>
#include <StringLoader.h>
#include <ErrorUI.h>
#include <sysutil.h>

#include <featmgr.h> // Feature manager

#include <voicerecorder.rsg>
#include <VoiceRecorderUID.h>
#include "VoiceRecorderPrivatePSKeys.h"
#include "CVRMemo.h"
#include "MVRObserver.h"
#include "VRConsts.h"
#include "VRUtils.h"
#include "CVRTonePlayer.h"
#include "CVRMdaRecorder.h"
#include "CVRDiskSpaceWatcher.h"
#include "CVRRecViewModel.h"
#include <AudioInput.h> 


// MACROS
#ifdef _DEBUG
#define DEB_PRN_0(str) RDebug::Print(str)
#else
#define DEB_PRN_0(str)
#endif

// CONSTANTS
const TInt KVRMdaRecorderDefaultStep( 5 );
const TInt KVRObserverArrayGranularity( 2 );
const TInt KVRMaxNoteLabelLength( 256 );


// ================= MEMBER FUNCTIONS ========================================

// ---------------------------------------------------------------------------
// CVRMdaRecorder::CVRMdaRecorder
// 
// ---------------------------------------------------------------------------
//
CVRMdaRecorder::CVRMdaRecorder() :
	iObservers( KVRObserverArrayGranularity ),
	iIsAttachedToMemo( EFalse ),
	iMediaServerError( KErrNone ),
	iDimLight (EFalse),
	iAttachingToMemo(EFalse),
	ifInRecording(EFalse),
	ifStopCalled(EFalse)
	{
	}


// ---------------------------------------------------------------------------
// CVRMdaRecorder::~CVRMdaRecorder
// 
// ---------------------------------------------------------------------------
//
CVRMdaRecorder::~CVRMdaRecorder()
	{

	delete iGlobalNote;
	delete iAudioInput; 
	delete iAudioRecorder;
	delete iErrorUI;
	delete iTimer;
	delete iTonePlayer;
	delete iLight;
	delete iDiskSpaceWatcher;
    iObservers.Close();
    
    if (iCurrentCallHandler)
    	{
    	delete iCurrentCallHandler;
    	}
	}


// ---------------------------------------------------------------------------
// CVRMdaRecorder::ConstructL
// 
// ---------------------------------------------------------------------------
//
void CVRMdaRecorder::ConstructL()
	{
	_LIT_SECURITY_POLICY_S0( KVRWritePolicy, KVoiceRecorderAppUID3 );
	_LIT_SECURITY_POLICY_PASS( KVRReadPolicy );
    // Connect to Pub&Sub:
    // Error ignored, key may be already defined which is ok for us
	iPropVRState.Define( KPSUidVoiceRecorder, KVoiceRecorderMode, 0,
	                     KVRReadPolicy, KVRWritePolicy );    
    
    User::LeaveIfError(
        iPropVRState.Attach(
        KPSUidVoiceRecorder, KVoiceRecorderMode ) );
        
   	// To allow/don't allow screensaver
   	// Errors ignored, no actions needed if API is not available
    iPropScreenSaver.Attach( KPSUidScreenSaver,
       KScreenSaverAllowScreenSaver );
	iAudioRecorder = CMdaAudioRecorderUtility::NewL( *this, NULL,
	  KAudioPriorityRecording, TMdaPriorityPreference( 
                               KAudioPrefRealOneLocalPlayback ) );

	iGlobalNote = CAknGlobalNote::NewL();

	iErrorUI = CErrorUI::NewL();

	iTonePlayer = CVRTonePlayer::NewL();
	iTonePlayer->PrepareToneL( EAvkonSIDVoiceRecordingStartTone );
	
	iLight = CHWRMLight::NewL(this);

	// start observe diskdrive
    RFs& fs( CEikonEnv::Static()->FsSession() );
 
    iDiskSpaceWatcher = CVRDiskSpaceWatcher::NewL(*this, fs);	

	}


// ---------------------------------------------------------------------------
// CVRMdaRecorder::MoscoStateChangeEvent
// 
// ---------------------------------------------------------------------------
//
void CVRMdaRecorder::MoscoStateChangeEvent( CBase* /*aObject*/,
										    TInt aPreviousState,
											TInt aCurrentState,
											TInt aErrorCode )
	{
#ifdef _DEBUG
	RDebug::Print( _L( "VoiceRecorder: MoscoStateChangeEvent() prev: %d, current: %d, error: %d" ), 
			aPreviousState, aCurrentState, aErrorCode );
#endif
	iMediaServerError = aErrorCode;
	
   	TRAP_IGNORE(CreateAudioInputL());
   

	// Stop the active scheduler wait (if present)
	if ( iActiveWait.IsStarted() )
		{
		iActiveWait.AsyncStop();
		}

	if (iMediaServerError != KErrNone ) 
		{
        
        // when mmc card is used and unpluged, this error returned
        if (iMediaServerError == KErrCorrupt || iMediaServerError == KErrNotReady)
        	{
        	CVRRecViewModel* iViewModel = NULL;
        	//iAudioRecorder->Close();
        	iViewModel = dynamic_cast <CVRRecViewModel*>(iAutoStopObserver);
        	if(iViewModel != NULL)
        		{
        		iViewModel->HandleCommandL(EEikCmdExit);
        		}
        	else
        		{
        		CEikonEnv::Static()->EikAppUi()->HandleCommandL( EAknSoftkeyExit);
        		}
        	}
 		
		// Media server reports KErrDied if the playing was interrupted
		// by some other system sound triggered by for example incoming SMS

        if (aPreviousState == CMdaAudioClipUtility::ERecording || aPreviousState == CMdaAudioClipUtility::EPlaying )
        	{ 	

        	//if ( iMediaServerError != KErrDied &&  iMediaServerError != KErrAccessDenied )
        	if ( iMediaServerError != KErrDied &&  
        		 iMediaServerError != KErrAccessDenied &&  
        		 iMediaServerError != KErrInUse )
				{
				TBuf< KVRMaxNoteLabelLength > label;

				// If showing of error note fails, then.. well it fails
				if ( iMediaServerError == KErrNotSupported )
					{
					CCoeEnv::Static()->
					ReadResourceAsDes16( label, R_VR_UNSUPPORTED_INFONOTE_LABEL );
				
					TRAP_IGNORE( iGlobalNote->
					ShowNoteL( EAknGlobalErrorNote, label ) );
				
					}
				else if ( iMediaServerError == KErrCorrupt )
					{
					CCoeEnv::Static()->
					ReadResourceAsDes16( label, R_VR_CORRUPT_INFONOTE_LABEL );
				
					TRAP_IGNORE( iGlobalNote->ShowNoteL( EAknGlobalErrorNote, 
													 label ) );
					}

				else
					{
					TRAP_IGNORE( iErrorUI->ShowGlobalErrorNoteL( iMediaServerError ) );
					}

				}
        	    if(iMediaServerError == KErrInUse && aPreviousState == CMdaAudioClipUtility::EPlaying)
        	    	{
					CVRRecViewModel* iViewModel = NULL;
					iViewModel = dynamic_cast <CVRRecViewModel*>(iAutoStopObserver);
					if(iViewModel != NULL)
            		{
            		iViewModel->HandleCommandL(ECmdPause);
            		}
        	        return;
        	    	}
        	    else
        	    	{
					StopAndNotify();
        	    	}
        	}
        
        //it will happen when the openfile fails
        if (aPreviousState == CMdaAudioClipUtility::ENotReady && aCurrentState == CMdaAudioClipUtility::ENotReady )
        	{
#ifdef _DEBUG        	
        	RDebug::Print( _L( "VoiceRecorder: callback stop called" ) );
#endif	    	
	    	iAudioRecorder->Close();				
        	}
        
#ifdef _DEBUG
		if ( aCurrentState == CMdaAudioClipUtility::ERecording)
			{
	        RDebug::Print( _L( "VoiceRecorder: ERecording" ) );
			}
#endif

		} // if ( iMediaServerError != KErrNone )

	if ( aCurrentState == CMdaAudioClipUtility::ERecording ||
		 aCurrentState == CMdaAudioClipUtility::EPlaying )
		{
	   	// Do not allow screen saver while playing
   	    // Errors ignored, no actions needed if API is not available	   	
	   	iPropScreenSaver.Set( KPSUidScreenSaver,
			KScreenSaverAllowScreenSaver, EVRScreenSaverNotAllowed );
		}

	if ( aCurrentState == CMdaAudioClipUtility::EOpen )
		{
		switch ( aPreviousState )
			{
			case CMdaAudioClipUtility::ENotReady:
				{
				iMemo->UpdateModifiedDate();
				if ( iAttachingToMemo )
				    {
				    iAttachingToMemo = EFalse;
				    TRAP_IGNORE( ConfigureMemoL());
				    }
				break;
				}
			default:
				{
				StopAndNotify();
				break;
				}
			}
		}
    DEB_PRN_0( _L( "CVRMdaRecorder::MoscoStateChangeEvent::Exit"));
	}


// ---------------------------------------------------------------------------
// CVRMdaRecorder::RegisterObserver
// 
// ---------------------------------------------------------------------------
//
void CVRMdaRecorder::RegisterObserver( MVRObserver* aObserver )
	{
	if ( aObserver && iObservers.Find( aObserver ) == KErrNotFound )
		{
		iObservers.Append( aObserver );
		}
	}


// ---------------------------------------------------------------------------
// CVRMdaRecorder::UnregisterObserver
// 
// ---------------------------------------------------------------------------
//
void CVRMdaRecorder::UnregisterObserver( MVRObserver* aObserver )
	{
	if ( !aObserver )
		{
		return;
		}
		
	TInt index( iObservers.Find( aObserver ) );
	if ( index != KErrNotFound )
		{
		iObservers.Remove( index );
		}
	}


// ---------------------------------------------------------------------------
// CVRMdaRecorder::SetAutoStopObserver
// 
// ---------------------------------------------------------------------------
//
void CVRMdaRecorder::SetAutoStopObserver( MVRObserver* aObserver )
	{
	iAutoStopObserver = aObserver;
	}


// ---------------------------------------------------------------------------
// CVRMdaRecorder::AttachToMemoL
// Opens a memo file.
// If the specified file does not exist (yet), it is created. 
// ---------------------------------------------------------------------------
//
void CVRMdaRecorder::AttachToMemoL( CVRMemo* aMemo )
	{
#ifdef _DEBUG
     RDebug::Print( _L( "CVRMdaRecorder::AttachToMemoL -enter" ));      	
#endif

     // if error happen, attach will fail 
     	
	if( iIsAttachedToMemo )
		{
		User::Panic( KVRPanic,EPanicAlreadyAttached );
		}

	if( !aMemo )
		{
		User::Panic( KVRPanic, KErrNotFound );
		}

	iMemo = aMemo;

    // if the mmc card is full, should not attach to it

     RFs& fs( CEikonEnv::Static()->FsSession() );


// old storage system
#ifndef RD_MULTIPLE_DRIVE
	if ( iMemo->MemoStore() == EMemoStorePhoneMemory )
		{
#ifdef  __AAC_ENCODER_PLUGIN
		if (((iMemo->Quality() != EQualityHigh) && SysUtil::FFSSpaceBelowCriticalLevelL( &fs, KVRRecStartMemBuf )) || 
		((iMemo->Quality() == EQualityHigh) && SysUtil::FFSSpaceBelowCriticalLevelL( &fs, KVRAACCriticalMemoryLevel )))
#else
        // now AAC format is not enabled
        if (SysUtil::FFSSpaceBelowCriticalLevelL( &fs, KVRRecStartMemBuf ))
#endif
        	{
 			VRUtils::ShowMemoryFullConfirmationQuery();
			if(GetInRecordingFlag())
				{
				SetInRecordingFlag(EFalse);	
				}       		
        	}
			
		}
		
     // MMC card
        else
     	{
#ifdef  __AAC_ENCODER_PLUGIN
		if (((iMemo->Quality() != EQualityHigh) && SysUtil::MMCSpaceBelowCriticalLevelL( &fs, KVRRecStartMemBuf )) || 
		((iMemo->Quality() == EQualityHigh) && SysUtil::MMCSpaceBelowCriticalLevelL( &fs, KVRAACCriticalMemoryLevel )))
#else
        // now AAC format is not enabled
        if (SysUtil::FFSSpaceBelowCriticalLevelL( &fs, KVRRecStartMemBuf ))
#endif
        	{
			VRUtils::ShowMemoryFullConfirmationQuery(ETrue);

			if(GetInRecordingFlag())
				{
				SetInRecordingFlag(EFalse);	
				}       		
        	}     		
     	}

// now multiple drives
#else

#ifdef  __AAC_ENCODER_PLUGIN
		if (((iMemo->Quality() != EQualityHigh) && SysUtil::DiskSpaceBelowCriticalLevelL( &fs, KVRRecStartMemBuf, iMemo->StorageDrive())) ||
		((iMemo->Quality() == EQualityHigh) && SysUtil::DiskSpaceBelowCriticalLevelL( &fs, KVRAACCriticalMemoryLevel, iMemo->StorageDrive())))		 
#else
        // now AAC format is not enabled
        if (SysUtil::DiskSpaceBelowCriticalLevelL( &fs, KVRRecStartMemBuf, iMemo->StorageDrive()))
#endif 

			{
			//mmc card			
			if ( iMemo->StorageDrive() == EDriveF)
				{
 				VRUtils::ShowMemoryFullConfirmationQuery(ETrue);
				}
			else
				{
		 		VRUtils::ShowMemoryFullConfirmationQuery();
				}
			if(GetInRecordingFlag())
				{
				SetInRecordingFlag(EFalse);	
				}			
			}


#endif
	
	// opens an existing file or creates new
	iAudioRecorder->OpenFileL( iMemo->File() );
	
	iAttachingToMemo = ETrue;
		
	if( !iActiveWait.IsStarted() )
		{
#ifdef _DEBUG
		RDebug::Print( _L( "CVRMdaRecorder::AttachToMemoL::AWaitStarting..."));
#endif
		iActiveWait.Start(); // CSI: 10 #

#ifdef _DEBUG
		RDebug::Print( _L( "CVRMdaRecorder::AttachToMemoL::AWResuming...[%d]"), iMediaServerError);
#endif
		}    
       
	User::LeaveIfError( iMediaServerError );
		


/*
	if( !iActiveWait.IsStarted() )
		{
		RDebug::Print( _L( "CVRMdaRecorder::AttachToMemoL::AWaitStarting..."));
		iActiveWait.Start(); // CSI: 10 #
		RDebug::Print( _L( "CVRMdaRecorder::AttachToMemoL::AWResuming...[%d]"), iMediaServerError);
		}    
    
	User::LeaveIfError( iMediaServerError );

    // Configure bitrate if recording an AMR clip.
    // Default bitrate is 12200 and it's used if codec doesn't support
    // the bitrate that is retrieved from Central Repository.
    if (iMemo->Quality() == EQualityMMSOptimized )
        {
        TUint configuredAmrBitrate( VRUtils::AMRBitrateL() );
        TUint amrBitrate( KVRDefaultAmrBitrate );
        
        // Fetch supported bitrates and find out if configured bitrate
        // is supported
        RArray<TUint> rates;
        iAudioRecorder->GetSupportedBitRatesL( rates );        
        TInt num = rates.Count();
        if(num > 0)
        	{
            rates.Sort();
            TInt found = rates.Find(configuredAmrBitrate);
            if (found == KErrNotFound)
            	{
            		amrBitrate = rates[num-1];
            	}
        	}
        rates.Reset();  // Clear array      
        iAudioRecorder->SetDestinationBitRateL( amrBitrate );                  
        }


// added for supporting mp4 format
#ifdef __AAC_ENCODER_PLUGIN
  else if (iMemo->Quality() == EQualityHigh)
    	{

         TInt configuredAacSamplerate( VRUtils::AACSamplerateL() );
         TInt aacSampleRate( KVRDefaultSamplerateAac );
 
         TInt configuredAacBitrate( VRUtils::AACBitrateL() );
         TInt aacBitrate( KVRDefaultBitrateAac );

         TInt configuredAudioMode( VRUtils::AACAudioModeL() );   
 
         // Sample rate
     	 RArray<TUint> supportedSampleRates;
#ifdef _DEBUG
      RDebug::Print( _L( "CVRMdaRecorder::AttachToMemoL-GetSupportedSampleRatesL -before" ));      	
#endif
         iAudioRecorder->GetSupportedSampleRatesL(supportedSampleRates);        

#ifdef _DEBUG
      RDebug::Print( _L( "CVRMdaRecorder::AttachToMemoL-GetSupportedSampleRatesL -after" ));      	
#endif

        TInt num1 = supportedSampleRates.Count();
        if(num1 > 0)
        	{
            supportedSampleRates.Sort();
            TInt found1 = supportedSampleRates.Find(configuredAacSamplerate);
            if (found1 == KErrNotFound)
            	{
            		aacSampleRate = supportedSampleRates[num1-1];
            	}
        	}

          supportedSampleRates.Reset();                           
#ifdef _DEBUG
      RDebug::Print( _L( "CVRMdaRecorder::AttachToMemoL-SetDestinationSampleRateL -before, sampleRate is %d"), aacSampleRate);      	
#endif
          iAudioRecorder->SetDestinationSampleRateL(aacSampleRate);

#ifdef _DEBUG
      RDebug::Print( _L( "CVRMdaRecorder::AttachToMemoL-SetDestinationSampleRateL -after" ));      	
#endif


         // Bit reate      
         RArray<TUint> supportedBitRates;         

#ifdef _DEBUG
      RDebug::Print( _L( "CVRMdaRecorder::AttachToMemoL-GetSupportedBitRatesL -before" ));      	
#endif
         iAudioRecorder->GetSupportedBitRatesL(supportedBitRates);

#ifdef _DEBUG
      RDebug::Print( _L( "CVRMdaRecorder::AttachToMemoL-GetSupportedBitRatesL -after" ));      	
#endif

        TInt num2 = supportedBitRates.Count();
        if(num2 > 0)
        	{
            supportedBitRates.Sort();
            TInt found2 = supportedBitRates.Find(configuredAacBitrate);
            if (found2 == KErrNotFound)
            	{
            		aacBitrate = supportedBitRates[num2-1];
            	}
        	}

         supportedBitRates.Reset();

#ifdef _DEBUG 
        RDebug::Print( _L( "CVRMdaRecorder::AttachToMemoL-SetDestinationBitRateL -before, bitrate is %d"), aacBitrate);      	
#endif
         iAudioRecorder->SetDestinationBitRateL(aacBitrate);
         
#ifdef _DEBUG 
      RDebug::Print( _L( "CVRMdaRecorder::AttachToMemoL-SetDestinationBitRateL -after" ));      	
#endif
         
         // the audio mode flag                                    

#ifdef _DEBUG
      RDebug::Print( _L( "CVRMdaRecorder::AttachToMemoL-GetSupportedNumberOfChannelsL -before" ));      	
#endif
         RArray<TUint> supportedNumberOfChannels;       
         iAudioRecorder->GetSupportedNumberOfChannelsL(supportedNumberOfChannels); 
         TInt num3 = supportedNumberOfChannels.Count();
         if (num3 > 0)
         	{
         	supportedNumberOfChannels.Sort();
            TInt ifFound = supportedNumberOfChannels.Find(configuredAudioMode);
            if (ifFound == KErrNotFound)
         	    {
         	     configuredAudioMode = supportedNumberOfChannels [0];	
         	    }
         	}
         supportedNumberOfChannels.Reset();
         iAudioRecorder->SetDestinationNumberOfChannelsL(configuredAudioMode);
        
    	}
#endif
 	    iIsAttachedToMemo = ETrue;
	    ResetPosition();	
*/

#ifdef _DEBUG
    RDebug::Print( _L( "CVRMdaRecorder::AttachToMemoL -exit" ));      	
#endif
	}


// ---------------------------------------------------------------------------
// CVRMdaRecorder::CreateNewMemoL
// 
// ---------------------------------------------------------------------------
//
void CVRMdaRecorder::CreateNewMemoL( CVRMemo* aMemo )
	{
	iMemo = aMemo;
	iAudioRecorder->OpenFileL( aMemo->File() );
	if ( !iActiveWait.IsStarted() )
		{
		iActiveWait.Start(); // CSI: 10 #
		}    
	User::LeaveIfError( iMediaServerError );
	}


// ---------------------------------------------------------------------------
// CVRMdaRecorder::DetachMemo
// 
// ---------------------------------------------------------------------------
//
void CVRMdaRecorder::DetachMemo()
	{
	if ( iIsAttachedToMemo )
		{
		Stop();
		//Need to delete iAudioInput before close!!!
	    	DeleteAudioInput();
		
	    	iAudioRecorder->Close();				

		iMemo = NULL;
		iIsAttachedToMemo = EFalse;
		}
	}


// ---------------------------------------------------------------------------
// CVRMdaRecorder::SetVolume
// 
// ---------------------------------------------------------------------------
//
void CVRMdaRecorder::SetVolume( TInt aStep, TInt aMaxSteps )
	{
	if ( iIsAttachedToMemo )
		{
		// On device, MaxVolume() typically returns 9.
		TInt maxVolume( iAudioRecorder->MaxVolume() );
		TInt volume = maxVolume * aStep / aMaxSteps;
		
		// Just to make sure that volume is between 0 .. MaxVolume
		if ( volume < 0)
			{
			volume = 0;
			}
		else if ( volume > maxVolume )
			{
			volume = maxVolume;
			}
			
#ifdef _DEBUG
		RDebug::Print( _L( "VoiceRecorder: Set volume: %d, max volume: %d" ),
						 volume, maxVolume  );
#endif			
		iAudioRecorder->SetVolume( volume );
		}
	}


// ---------------------------------------------------------------------------
// CVRMdaRecorder::PlayL
// 
// ---------------------------------------------------------------------------
//
TInt CVRMdaRecorder::PlayL()
	{
	__ASSERT_DEBUG( iIsAttachedToMemo, User::Panic( KVRPanic,
		EPanicNotAttached ) );

	if( iPause )
		{
		iAudioRecorder->SetPosition( iPosition );
		iPosition = 0;
		iPause = EFalse;
		}

	// This line was added to prevent MMF crash, remove it when play works 
	// without this
	iAudioRecorder->SetPlaybackBalance( KMMFBalanceCenter );
	
	SetIfStopCalled(EFalse);
	iAudioRecorder->PlayL();

    User::LeaveIfError( iPropVRState.Set( KPSUidVoiceRecorder,
        KVoiceRecorderMode, EVRPlaying ) );

   	// Do not allow screen saver while playing
   	// Errors ignored, no actions needed if API is not available   	
   	iPropScreenSaver.Set( KPSUidScreenSaver,
		KScreenSaverAllowScreenSaver, EVRScreenSaverNotAllowed );

	if( !iActiveWait.IsStarted() )
		{
		iActiveWait.Start(); // CSI: 10 #
		}    
	StartTimerL();
	return iMediaServerError;
	}

// ---------------------------------------------------------------------------
// CVRMdaRecorder::RecordL
// Starts the recording of a sound clip. Checks the memory space situation
// and plays start tone before starting recording.
// ---------------------------------------------------------------------------
//
TInt CVRMdaRecorder::RecordL()
	{
	// If we receive record command and file is not opened yet. wait for file
	// to be opened and then resume.
	if ( !iIsAttachedToMemo && iAttachingToMemo && !iActiveWait.IsStarted() )
		{
		iActiveWait.Start();
		User::LeaveIfError( iMediaServerError );
	    }
	
	__ASSERT_DEBUG( iIsAttachedToMemo, User::Panic( KVRPanic,
														EPanicNotAttached ) );
    TBool wasPaused( iPause );
	iPause = EFalse;

	// Check if available memory is/goes below critical level 
	// in that case record is not performed
	// KVRRecStartMemBuf should be size of 60 sec AMR and 5 sec WAV
	// New max time estimation for WAV is calculated after 5 secs
	RFs& fs( CEikonEnv::Static()->FsSession() );
	
	TBool critical( EFalse );

// the current storage system (phone memory and mmc card)
#ifndef RD_MULTIPLE_DRIVE
	if ( iMemo->MemoStore() == EMemoStorePhoneMemory )
		{
#ifdef  __AAC_ENCODER_PLUGIN
		if (((iMemo->Quality() != EQualityHigh) && SysUtil::FFSSpaceBelowCriticalLevelL( &fs, KVRRecStartMemBuf )) || 
		((iMemo->Quality() == EQualityHigh) && SysUtil::FFSSpaceBelowCriticalLevelL( &fs, KVRAACCriticalMemoryLevel )))
#else
        // now AAC format is not enabled
        if (SysUtil::FFSSpaceBelowCriticalLevelL( &fs, KVRRecStartMemBuf ))
#endif 	    
			{
			VRUtils::ShowMemoryFullConfirmationQuery();
			critical = ETrue;
			}
		}

     // MMC card
     else
     	{
#ifdef  __AAC_ENCODER_PLUGIN
		if (((iMemo->Quality() != EQualityHigh) && SysUtil::MMCSpaceBelowCriticalLevelL( &fs, KVRRecStartMemBuf )) || 
		((iMemo->Quality() == EQualityHigh) && SysUtil::MMCSpaceBelowCriticalLevelL( &fs, KVRAACCriticalMemoryLevel )))
#else
        // now AAC format is not enabled
        if (SysUtil::MMCSpaceBelowCriticalLevelL( &fs, KVRRecStartMemBuf ))
#endif
		    {
			VRUtils::ShowMemoryFullConfirmationQuery( ETrue );
			critical = ETrue;
			}
		
     	}

// now for multiple drives
#else
#ifdef  __AAC_ENCODER_PLUGIN
		if (((iMemo->Quality() != EQualityHigh) && SysUtil::DiskSpaceBelowCriticalLevelL( &fs, KVRRecStartMemBuf, iMemo->StorageDrive())) ||
		((iMemo->Quality() == EQualityHigh) && SysUtil::DiskSpaceBelowCriticalLevelL( &fs, KVRAACCriticalMemoryLevel, iMemo->StorageDrive())))		 
#else
        // now AAC format is not enabled
        if (SysUtil::DiskSpaceBelowCriticalLevelL( &fs, KVRRecStartMemBuf, iMemo->StorageDrive()))
#endif 
			{
			if ( iMemo->StorageDrive() == EDriveF)
				{
 				VRUtils::ShowMemoryFullConfirmationQuery(ETrue);
				}
			else
				{
		 		VRUtils::ShowMemoryFullConfirmationQuery();
				}
			critical = ETrue;
			}


#endif

	if( critical )
		{
		User::Leave( KErrDiskFull );
		}


    if ( !wasPaused )
        {
    	iTonePlayer->PlayTone( EAvkonSIDVoiceRecordingStartTone );
    	iTonePlayer->PrepareToneL( EAvkonSIDVoiceRecordingStopTone );
        }

// start monitor disk space 	 
#ifdef  __AAC_ENCODER_PLUGIN
    if (iMemo->Quality() == EQualityHigh)
    	{
        TInt drive (0);
#ifndef RD_MULTIPLE_DRIVE
	    if ( iMemo->MemoStore() == EMemoStorePhoneMemory )
		    {
		    drive = EDriveC;	
		    }
        else
    	    {
    	    drive = EDriveE;	
    	    }
// multiple drives
#else
        drive = iMemo->StorageDrive();
#endif

        iDiskSpaceWatcher->RequestNotification(KVRAACCriticalMemoryLevel,drive);		
    	}
#endif

	// Set recording preference
	iAudioRecorder->SetPriority( KAudioPriorityRecording, 
				TMdaPriorityPreference( KAudioPrefVoiceRec ) );	
	iAudioRecorder->SetGain( iAudioRecorder->MaxGain() );

	SetIfStopCalled(EFalse);

	iAudioRecorder->RecordL();
	
	// Sets the indication that recording has started -> 
	// Current file's handle can't be reused for next file
	iMemo->SetRecorded( ETrue );
	
			
    User::LeaveIfError( iPropVRState.Set( KPSUidVoiceRecorder,
        KVoiceRecorderMode, EVRRecording ) );

   	// Do not allow screen saver while recording
   	// Errors ignored, no actions needed if API is not available
   	iPropScreenSaver.Set( KPSUidScreenSaver, KScreenSaverAllowScreenSaver,
   		EVRScreenSaverNotAllowed );

	if( !iActiveWait.IsStarted() )
		{
		iActiveWait.Start(); // CSI: 10 #
		}    
	StartTimerL();

	return iMediaServerError;
	}


// ---------------------------------------------------------------------------
// CVRMdaRecorder::Stop
// 
// ---------------------------------------------------------------------------
//
void CVRMdaRecorder::Stop()
	{
	if ( iIsAttachedToMemo )
		{
		TBool reallyStopping( iPause );		
#ifdef _DEBUG
		RDebug::Print( _L( "VoiceRecorder: Stop error: %d" ), 
				iMediaServerError );
#endif
		iPause = EFalse;

		if( iAudioRecorder->State() == CMdaAudioRecorderUtility::ERecording )
			{
			reallyStopping = ETrue;
            
            // if the dim light is on, turn off it.
            if(iDimLight)
				{
	            
				iLight->ReleaseLight (CHWRMLight::EPrimaryDisplay);
				User::ResetInactivityTime();
                iDimLight = EFalse;
				}			
			
			}
	    // it will tell VRecorder not pause because it has been stopped.
		SetIfStopCalled(ETrue);	
		iAudioRecorder->Stop();

	    // if error does not happen in recording, change the file to visible 
	    iMemo->File().SetAtt( KEntryAttNormal, KEntryAttHidden);	
				
		iTonePlayer->PlayTone( EAvkonSIDVoiceRecordingStopTone );
		iTonePlayer->PrepareTone( EAvkonSIDVoiceRecordingStartTone );			

		if( reallyStopping )
			{
			// Set lower preference to ensure other sounds will 
			// be played correctly
			iAudioRecorder->SetPriority( KAudioPriorityRecording, 
				TMdaPriorityPreference( KAudioPrefRealOneLocalPlayback ) );
			}

        // Error value ignored if PS key is not available
        iPropVRState.Set( KPSUidVoiceRecorder, KVoiceRecorderMode, EVRIdle );
        // Used to check the state of a phone call if any
       
        if ( iCurrentCallHandler ) delete iCurrentCallHandler;
        
    	TRAP_IGNORE( iCurrentCallHandler = CVRSystemEventHandler::NewL() );
    	TInt currentState( iCurrentCallHandler->StateL(	KPSUidCtsyCallInformation, KCTsyCallState ) );
    	
    	// Do not allow screen saver if there is an active phone call
    	if(currentState == EPSCTsyCallStateRinging)
    		{
    	   	// Do not allow screen saver
       	  // Errors ignored, no actions needed if API is not available	   	
    	   	iPropScreenSaver.Set( KPSUidScreenSaver,
        		KScreenSaverAllowScreenSaver, EVRScreenSaverNotAllowed );
    		}
    	else
    		{
    	   	// Allow screen saver
       	    // Errors ignored, no actions needed if API is not available	   	
    	   	iPropScreenSaver.Set( KPSUidScreenSaver,
        		KScreenSaverAllowScreenSaver, EVRScreenSaverAllowed );
    		}

		StopTimer();
		ResetPosition();
		iMemo->UpdateModifiedDate();
		}
	}


// ---------------------------------------------------------------------------
// CVRMdaRecorder::StopAndNotify
// 
// ---------------------------------------------------------------------------
//
void CVRMdaRecorder::StopAndNotify()
	{
	Stop();
	if ( iAutoStopObserver )
		{
#ifdef _DEBUG
		RDebug::Print( _L( "VoiceRecorder: StopAndNotify AutoStopObserver: %d" ),
				iMediaServerError );
#endif
		iAutoStopObserver->Update();
#ifdef _DEBUG
		RDebug::Print( _L( "VoiceRecorder: StopAndNotify AutoStopObserver: %d" ),
				iMediaServerError );
#endif
		}
	}


// ---------------------------------------------------------------------------
// CVRMdaRecorder::Pause
// 
// ---------------------------------------------------------------------------
void CVRMdaRecorder::Pause()
	{
	__ASSERT_DEBUG( iIsAttachedToMemo, User::Panic( KVRPanic,
		EPanicNotAttached ) );

	iPause = ETrue;

#ifdef _DEBUG
	RDebug::Print( _L("VoiceRecorder: Pause called. Position: %d:%d"),
	    I64HIGH( iPosition.Int64() ), I64LOW( iPosition.Int64() )  );
#endif

	iAudioRecorder->Stop();

    // Error value ignored if PS key is not available
    iPropVRState.Set( KPSUidVoiceRecorder, KVoiceRecorderMode, EVRIdle );

   	// Don't allow screen saver
   	// Errors ignored, no actions needed if API is not available   	
   	iPropScreenSaver.Set( KPSUidScreenSaver,
		KScreenSaverAllowScreenSaver, EVRScreenSaverAllowed );

	StopTimer();
	SyncAndNotify();
	}


// ---------------------------------------------------------------------------
// CVRMdaRecorder::ForwardL
// 
// ---------------------------------------------------------------------------
//
TInt CVRMdaRecorder::ForwardL()
	{
	if ( !MovePositionL( KVRMdaRecorderDefaultStep ) )
		{
		return KErrOverflow;
		}
	return iMediaServerError;
	}


// ---------------------------------------------------------------------------
// CVRMdaRecorder::RewindL
// 
// ---------------------------------------------------------------------------
//
TInt CVRMdaRecorder::RewindL()
	{
	MovePositionL( -KVRMdaRecorderDefaultStep );
	return iMediaServerError;
	}


// ---------------------------------------------------------------------------
// CVRMdaRecorder::CanAcceptCommands
// 
// ---------------------------------------------------------------------------
//
TBool CVRMdaRecorder::CanAcceptCommands() const
	{
	return !iActiveWait.IsStarted();
	}


// ---------------------------------------------------------------------------
// CVRMdaRecorder::MovePositionL
// 
// ---------------------------------------------------------------------------
//
TBool CVRMdaRecorder::MovePositionL( TInt aSeconds )
	{
	__ASSERT_DEBUG( iIsAttachedToMemo, User::Panic( KVRPanic,
													EPanicNotAttached ) );

#ifdef _DEBUG
	RDebug::Print( _L("VoiceRecorder: MovePosition called. Base position: %d:%d"),
	    I64HIGH( iAudioRecorder->Position().Int64() ),
        I64LOW( iAudioRecorder->Position().Int64() )  );
#endif

	TInt64 interval;
	if (iPause)
		{
		interval = iPosition.Int64() + aSeconds * KVRSecondAsMicroSeconds;
		}
	// Not sure if iPosition value is valid other than when paused, so do as
	// previous for all other states:
	else 
		{
		interval = iAudioRecorder->Position().Int64() + 
								   aSeconds * 
								   KVRSecondAsMicroSeconds;
		}
	
	// Truncated to integer values	
	TInt integerInterval( interval / KVRSecondAsMicroSeconds );
	TInt integerDuration( iAudioRecorder->Duration().Int64() / 
						  KVRSecondAsMicroSeconds );	
	if ( interval < 0 )
		{
		interval = 0;
		}
	// if gone past the end or integer values are the same 
	// in duration and position	
	else if ( interval > iAudioRecorder->Duration().Int64() || 
		integerInterval == integerDuration )
		{
		interval = iAudioRecorder->Duration().Int64();
		}

#ifdef _DEBUG
	RDebug::Print( _L("VoiceRecorder: MovePosition called. Interval: %d:%d"),
	 I64HIGH( interval ), I64LOW( interval )  );
#endif

	TTimeIntervalMicroSeconds newPosition( interval );

	if (iPause)
		{
		iPosition = newPosition;
		}
	else if ( iAudioRecorder->State() == CMdaAudioClipUtility::EOpen )
		{
		// not playing, recording or paused.
#ifdef _DEBUG
	RDebug::Print( _L("VoiceRecorder. Idle.: MovePosition called. New Position: %d:%d"),
	 I64HIGH( newPosition.Int64() ), I64LOW( newPosition.Int64() )  );
#endif
		iAudioRecorder->SetPosition( newPosition );
		}
	else
		{

#ifdef _DEBUG
	RDebug::Print( _L("VoiceRecorder. Playing.: MovePosition called. New Position: %d:%d"),
	 I64HIGH( newPosition.Int64() ), I64LOW( newPosition.Int64() )  );
#endif

		iAudioRecorder->Stop();
		iAudioRecorder->SetPosition( newPosition );
		iAudioRecorder->PlayL();
		if( !iActiveWait.IsStarted() )
			{
			iActiveWait.Start(); // CSI: 10 #
			}		
		}
	SyncAndNotify();
	return ETrue;
	}

// ---------------------------------------------------------------------------
// CVRMdaRecorder::ConfigureMemoL()
// 
// ---------------------------------------------------------------------------
//
void CVRMdaRecorder::ConfigureMemoL()
    {
    DEB_PRN_0( _L( "CVRMdaRecorder::ConfigureMemoL-Enter" ));

    // Configure bitrate if recording an AMR clip.
    // Default bitrate is 12200 and it's used if codec doesn't support
    // the bitrate that is retrieved from Central Repository.
    if (iMemo->Quality() == EQualityMMSOptimized )
        {
        TUint configuredAmrBitrate( VRUtils::AMRBitrateL() );
        TUint amrBitrate( KVRDefaultAmrBitrate );

        // Fetch supported bitrates and find out if configured bitrate
        // is supported
        RArray<TUint> rates;
        iAudioRecorder->GetSupportedBitRatesL( rates );        
        TInt num = rates.Count();
        if(num > 0)
            {
            rates.Sort();
            TInt found = rates.Find(configuredAmrBitrate);
            if (found == KErrNotFound)
                {
                amrBitrate = rates[num-1];
                }
            }
        rates.Reset();  // Clear array      
        iAudioRecorder->SetDestinationBitRateL( amrBitrate );                  

        DEB_PRN_0( _L( "CVRMdaRecorder::ConfigureMemoL-Configured MMSOpt bitrate" ));
        }


    // added for supporting mp4 format
#ifdef __AAC_ENCODER_PLUGIN
    else if (iMemo->Quality() == EQualityHigh)
    	{
        TInt configuredAacSamplerate( VRUtils::AACSamplerateL() );
        TInt aacSampleRate( KVRDefaultSamplerateAac );

        TInt configuredAacBitrate( VRUtils::AACBitrateL() );
        TInt aacBitrate( KVRDefaultBitrateAac );

        TInt configuredAudioMode( VRUtils::AACAudioModeL() );   

        // Sample rate
        RArray<TUint> supportedSampleRates;
        iAudioRecorder->GetSupportedSampleRatesL(supportedSampleRates);        

        TInt num1 = supportedSampleRates.Count();
        if(num1 > 0)
            {
            supportedSampleRates.Sort();
            TInt found1 = supportedSampleRates.Find(configuredAacSamplerate);
            if (found1 == KErrNotFound)
                {
                aacSampleRate = supportedSampleRates[num1-1];
                }
            }
        supportedSampleRates.Reset();                           
        iAudioRecorder->SetDestinationSampleRateL(aacSampleRate);

        DEB_PRN_0( _L( "CVRMdaRecorder::ConfigureMemoL-Configured HiQ sampling rate" ));

        // Bit reate      
        RArray<TUint> supportedBitRates;         
        iAudioRecorder->GetSupportedBitRatesL(supportedBitRates);
        TInt num2 = supportedBitRates.Count();
        if(num2 > 0)
            {
            supportedBitRates.Sort();
            TInt found2 = supportedBitRates.Find(configuredAacBitrate);
            if (found2 == KErrNotFound)
                {
                aacBitrate = supportedBitRates[num2-1];
                }
            }
        supportedBitRates.Reset();
        iAudioRecorder->SetDestinationBitRateL(aacBitrate);
        DEB_PRN_0( _L( "CVRMdaRecorder::ConfigureMemoL-Configured HiQ bit rate" ));

        // the audio mode flag                                    
        RArray<TUint> supportedNumberOfChannels;       
        iAudioRecorder->GetSupportedNumberOfChannelsL(supportedNumberOfChannels); 
        TInt num3 = supportedNumberOfChannels.Count();
        if (num3 > 0)
            {
            supportedNumberOfChannels.Sort();
            TInt ifFound = supportedNumberOfChannels.Find(configuredAudioMode);
            if (ifFound == KErrNotFound)
                {
                configuredAudioMode = supportedNumberOfChannels [0];	
                }
            }
        supportedNumberOfChannels.Reset();
        iAudioRecorder->SetDestinationNumberOfChannelsL(configuredAudioMode);
        
        DEB_PRN_0( _L( "CVRMdaRecorder::ConfigureMemoL-Configured HiQ channel" ));        
        }
#endif // __AAC_ENCODER_PLUGIN
    iIsAttachedToMemo = ETrue;
    ResetPosition();
    DEB_PRN_0( _L( "CVRMdaRecorder::ConfigureMemoL-Exit" ));
    }

// ---------------------------------------------------------------------------
// CVRMdaRecorder::ResetPosition
// 
// ---------------------------------------------------------------------------
//
void CVRMdaRecorder::ResetPosition()
	{
	if ( iIsAttachedToMemo )
		{
		iAudioRecorder->SetPosition( TTimeIntervalMicroSeconds( 0 ) );
		SyncAndNotify();
		}
	}


// ---------------------------------------------------------------------------
// CVRMdaRecorder::StartTimerL
// Create and start a timer with initial delay of 0
// and a frequency of one second 
// ---------------------------------------------------------------------------
//
void CVRMdaRecorder::StartTimerL()
	{
	StopTimer();	// stop if already running

	TCallBack cb( TimerCallBack, this );
	iTimer = CPeriodic::NewL( 1 );  // priority = EPriorityNormal + 1 = 1
	iTimer->Start( 0, KVRDisplayUpdateDelay, cb );
	}


// ---------------------------------------------------------------------------
// CVRMdaRecorder::StopTimer
// Stop the timer by deleting the CPeriodic object 
// ---------------------------------------------------------------------------
//
void CVRMdaRecorder::StopTimer()
	{
	delete iTimer;
	iTimer = NULL;
	}


// ---------------------------------------------------------------------------
// CVRMdaRecorder::SyncAndNotify
// 
// ---------------------------------------------------------------------------
//
void CVRMdaRecorder::SyncAndNotify()
	{
	if(!iPause)
		{
		iPosition = iAudioRecorder->Position();
		}
	iMemo->SetPosition( iPosition );

	iMemo->SetDuration( iAudioRecorder->Duration() );

	for ( int i = 0; i < iObservers.Count(); i++ )
		{
		TVRUpdateCommand command( EVRUpdate );
		if ( ( iMemo->Position().Int64() / KVRSecondAsMicroSeconds ) == 1 )
		    {
		    command = EVRUpdate1Second;
		    }
		iObservers[ i ]->Update( command );
		}
	}


// ---------------------------------------------------------------------------
// CVRMdaRecorder::TimerCallBack
// 
// ---------------------------------------------------------------------------
//
TInt CVRMdaRecorder::TimerCallBack( TAny* aMdaRecorder )
	{
	CVRMdaRecorder* recorder = reinterpret_cast< CVRMdaRecorder* >(
															   aMdaRecorder );
	recorder->HandleTick();
	return ETrue;
	}


// ---------------------------------------------------------------------------
// CVRMdaRecorder::HandleTick
// 
// ---------------------------------------------------------------------------
//
void CVRMdaRecorder::HandleTick()
	{
	SyncAndNotify();

	if ( iAudioRecorder->State() == CMdaAudioClipUtility::ERecording )
		{
		
		TTimeIntervalMicroSeconds position( 0 );
		TTimeIntervalMicroSeconds duration( 0 );
		TTimeIntervalMicroSeconds maxDuration( 0 );
		
		position = iMemo->Position();
		duration = iMemo->Duration();
		maxDuration = iMemo->MaxDuration();
		
		// Position value is used for comparison because it gives more accurate 
		// value than Duration value
		if ( position >= maxDuration )
			{
#ifdef _DEBUG
	RDebug::Print( _L("VoiceRecorder: Max duration passed. Position: %d:%d"),
	 I64HIGH( position.Int64() ), I64LOW( position.Int64() )  );
#endif	

#ifdef _DEBUG
	RDebug::Print( _L("VoiceRecorder: Max duration passed. Duration: %d:%d"),
	 I64HIGH( duration.Int64() ), I64LOW( duration.Int64() )  );
#endif	

#ifdef _DEBUG
	RDebug::Print( _L("VoiceRecorder: Max duration passed. MaxDuration: %d:%d"),
	 I64HIGH( maxDuration.Int64() ), I64LOW( maxDuration.Int64() )  );
#endif	
			// Note that some data cropping is possible after stopping
			// ->Duration decreases
			StopAndNotify();
			}
		}
	}


// ---------------------------------------------------------------------------
// CVRMdaRecorder::SetIhf
// 
// ---------------------------------------------------------------------------
//
TInt CVRMdaRecorder::SetIhf( TBool /*aEnabled*/ )
	{
	return KErrNotSupported;
	}


// ---------------------------------------------------------------------------
// CVRMdaRecorder::SetPositionL
// 
// ---------------------------------------------------------------------------
//
void CVRMdaRecorder::SetPositionL( TInt aSeconds )
	{
	__ASSERT_DEBUG( iIsAttachedToMemo, User::Panic( KVRPanic,
											EPanicNotAttached ) );
	
	if( iAudioRecorder->State() == CMdaAudioClipUtility::ERecording ||
		iAudioRecorder->State() == CMdaAudioClipUtility::ENotReady )
		{
		return;
		}
		
	TInt64 interval ( TInt64( aSeconds ) * KVRSecondAsMicroSeconds );
	
	// Truncated to integer values	
	TInt integerInterval( interval / KVRSecondAsMicroSeconds );
	TInt integerDuration( iAudioRecorder->Duration().Int64() / 
										  KVRSecondAsMicroSeconds );	
	
	if ( interval < 0 || integerDuration == 0 )
		{
		interval = 0;
		}
	// if gone past the end or integer values are the same in 
	// duration and position
	else if ( interval > iAudioRecorder->Duration().Int64() || 
		integerInterval == integerDuration )
		{
		interval = iAudioRecorder->Duration().Int64();
		}

	TTimeIntervalMicroSeconds newPosition( interval );

	if ( iPause )
		{
		iPosition = newPosition;
		}
	else if ( iAudioRecorder->State() == CMdaAudioClipUtility::EOpen )
		{
		iAudioRecorder->SetPosition( newPosition );
		}
	else
		{
		iAudioRecorder->Stop();
		iAudioRecorder->SetPosition( newPosition );
		iAudioRecorder->PlayL();
		if( !iActiveWait.IsStarted() )
			{
			iActiveWait.Start(); // CSI: 10 #
			}
		}
	SyncAndNotify();	
	}

// ---------------------------------------------------------
// From class MBacklightControlObserver.
// CNTExternalEventObserver::LightStatusChanged
// ---------------------------------------------------------
//
void CVRMdaRecorder::LightStatusChanged(TInt aTarget, CHWRMLight::TLightStatus aStatus)
     {

     if(aTarget == CHWRMLight::EPrimaryDisplayAndKeyboard || aTarget == CHWRMLight::EPrimaryDisplay)
     	{
        if(aStatus == CHWRMLight::ELightOff)
            {
 	        if (iAudioRecorder)
 	        	{	
 	            if (iAudioRecorder->State() == CMdaAudioClipUtility::ERecording ) // set light to dimmed
 	                {

                    TRAPD(err, iLight->LightOnL(CHWRMLight::EPrimaryDisplay, KHWRMInfiniteDuration, 10, ETrue));
                    if ( err == KErrNone )
                       {
                       iDimLight = ETrue;
                       }

 	                }
               	}
               	
                return;
             }
         
         if((aStatus == CHWRMLight::ELightOn) && (aTarget == CHWRMLight::EPrimaryKeyboard))
         	{
             
             if (iLight)
             	{
             		             	
                CHWRMLight::TLightStatus status = iLight->LightStatus(CHWRMLight::EPrimaryDisplay);
                
                if( (status == CHWRMLight::ELightOn)  && iDimLight )
                	{
                	    User::ResetInactivityTime();
                		iDimLight = EFalse;
                	}
             	}
            return;
         	}
     
     	}
     }


// ---------------------------------------------------------
// GetInRecordingFlag
// 
// ---------------------------------------------------------
//		
TBool CVRMdaRecorder::GetInRecordingFlag()
	{
		return ifInRecording;
	}
	
	
// ---------------------------------------------------------
// SetInRecordingFlag
// ---------------------------------------------------------
//
void CVRMdaRecorder::SetInRecordingFlag(TBool aFlag)
	{
	ifInRecording = aFlag;	
	}


// ---------------------------------------------------------
// GetInRecordingFlag
// 
// ---------------------------------------------------------
//		
TBool CVRMdaRecorder::GetIfStopCalled()
	{
		return ifStopCalled;
	}
	
	
// ---------------------------------------------------------
// SetInRecordingFlag
// ---------------------------------------------------------
//
void CVRMdaRecorder::SetIfStopCalled(TBool aFlag)
	{
	ifStopCalled = aFlag;	
	}

// ---------------------------------------------------------
// IsAttachedToMemo
// ---------------------------------------------------------
//
TBool CVRMdaRecorder::IsAttachedToMemo()
	{
	return iIsAttachedToMemo;
	}


// ---------------------------------------------------------
// CreateAudioInput
// ---------------------------------------------------------
//	
void CVRMdaRecorder::CreateAudioInputL()
	{
	CAudioInput::TAudioInputPreference audioInput = CAudioInput::EDefaultMic;
  
	if ((iAudioRecorder->State() == CMdaAudioClipUtility::EOpen)&&
	    (!iAudioInput))
    	{  
        iAudioInput = CAudioInput::NewL(*iAudioRecorder);        
        SetAudioInputL(audioInput); 
        iCurrentAudioInput = audioInput;                                    
    	}
    
	}
// ---------------------------------------------------------
// SetAudioInputL
// ---------------------------------------------------------
//
void CVRMdaRecorder::SetAudioInputL(CAudioInput::TAudioInputPreference aAudioInput)
	{
	if((iAudioInput)&&(iCurrentAudioInput!=aAudioInput))
    	{	
        RArray<CAudioInput::TAudioInputPreference> inputArray(4);
        CleanupClosePushL( inputArray );
        inputArray.Append( aAudioInput );
        iAudioInput->SetAudioInputL( inputArray.Array() );   
        CleanupStack::PopAndDestroy( &inputArray );
        iCurrentAudioInput = aAudioInput;
    	}
    	
	}
	
// ---------------------------------------------------------
// DeleteAudioInput
// ---------------------------------------------------------
//	
void CVRMdaRecorder::DeleteAudioInput()
	{
	delete iAudioInput;
	iAudioInput = NULL;
	}
	
// ---------------------------------------------------------
// ConfigSampleRateOfVoiceCallL
// ---------------------------------------------------------
//	
void CVRMdaRecorder::ConfigSampleRateOfVoiceCallL()
	{
	// Sample rate
	TUint sampleRates = iAudioRecorder->DestinationSampleRateL();
	TInt configuredAacSamplerate( VRUtils::AACSamplerateL() );
	if(sampleRates == configuredAacSamplerate)
		{
		return;
		}
	
	if (iMemo->Quality() == EQualityHigh)
		{
		TInt aacSampleRate( KVRDefaultSamplerateAac );
		// Sample rate
		RArray<TUint> supportedSampleRates;
		iAudioRecorder->GetSupportedSampleRatesL(supportedSampleRates);
		TInt num1 = supportedSampleRates.Count();
		if(num1 > 0)
			{
			supportedSampleRates.Sort();
			TInt found1 = supportedSampleRates.Find(configuredAacSamplerate);
			if (found1 == KErrNotFound)
				{
				aacSampleRate = supportedSampleRates[num1-1];
				}
			}
		supportedSampleRates.Reset(); 
		iAudioRecorder->SetDestinationSampleRateL(aacSampleRate);
		}
	}

// End of file
