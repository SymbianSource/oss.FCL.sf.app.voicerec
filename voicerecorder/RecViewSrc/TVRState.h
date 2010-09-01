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
*     This class is a data container for a single RecView visual (UI) state
*     It is able to read its parameters from a resource file.
*     The run-time size of a single TVRState instance is 152 bytes
*     (unicode build)
*
*/


#ifndef __TVRSTATE_H__
#define __TVRSTATE_H__

// INCLUDES
#include <e32def.h>
#include "voicerecorder.hrh"

// CONSTANTS
const TInt KVRMaxButtons( 5 );	// defined by UI spec

// FORWARD DECLARATIONS
class TResourceReader;

// CLASS DEFINITION
/**
* This class is a data container for a single Voice Recorder visual (UI) state
* It is able to read its parameters from a resource file.
* The run-time size of a single TVRState instance is 152 bytes (unicode build)
*/
class TVRState
	{
	private: // nested classes

		// CLASS DEFINITION
		/**
		* This class is a data container for a single CVRButtonPanel button
		* defeinition. It is able to read its parameters from a resource file.
		* The run-time size of a single TButton instance is 12 bytes.
		*/
		class TButton
			{
			public: // new methods

				/**
				* Constructs this button from a VR_BUTTON resource
				* @param aReader A resource reader object initialised
				*		 to a resource.
				*/
				void ReadFromResource( TResourceReader& aReader );

			public: // data

				/**
				* The command id of this button (for HandleCommandL use).
				*/
				TUint iCommandId;

				/**
				* The visual state of this button
				* (see TVRButtonStates in VoiceRecorder.hrh)
				*/ 
				TUint8 iState;

			};

	public: // new methods

		/**
		* Constructs this state from a VR_STATE resource
		* @param aReader A resource reader object initialised to a resource.
		*/
		void ReadFromResource( TResourceReader& aReader );

		/**
		* Returns the state of a button.
		* @param aButtonId Identifies a button in iButtons array;
		* @return The current state of this button. See TButton.
		*/
		TUint ButtonState( TInt aButtonId ) const;

		/**
		* Returns the command id of a button.
		* @param aButtonId Identifies a button in iButtons array;
		* @return The command id of this button. See TButton.
		*/
		TUint ButtonCommandId( TInt aButtonId ) const;

	public: // data
		
		/**
		* An unique identifier for this visual state.
		*/	
		TUint iStateId;
		
		/**
		* An unique identifier for this state.
		*/
		TUint iId;

		/**
		* Identifies the menu bar resource for this state (or null).
		*/
		TUint iMenubarResourceId;

		/**
		* Identifies the softkey resource for this state (or null).
		*/
		TUint iCbaResourceId;

		/**
		* Identifies the button that should gain initial focus.
		*/
		TUint8 iFocusButtonId;

		/**
		* Nonzero if this state should have a volume control.
		*/
		TUint8 iHasVolumeControl;

		/**
		* Nonzero if this state should have a progress bar.
		*/
		TUint8 iHasProgressBar;

		/**
		* Identifies the set of labels for this state.
		*/
		TUint8 iLabelSetId;

		/**
		* The number of buttons in this state.
		*/
		TInt iButtonCount;

		/**
		* Status text for this state (or KNullDesc).
		*/
		TBuf< VRLABELMAXLENGTH > iStatus;

	private: // data

		/**
		* An array of buttons in this state.
		* The run-time size of iButtons is 60 bytes.
		*/
		TButton iButtons[ KVRMaxButtons ];

	};

#endif	// __TVRSTATE_H__
