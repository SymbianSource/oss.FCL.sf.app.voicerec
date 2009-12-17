/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class that handles playback of tones in Voice Recorder application
*
*/


#ifndef CVRTONEPLAYER_H
#define CVRTONEPLAYER_H

// INCLUDES
#include <mdaaudiotoneplayer.h>

// FORWARD DECLARATIONS
class CAknKeySoundSystem;
class CAknSoundInfo;

/**
 *  Class that handles playback of tones in Voice Recorder application
 *
 *  @lib VoiceRecorderRecView.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CVRTonePlayer )
	: public CBase, public MMdaAudioToneObserver
    {

private:
	enum TVRTonePlayerState
		{
		EVRToneIdle,
		EVRTonePreparing,
		EVRTonePlaying
		};

public:

    static CVRTonePlayer* NewL();
    virtual ~CVRTonePlayer();

    /**
     * Prepares a tone for playback. This does not guarantee
     * that preparing is ready after completion.
     *
     * @since S60 v3.0
     * @param aToneId Predefined id of the tone to be prepared
     */
    void PrepareToneL( TInt aToneId );

    /**
     * Non-leaving method that calls PrepareToneL and ignores all leaves
     *
     * @since S60 v3.0
     * @param aToneId Predefined id of the tone to be prepared
     */
    void PrepareTone( TInt aToneId );

    /**
     * Starts playback of tone. Playback starts immediately if the tone
     * specified has been already been prepared. Otherwise the tone
     * is first prepared.
     *
     * @since S60 v3.0
     * @param aToneId Predefined id of the tone to be played
     */
    void PlayTone( TInt aToneId );

// from base class MMdaAudioToneObserver

    /**
     * From MMdaAudioToneObserver.
     * Defines required client behaviour when an attempt to configure the
	 * audio tone player utility has completed, successfully or otherwise.
     *
	 * @param     aError KErrNone if successful. One of the system
	 *            wide error codes if the attempt failed. The possible
	 *            values depend on the EPOC platform.
     */
    void MatoPrepareComplete(TInt aError);

    /**
     * From MMdaAudioToneObserver.
	 * Defines required client behaviour when the tone playing operation has
	 * completed, successfully or otherwise.
     *
	 * @param     aError KErrNone if successful. One of the system
	 *            wide error codes if the attempt failed. The possible
	 *            values depend on the EPOC platform.
     */
    void MatoPlayComplete(TInt aError);

private:

    CVRTonePlayer();
    void ConstructL();

private: // data

    /**
     * Instance of the player class.
     * Own.
     */
     CMdaAudioToneUtility* iPlayer;

	/**
	 * Pointer to sound system utility that retrieves the sound data.
	 * Not owned.
	 */
	 CAknKeySoundSystem* iSoundSystem;

	/**
	 * The id of the tone that is currently prepared
	 */
	 TInt iPreparedTone;

	/**
	 * The id of the tone that is in queue
	 */
	 TInt iQueuedTone;	
	 
	/**	
	 * Is a tone currently in preparation
	 */
	 TVRTonePlayerState iState;
	 
	 /**
	 * Used for waiting tone play comlete until continuing
	 */
	 CActiveSchedulerWait iShedulerWait;
    
    };

#endif // CVRTONEPLAYER_H
