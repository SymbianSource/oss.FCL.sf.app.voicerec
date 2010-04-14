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


#ifndef __CVRRECVIEWMODEL_H__
#define __CVRRECVIEWMODEL_H__

// INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <e32property.h> 
#include <AknNotifyStd.h>
#include <badesca.h>

#include "MVRIHFObserver.h"
#include "MVRKeyObserver.h"
#include "MVRDecoratorModel.h"
#include "MVRButtonPanelModel.h"
#include "MVRStateInfoModel.h"
#include "MVRObserver.h"
#include "MVRSelectionProvider.h"
#include "MVRVolumeChangeObserver.h"

#include "MVRSystemEventObserver.h"
#include "MVRUSBEventObserver.h"
#include "MVRMediaRemovalObserver.h"


#include "TVRContext.h"
#include "voicerecorder.hrh"

#include "MVRStateInfoObserver.h"

// FORWARD DECLARATIONS
class CAknGlobalNote;
class CSendUi;
class MVRMdaRecorder;
class CVRMemo;
class MVRObserver;
class TResourceReader;
class TVRState;
class CVRSystemEventHandler;
class CVRUSBEventHandler;
class CVRMediaRemovalMonitor;


// CLASS DEFINITION
/**
* This class is the data model and state machine of the Recorder View.
* The state machine is defined by a VR_STATEMACHINE resource structure.
*/
NONSHARABLE_CLASS( CVRRecViewModel ): 
	  public CBase,
	  public MVRDecoratorModel, public MVRButtonPanelModel,
	  public MVRStateInfoModel, public MVRObserver,
	  public MVRKeyObserver, public MVRVolumeChangeObserver,
      public MVRSystemEventObserver,
      public MVRUSBEventObserver, public MVRMediaRemovalObserver      
	{
	private:	// nested classes

		class TVRLabelLayoutLink
			{
			public:
				/**
				* Constructs this object from a VR_LABEL_LINK resource
				* @param aReader A resource reader object initialised
				*		 to a resource.
				*/
				void ReadFromResource( TResourceReader& aReader );
			public:

				// Link to the latin layout
				TInt iEuroId;
				
				// Link to the APAC layout
				TInt iApacId;
				
				// Link to the A/H layout
				TInt iArabicHebrewId;
			};
		
	public: // Constructors and destructor

		/**
		* Default constructor.
		*/
		CVRRecViewModel();

		/**
		* Destructor
		*/
		~CVRRecViewModel();

		/**
		* Constructs the model from a VR_STATEMACHINE resource.
		* @param aReader A resource reader object initialised to a resource.
		*/
		void ConstructFromResourceL( TResourceReader& aReader );

	public: // from MVRKeyObserver

		/**
		* This function is called when a subject wants to report a key event.
		* @param aKeyEvent The key event.
		* @param aType The type of key event: EEventKey, EEventKeyUp
		*              or EEventKeyDown
		* @return Indicates whether or not the key event was used.
		*/
		TBool ProcessKeyEventL( const TKeyEvent& aKeyEvent,
								const TEventCode aType );

	public: // from MVRDecoratorModel

		/**
		* Returns the resource id for current softkeys
		* @return Softkey resource identifier
		*/
		TInt CbaResourceId() const;

		/**
		* Returns the resource id for current menu bar
		* @return Menu bar resource identifier.
		*/
		TInt MenuBarResourceId() const;

		/**
		* Used to get the state of the volume control component.
		*/
		void GetVolumeControlState( TVRVolumeControlState& aState,
											TInt& aVolume );

		/**
		* Specifies the current help context.
		* @param aContext the context IDs are placed here
		*/
		void GetHelpContext( TCoeHelpContext& aContext ) const;


		/**
		* Registers an observer for receiving
		* update notifications from this interface.
		* @param aObserver The observer object.
		*/
		void SetDecoratorObserver( MVRObserver* aObserver );
		
		/**
		* Tells in return value if CBA Buttons should be enabled or not
		* @return ETrue if buttons should be enabled, EFalse otherwise
		*/
		TBool CBAEnabled() const;
		
	public: // from MVRButtonPanelModel

		/**
		* Returns the number of buttons.
		* @return number of buttons (>=0)
		*/
		TInt ButtonCount() const;

		/**
		* Returns the state of a specific button.
		* @param aButtonId Button identifier
		* @return ENormal or EDimmed
		*/
		TInt ButtonState( TInt aButtonId ) const;

		/**
		* Returns the command id of a specific button.
		* @param aButtonId Button identifier
		* @return One of TVRCommands
		*/
		TInt CommandId( TInt aButtonId ) const;

		/**
		* Returns the id of the button that should have initial focus.
		* @return Button identifier
		*/
		TInt InitialFocusButtonId() const;

		/**
		* Registers an observer for receiving
		* update notifications from this interface.
		* @param aObserver The observer object.
		*/
		void SetButtonPanelObserver( MVRObserver* aObserver );

		/**
		* Checks if button panel model needs to reset the focus
		* for example after a state change
		* @return Is reset needed
		*/		
		TBool ResetNeeded();
		
		/**
		* Returns an ID of the current visual state.
		* @returns Visual state Id.
		*/
		TUint VisualStateId() const;
		
        /**
		* Checks if there is a call incoming at the moment
		* @return ETrue if call incoming, EFalse otherwise
		*/
        TBool IncomingCall() const;	

		/**
		* Checks if model is in a state where it can take and handle
		* new commands
		* @return Can model handle new commands
		*/
		TBool CanHandleCommands() const;

	public: // from MVRStateInfoModel

		/**
		* Returns the length of the current memo
		* @return The duration in microseconds.
		*/
		TTimeIntervalMicroSeconds Duration() const;

		/**
		* Returns the playing or recording position of the current memo.
		* @return The position in microseconds.
		*/
		TTimeIntervalMicroSeconds Position() const;

		/**
		* Returns the text content for a label.
		* @param aName Returned text is placed in this descriptor
		* @param aLabelIndex Label identifier (>=0)
		*/
		void GetLabel( TDes& aName, TInt aLabelIndex ) const;

		/**
		* Returns the resource id for a label.
		* @param aLabelIndex Label identifier (>=0)
		* @return The resource id for the specified label
		*/
		TInt ResourceIdForLabel( TInt aLabelIndex, TAknLayoutId aLayout ) const;

		/**
		* Registers an observer for receiving
		* update notifications from this interface.
		* @param aObserver The observer object.
		*/
		void SetStateInfoObserver( MVRObserver* aObserver );
		
		/**
		* Determines if model has a progress bar
		* @return ETrue, if model has a progress bar, otherwise EFalse
		*/
		TBool HasProgressBar() const;
		
		/**
		* Determines if model has a volume control in this state
		* @return ETrue, if model has a volume control, otherwise EFalse
		*/
		TBool HasVolumeControl() const;
		
		/**
		* Registers an observer to communicate about the changes in InfoPanel. 
		* @param aObserver The observer object.
		*/
		void SetStateInfoPanelObserver( MVRStateInfoObserver* aObserver );

	public: // from MVRObserver

		/**
		* Called to notify a change in the observed subject's state.
		*/
		void Update( TVRUpdateCommand aCommand );

	public: // from MVRSelectionProvider

		/**
		* Returns the current memo name in an array
		* Ownership is transferred. Caller must destroy the returned array.
		* @return The array containing 0 or 1 names.
		*/
		MDesCArray* GetSelectedFilesLC() const;

	public: // from MVRVolumeChangeObserver

		/**
		* Called to notify a change in volume settings.
		* @param aStep Volume level in steps
		* @param aMaxSteps How many steps there are (10 usually).
		*/
		void SetVolume( TInt aStep, TInt aMaxSteps );

		/***
		* Indicates if volume can be set, because rinto tone
		* volume change has no effect if the tone is playing.
		*/
		TBool CanSetVolume();

	public: // CVRSystemEventObserver
	
		void HandleSystemEventL();


		void HandleUSBEventL();
		
		
		void HandleMMCEjectEventL();
		
	public: // new methods

		/**
		* Handles a option menu / command button / softkey command.
		* @param aCommandId Command identifier, one of TVRCommands
		*/
		void HandleCommandL( TInt aCommandId );
		
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
		* Check if stop has been called before
		* @return True or False
		*/
        TBool GetIfStopCalled();


		/**
		* Enters a new state machine context.
		* Executes the initial state transition defined by the context.
		* @param aContext Identifies the new context.
		*/
		void EnterContextL( const TVRRecViewContexts& aContext );

		/**
		* Leaves the current state machine context.
		*/
		void LeaveContext();

		/**
		* Sets the memo name. Forwards the call to CVRMemo.
		* @param aFilename The new file name
		*/
		void SetMemoName( const TDesC& aFilename );

		/**
		* Sets the memo name to a new, unique, one.
		* Forwards the call to CVRMemo.
		*/
		void SetMemoNameNewL( TBool aEmbedded = EFalse );

		/**
		* Sets the path to directory where memos will be saved.
		* Forwards the call to CVRMemo.
		*/
		void SetSavingLocationL( const TDesC& aPath );

		/**
		* Returns the complete file name (path and everything).
		* @return Reference to the memo name.
		*/
		const TDesC& Filename() const;

		/**
		* Returns embedding status.
		* @param ETrue if the current context is an embedded one.
		*/
		TBool IsEmbedded() const;

		/**
		* Indicates if we are in a state that can accept
		* more commands (i.e. all async calls have completed).
		* @return ETrue if it's ok to proceed calling methods.
		*/
		TBool CanAcceptCommands() const;

		/**
		* Sends the current memo via MMS/Infra/Bluetooth/Email
		* @param aSendUi The CSendUi object used for sending
		*/
		void SendViaL( CSendUi* aSendUi );

		/**
		* Activates the recorder view model
		*/
		void ActivateL();

		/**
		* Deactivates the recorder view model
		*/
		void Deactivate();

        /**
        * Sets the file handle to record into
        */
        void SetFileHandle( RFile& aFile );

        /**
        * Returns the active file handle of CVRMemo object
        */
        RFile& MemoFileHandle();
        
        /**
        * Enables or disables the Volume Control drawing in current
        * visual state
        * @param aVolumeControl ETrue if Volume Control should be enabled
        * 						EFalse if disabled 
        */
        void EnableVolumeControl( TBool aVolumeControl );
        
        /**
		* Returns the active quality setting.
		* @return Quality setting of the active memo
		*/
		TVRQuality Quality() const;
		
		              
	private: // new methods

		/**
		* Enters a new state.
		* Executes a function identified by the state transition.
		* Notifies all registered observers.
		* @param aNewState identifies the new state.
		* @param aForce force change state (even if new is same as old)
		*/
		void ChangeState( TUint aNewState, TBool aForced = EFalse );

		/**
		* Retrieves transition information to the given transition object.
		* @param aTransition Transition to be filled in.
		* @param aCommandId The command that triggered this transition.
		*/
		void GetTransition( TVRContext::TTransition& aTransition,
							TUint aCommandId ) const;

		/**
		* Retrieves transition information to the given transition object.
		* This is used the retrieve the initial transition of a state.
		* @param aTransition Transition to be filled in.
		*/
		void GetTransition( TVRContext::TTransition& aTransition ) const;

		/**
		* Calls the specified function.
		* @param aFunctionId Function identifier (TVRFunctions)
		* @return EFalse if the function want's to cancel the state change
		*/
		TBool CallFunctionByIdL( TInt aFunctionId );

		/**
		* Returns the text content for a label.
		* This is a leaving variant of GetLabel(). It is called by
		* GetLabel() inside a TRAP.
		* @param aName Returned text is placed in this descriptor
		* @param aLabelIndex Label identifier (>=0)
		*/
		void GetLabelL( TDes& aName, TInt aLabelIndex ) const;

		/**
		* Stops recording and saves the memo.
		* The specified note text is displayed while saving.
		* @param aResourceId  Identifies the note text.
		*/
		void StopAndSaveL( TInt aResourceId );

		/**
		* Stops recording and saves the memo.
		* The specified note text is displayed while saving.
		* Leaves the memo detached so that application can be closed.
		* @param aResourceId  Identifies the note text.
		*/
		void StopAndCloseL( TInt aResourceId );

		/**
		* Shows a global note.
		* @param aResourceId Identifies the note text.
		* @param aNoteType Identifies the note type.
		*/
		void ShowNoteL( TInt aResourceId, TAknGlobalNoteType aNoteType );

		/**
		* Loads the volume settings (IHF & ear piece) from VoiceRecorder.ini
		*/
		void LoadVolumeSettingsL();

		/**
		* Saves the volume settings (IHF & ear piece) to VoiceRecorder.ini
		* Note: This method is non-leaving and ignores all errors, because
		* it's called inside the destructor. LoadVolumeSettingsL() has
		* sanity checking so the situation isn't fatal if the saving fails.
		*/
		void SaveVolumeSettings();

		/**
		* Sets IHF on/off, if the phone is not in voice on alerting state
		* @param aIhf ETrue == set IHF on, EFalse == set IHF off
		*/
		void SetIHFIfCallNotActive( TBool aIhf );

		/*
		* Checks if the current default speaker is loudspeaker or earpiece
		*/
		void CheckIhfState();

		/**
		* Sets the volume on the media server client object according
		* to the current active IHF setting.
		*/
		void SetRecorderVolume();

		/**
		* Reads the default speaker setting and sets iIhfState accordingly
		*/
		void ReadDefaultSpeakerL();

		/*
		* Handles error in Update method.
		* @param aErr Id of the error
		*/
		void HandleUpdateErrorL( TInt aErr );

		/*
		* Formats time to R_QTN_TIME_DURAT_LONG or 
		* R_QTN_TIME_DURAT_MIN_SEC_WITH_ZERO depending of time value
		* @param aTime Time that is formatted
		* @param aString Buffer that holds the formatted time
		*/
		void FormatTimeL( const TTime& aTime, TDes& aString ) const;
		
		/*
		* Generates an array of services that should be dimmed from the 
		* options menu "Send" selection list
		* @return A pointer to an array that consists of the service Uids 
		* 		that should be dimmed
		*/
		CArrayFixFlat<TUid>* SendViaServicesToDimLC();

	private: // data

		/**
		* The state machine structure.
		*/
		TVRContext iContexts[ ENumContexts ];

		/**
		* The current context.
		*/
		TVRRecViewContexts iContext;

		/**
		* Pointer to the current state. Contained in iContexts.
		*/
		TVRState* iState;

		/**
		* Pointer to the voice memo object. Owned
		*/
		CVRMemo* iMemo;

		/**
		* Pointer to the recorder object. Owned
		*/
		MVRMdaRecorder* iRecorder;

		/**
		* Pointer to a global information note object. Owned
		*/
		CAknGlobalNote* iGlobalNote;

		/**
		* Pointer to an array of UI states. Owned
		*/
		CArrayFixSeg< TVRState > iStateArray;

		/**
		* Pointer to an array of label sets. Owned.
		*/
		CArrayPtrSeg< CDesCArray > iLabelSets;

		/**
		* Pointer to an array of label resource ids. Owned.
		*/
		TVRLabelLayoutLink* iLabelLayoutLinks;

		/**
		* Pointer to the button panel observer. Not owned.
		*/
		MVRObserver* iButtonPanelObserver;

		/**
		* Pointer to the decorator observer. Not owned.
		*/
		MVRObserver* iDecoratorObserver;

		/**
		* Pointer to the state info observer. Not owned.
		*/
		MVRObserver* iStateInfoObserver;

		/**
		* Format string placeholder.
		* R_QTN_DATE_USUAL_WITH_ZERO is loaded to it in construction.
		*/
		TBuf< VRLABELMAXLENGTH > iDateUsualWithZero;

		/**
		* Format string placeholder.
		* R_QTN_TIME_USUAL_WITH_ZERO is loaded to it in construction.
		*/
		TBuf< VRLABELMAXLENGTH > iTimeUsualWithZero;

		/**
		* Format string placeholder.
		* R_QTN_TIME_DURAT_MIN_SEC_WITH_ZERO is loaded to it in construction.
		*/
		TBuf< VRLABELMAXLENGTH > iTimeDuratMinSecWithZero;
		
		/**
		* Format string placeholder.
		* R_QTN_TIME_DURAT_LONG is loaded to it in construction.
		*/		
		TBuf< VRLABELMAXLENGTH > iTimeDuratLong;

		/**
		* ETrue if the model has succesfully entered a context.
		*/
		TBool iContextEntered;

		/**
		* For listening call events. Owned.
		*/
		CVRSystemEventHandler* iCurrentCallHandler;

		/**
		* For listening USB events. Owned.
		*/
		CVRUSBEventHandler* iCurrentUSBHandler;
		
		CVRMediaRemovalMonitor* iCurrentMMCEjectHandler;


		/**
		* Current logical state (i.e. what the user sees, not the actual
		* state of the HW) of the IHF
		*/
		TBool iIhfState;

		/**
		* Prefered volume of the IHF
		*/
		TInt iIhfVolume;

		/**
		* Used to tell the IHF change event listener, that the next IHF
		* change is not made with voice key and is to be ignored
		*/
		TBool iIgnoreNextIhfChange;

		/**
		* Indicates if the view is actually active, so that the model
		* knows when to ignore IHF change events
		*/
		TBool iActive;

		/**
		* Default speaker when playing
		*/
		TVRSpeaker iDefaultSpeaker;

		/**
		* Indicates if End key is pressed so view deactivation can trigger 
		* right operation ( paused memo is saved as well )
		*/
		TBool iIsEndKey;

		/*
		* Id for canceling waitnote
		*/
		TInt iNoteId;

		/*
		*
		*/
		TBool iIsNoteLaunched;

		/**
		* Default audio format for recording audio clip
		*/
		TInt iVRAudioFormat;

		/**
		* Allow In-call recording functionality
		*/
		TInt iVRAllowInCallRecording;


		/**
		* The call state retrieved on previous notification
		*/
		TInt iPreviousCallState;
		
		/**
		* Should the button panel reset itself on update
		*/
		TBool iResetButtons;
		
		/**
		* If model can handle commands. i.e. no critical command handling
		* just ongoing, e.g. waiting start tone play before starting
		* recording
		*/
		TBool iCanHandleCommands;
		
		/**
		* Pointer to the state info panel observer. Not owned.
		*/
		MVRStateInfoObserver* iStateInfoPanelObserver;
		
		/**
		* State of the CBA buttons. If EFalse LSK/RSK deliver ECmdNOP which
		* causes no action.
		*/
		TBool iCBAEnabled;
	};

#endif // __CVRRECVIEWMODEL_H__
