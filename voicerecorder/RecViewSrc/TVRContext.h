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
*     This class is a data container for a single RecView context.
*     A context contains a set of states. Each state
*     contains a set of valid state transitions.
*     Different contexts are defined in VoiceRecorder.hrh.
*
*
*/


#ifndef __TVRCONTEXT_H__
#define __TVRCONTEXT_H__

// INCLUDES
#include "voicerecorder.hrh"
#include "VRConsts.h"

// CLASS DEFINITION
/**
* This class is a data container for a single RecView context.
* A context contains a set of states. Each state
* contains a set of valid state transitions.
* Different contexts are defined in VoiceRecorder.hrh.
*/
class TVRContext
	{
	public:

		// CLASS DEFINITION
		/**
		* This class is a data container for a single RecView
		* state transition.
		*/
		class TTransition
			{
			public: // data

				/**
				* ETrue if this transition has been succesfully initialised.
				*/
				TBool iValid;

				/**
				* Identifies the command that executes this transition.
				* This is one of TVRCommands.
				*/
				TUint iCommandId;

				/**
				* Identifies the state that is entered by this transition.
				* This is an index into the TState::iStates array.
				*/
				TUint iNewState;

				/**
				* Identifies the function that gets called after the
				* state transition is complete.
				*/
				TUint iFunctionId;

			};

		// CLASS DEFINITION
		/**
		* This class is a data container for a single RecView
		* state. It contains a set of valid state transitions.
		*/
		class TState
			{
			public: // data

				/**
				* ETrue if this state has been succesfully initialised
				*/
				TBool iValid;

				/**
				* A static array of valid transitions (in this State).
				*/
				TTransition iTransitions[ KVRMaxTransitions ];
			
			};

	public: // data

		/**
		* ETrue if this context has been succesfully initialised
		*/
		TBool iValid;

		/**
		* A static array of valid states (in this context).
		*/
		TState iStates[ ENumStates ];

		/**
		* Defines the transition that is automatically
		* executed after entering this context.
		*/
		TTransition iInitialTransition;

	};

#endif // __TVRCONTEXT_H__
