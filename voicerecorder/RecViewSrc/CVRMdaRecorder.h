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


#ifndef __CVRMDARECORDER_H__
#define __CVRMDARECORDER_H__

// INCLUDES
#include <e32property.h>
#include <mdaaudiosampleeditor.h>
#include <hwrmlight.h>

#include "MVRMdaRecorder.h"
#include "MVRUpdateEventProvider.h"

#include <AudioInput.h>
// FORWARD DECLARATIONS
class CVRSystemEventHandler;
class CAknGlobalNote;
class CMdaAudioRecorderUtility;
class CVRMemo;
class MVRObserver;
class CErrorUI;
class CAudioOutput;
class CVRTonePlayer;
class CVRDiskSpaceWatcher;
class CAudioInput;

// CLASS DEFINITION
/**
* This class acts as a high level wrapper around the media server
* playback and recording functionalities.
*/
NONSHARABLE_CLASS( CVRMdaRecorder )
    : public CBase, public MVRMdaRecorder,
      public MMdaObjectStateChangeObserver, 
      public MVRUpdateEventProvider,
      public MHWRMLightObserver   // for light control
	{
	public: // Constructors and destructor

		/**
		* Default constructor
		*/
		CVRMdaRecorder();

		/**
		* Destructor
		*/
		~CVRMdaRecorder();

		/**
		* 2nd phase constructor
		*/
		void ConstructL();

	public: // from MMdaObjectStateChangeObserver

		/**
		* Encapsulates the interface for handling the change of state of
		* an audio data sample object.
		* @param aObject A pointer to the audio sample object that has
		*				 changed state.
		* @param aPreviousState The state before the change
		* @param aCurrentState The state after the change
		* @param aErrorCode If not KErrNone, then the error that
		*					caused the state change
		*/
		void MoscoStateChangeEvent( CBase* aObject, TInt aPreviousState,
									TInt aCurrentState, TInt aErrorCode );

		/**
		* Registers an observer for receiving update notifications.
		* @param aObserver The observer object.
		*/
		void RegisterObserver( MVRObserver* aObserver );

		/**
		* Unregisters a previously registered observer
		* @param aObserver The observer object.
		*/
		void UnregisterObserver( MVRObserver* aObserver );

		/**
		* Registers an observer for receiving auto stop notifications.
		* @param aObserver The observer object.
		*/
		void SetAutoStopObserver( MVRObserver* aObserver );

		/**
		* Attachs this recorder to a memo file, i.e. opens it
		* and initialises media server.
		* @param aMemo Specifies the file to be attached to.
		*/
		void AttachToMemoL( CVRMemo* aMemo );

		/**
		* Creates a new memo file and attachs to it.
		* @param aMemo Specifies the file to be created.
		*/
		void CreateNewMemoL( CVRMemo* aMemo );

		/**
		* Detachs from the current memo, i.e. closes the file.
		*/
		void DetachMemo();

		/**
		* Sets playback volume.
		* @param aStep Volume level in steps
		* @param aMaxSteps How many steps there are (10 usually).
		*/
		void SetVolume( TInt aStep, TInt aMaxSteps );

		/**
		* Starts playback.
		* @return System wide error code
		*/
		TInt PlayL();

		/**
		* Starts recording.
		* @return System wide error code
		*/
		TInt RecordL();

		/**
		* Stops playback or recording.
		*/
		void Stop();

		/**
		* Pauses playback or recording.
		*/
		void Pause();

		/**
		* Moves the playing position forward 5 seconds.
		* @return System wide error code. KErrOverflow if the
		* position was forwarded past the end.
		*/
		TInt ForwardL();

		/**
		* Moves the playing position backward 5 seconds.
		* @return System wide error code
		*/
		TInt RewindL();

		/**
		* Indicates if we are in a state that can accept
		* more commands (i.e. all async calls have completed).
		* @return ETrue if it's ok to proceed calling methods.
		*/
		TBool CanAcceptCommands() const;

		/**
		* Set active speaker if feature is supported
		* @param aEnabled ETrue if playback should be routed to IHF
		* @return Error code returned by routing operation
		*/
		TInt SetIhf( TBool aEnabled );

		/**
		* Set the audio recorder position to a new value
		* @param aSeconds The position to set
		*/
		void SetPositionL( TInt aSeconds );

        /**
        * From MBacklightControlObserver.
        * Callback for change in backlight status
        *
        * @since S60 ?S60_version
        * @param aTarget The target of light change event; keypad, screen or both
        * @param aTarget New status of the target
        */
        void LightStatusChanged(TInt aTarget, CHWRMLight::TLightStatus aStatus);
	
		/**
		* Returns iInRecording flag
		* @return True or False
		*/
        TBool GetInRecordingFlag();
        
       /**
		* set iInRecording flag
		* 
		*/
        void SetInRecordingFlag(TBool);
 
		/**
		* Returns ifStopCalled flag
		* @return True or False
		*/
        TBool GetIfStopCalled();
        
       /**
		* set ifStopCalled flag
		* 
		*/
        void SetIfStopCalled(TBool); 
 
       /**
		* return the iIsAttachToMemo flag
		* 
		*/
        TBool IsAttachedToMemo();
            	/** 
		* Sets the AudioInput.
		* 
		* Leaves on error.
		*/
        void SetAudioInputL(CAudioInput::TAudioInputPreference aAudioInput);
        
       /**
		* set sample rate during voice call
		* 
		*/
        void ConfigSampleRateOfVoiceCallL();
                 
public:
      friend class CVRDiskSpaceWatcher;        
  
	private: // new methods

		/**
		* Moves the playing position to beginning.
		*/
		void ResetPosition();

		/**
		* Stops playing or recording and notifies the observer
		* registered with SetAutoStopObserver().
		*/
		void StopAndNotify();

		/**
		* Updates duration and position fields in CVRMemo
		* and notifies the observer registered with SetObserver();
		*/
		void SyncAndNotify();

		/**
		* Creates and starts a CPeriodic timer object
		* with a resolution of one second.
		*/
		void StartTimerL();

		/**
		* Stops and deletes the timer object.
		*/
		void StopTimer();

		/**
		* Called by the CPeriodic timer to handle a tick.
		* Uses HandleTick() to do the actual work.
		* @param aMdaRecorder Always a valid pointer to CVRMdaRecorder object
		* @return ETrue to indicate that the timer should continue.
		*/
		static TInt TimerCallBack( TAny* aMdaRecorder );

		/**
		* Non-static variant of TimerCallBack (more convinient to implement).
		*/
		void HandleTick();

		/**
		* Moves the playing position forward or backward.
		* @param aSeconds Negative for bacward, positive for forward
		* @return EFalse if the position moved past the end
		*/
		TBool MovePositionL( TInt aSeconds );
		
		/**
		* Configures the memo to be recorded.
		*
		* This function reads configuration data (bitrate, sampling rate,
		* mono/stereo information from utility and configures the clip opened
		* recording.
		*
		* Leaves on error.
		*
		*/
		void ConfigureMemoL();
		
        
        	/** 
		* Creates the AudioInput.
		* 
		* Leaves on error.
		*/
        	void CreateAudioInputL();
        
      
		/**
		* Deletes the AudioInput.
		*
		*/
        	void DeleteAudioInput();
        
        

	private: // data
		
		/**
		* For listening call events. Owned.
		*/
		CVRSystemEventHandler* iCurrentCallHandler;

		/**
		* Pointer to the media server audio recorder object. Owned.
		*/
		CMdaAudioRecorderUtility* iAudioRecorder;

		/**
		* Pointer to the voice memo object. Not owned.
		*/
		CVRMemo* iMemo;

		/**
		* Pointer to the timer object. Owned.
		*/
		CPeriodic* iTimer;

		/**
		* Pointers to the memo status observers. Not owned.
		*/
		RPointerArray<MVRObserver> iObservers;

		/**
		* Pointer to the auto stop (non-user initiated) observer. Not owned.
		*/
		MVRObserver* iAutoStopObserver;


		/**
		* ETrue if a memo is currently attached.
		*/
		TBool iIsAttachedToMemo;

		/**
		* Identifies the last media server error, or KErrNone.
		*/
		TInt iMediaServerError;

		/**
		* CActiveScheduler wrapper object. Owned.
		*/
		CActiveSchedulerWait iActiveWait;

		/**
		* Pointer to the generic error note object. Owned.
		*/
		CAknGlobalNote* iGlobalNote;

		/**
		* Pointer to a Error UI object. Owned.
		* Used for showing system error notes
		*/
		CErrorUI* iErrorUI;

        /**
		* Pub&Sub property.
		* For notifying voice dialing about the state of voice recorder
		*/        
        RProperty iPropVRState;

        /**
		* Pub&Sub property.
		* For checking and setting the state of the screen saver.
		*/
        RProperty iPropScreenSaver;
        
		/**
		* Fix for pause problem
		*/
		TBool iPause;

		/**
		* For storing current position
		*/
		TTimeIntervalMicroSeconds iPosition;
		
		/**
		* For playing the recording start and stop tones. Owned.
		*/
		CVRTonePlayer* iTonePlayer;

	    /**
		* Pointer to the CHWRMLight. owned.
		*/
        CHWRMLight* iLight;

	    /**
		* State of the Light. If EFalse the dimmed light is set as off
		* causes no action.
		*/
		TBool iDimLight;
		
		// Boolean to maintain attaching to memo state.
		TBool iAttachingToMemo;

       /**
		* Returns if Voice Recorder start to record new file (real recording not necessary started) .
		* @return True or False
		*/
        TBool ifInRecording;


       /**
		* Returns if Voice Recorder has called stoped, if so should not handle Pause .
		* @return True or False
		*/
        TBool ifStopCalled;

        
        CVRDiskSpaceWatcher* iDiskSpaceWatcher;
        CAudioInput* iAudioInput;
        CAudioInput::TAudioInputPreference iCurrentAudioInput;


  	};

#endif // __CVRMDARECORDER_H__
