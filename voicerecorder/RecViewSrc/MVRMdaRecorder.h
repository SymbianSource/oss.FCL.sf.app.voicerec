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
*     This interface acts as a high level wrapper around the media server
*     playback and recording functionalities. It is inherited by both 
*	  CVRMdaRecorder
*
*/


#ifndef __MVRMDARECORDER_H__
#define __MVRMDARECORDER_H__


// FORWARD DECLARATIONS
class CVRMemo;
class MVRObserver;

// CLASS DEFINITION
/**
* This class acts as a high level wrapper around the media server
* playback and recording functionalities.
*/
class MVRMdaRecorder 
{

	public: // new methods

		/**
		* Registers an observer for receiving update notifications.
		* @param aObserver The observer object.
		*/
		virtual void RegisterObserver( MVRObserver* aObserver ) = 0;
		
		/**
		* Unregisters a previously registered observer
		* @param aObserver The observer object.
		*/		
		virtual void UnregisterObserver( MVRObserver* aObserver ) = 0;

		/**
		* Registers an observer for receiving auto stop notifications.
		* @param aObserver The observer object.
		*/
		virtual void SetAutoStopObserver( MVRObserver* aObserver ) = 0;

		/**
		* Attachs this recorder to a memo file, i.e. opens it
		* and initialises media server.
		* @param aMemo Specifies the file to be attached to.
		*/
		virtual void AttachToMemoL( CVRMemo* aMemo ) = 0;

		/**
		* Creates a new memo file and attachs to it.
		* @param aMemo Specifies the file to be created.
		*/
		virtual void CreateNewMemoL( CVRMemo* aMemo ) = 0;

		/**
		* Detachs from the current memo, i.e. closes the file.
		*/
		virtual void DetachMemo() = 0;

		/**
		* Sets playback volume.
		* @param aStep Volume level in steps
		* @param aMaxSteps How many steps there are (10 usually).
		*/
		virtual void SetVolume( TInt aStep, TInt aMaxSteps )= 0;

		/**
		* Starts playback.
		* @return System wide error code
		*/
		virtual TInt PlayL() = 0;

		/**
		* Starts recording.
		* @return System wide error code
		*/
		virtual TInt RecordL() = 0;

		/**
		* Stops playback or recording.
		*/
		virtual void Stop() = 0;

		/**
		* Pauses playback or recording.
		*/
		virtual void Pause() = 0;

		/**
		* Moves the playing position forward 5 seconds.
		* @return System wide error code. KErrOverflow if the
		* position was forwarded past the end.
		*/
		virtual TInt ForwardL() = 0;

		/**
		* Moves the playing position backward 5 seconds.
		* @return System wide error code
		*/
		virtual TInt RewindL() = 0;

		/**
		* Indicates if we are in a state that can accept
		* more commands (i.e. all async calls have completed).
		* @return ETrue if it's ok to proceed calling methods.
		*/
		virtual TBool CanAcceptCommands() const = 0;

		/**
		* Set active speaker if feature is supported
		* @param aEnabled ETrue if playback should be routed to IHF
		* @return Error code returned by routing operation
		*/
		virtual TInt SetIhf( TBool aEnabled ) = 0;
		
		/**
		* Set the audio recorder position to a new value
		* @param aSeconds The position to set
		*/
		virtual void SetPositionL( TInt aSeconds ) = 0;
		
		virtual ~MVRMdaRecorder() {};
	};

#endif // __MVRMDARECORDER_H__
