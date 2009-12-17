/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Voice Recorder internal Central Repository keys
*
*/



#ifndef VOICERECORDERINTERNALCRKEYS_H
#define VOICERECORDERINTERNALCRKEYS_H

//  INCLUDES
#include <e32std.h>

// CONSTANTS

/******************************************************************************
* Voice Recorder Settings CentRep API
* Keys under this category are used in defining telephony
* variant configuration.
*/
const TUid KCRUidVoiceRecorder = {0x101F87A3};

/**
 * To allow or prohibit in-call recording.
 *
 * Possible values are 0 (no), 1 (yes).
 * Default value is 1.
 */
const TUint32 KVRAllowInCallRecording 		= 0x00000000;

/**
 * This key determines which audio format should be used as a default for
 * recording.
 *
 * Possible values = 0 (AMR), 1 (wav) and 2 (mp4)
 * Default value is 0.
 */
const TUint32 KVRDefaultAudioFormat 		= 0x00000001;

/**
 * Determines which speaker is used, when sound files are played by Voice
 * Recorder.
 *
 * Possible values are 0 ( ESpeakerEarPiece ) and 1 ( ESpeakerIhf )
 * Default value is 1.
 */
const TUint32 KVRDefaultSpeaker				= 0x00000002;

/**
 * Tells where to store music files recorder by Voice Recorder.
 *
 * Possible values are 0 ( EMemoStorePhoneMemory ) and 1 ( EMemoStoreMMC  )
 * Default value is 0.
 */
const TUint32 KVRMemoStore					= 0x00000003;

/**
 * Volume setting for the ear piece. Loaded on startup and saved when closing VR.
 *
 * Possible values are currently 0 to 10
 * Default value is 4.
 */
const TUint32 KVREarPieceVolume				= 0x00000004;

/**
 * Volume setting for the IHF. Loaded on startup and saved when closing VR.
 *
 * Possible values are currently 0 to 10
 * Default value is 4.
 */
const TUint32 KVRIHFVolume					= 0x00000005;

/**
 * Quality setting for recorded memos.
 *
 * Possible values are 0 (EQualityMMSOptimized), 1 (EQualityHigh AAC-LC supported, or EqualityNormal AAC-LC not supported ), 2 (only when AAC-LC is supported)
 */
const TUint32 KVRQuality					= 0x00000006;

/**
 * Maximum recording time for memo in minutes. Only used if no stricter
 * limit is set runtime (eg. file size limitation)
 */
const TUint32 KVRMaxRecordTime				= 0x00000007;

/**
* Bitmask specifying the elements that should be variated on or off in a
* release. See VoiceRecorderLocalVariation.txt for description of specific bit.
*/
const TUint32 KVRVariationFlags				= 0x00000008;

/**
* Count of recorded memos. This number is used in name of memo, eg. "Memo(25).amr"
*/
const TUint32 KVRMemoCount	    			= 0x00000009;

/**
* Bitrate used in AMR-NB recording
*
* Possible values: 4750, 5150, 5900, 6700, 7400, 7950, 10200, 12200
* Note that selected bitrate may not be supported by codec. In this case,
* the default bitrate of 12200 is used.
*/
const TUint32 KVRBitrateAmr	    			= 0x00000010;

/**
* Bitrate used in AAC-LC recording
*
* Possible values: 8000 - 288000
* Note that selected bitrate may not be supported by codec. In this case,
* the default bitrate of 32000 is used.
*/
const TUint32 KVRBitrateAac	    			= 0x00000011;

/**
* Sampling rate used in AAC-LC recording
*
* Possible values: 8, 11.025, 12, 16, 22.05, 24, 32, 44.1 and 48kHz
* Note that selected bitrate may not be supported by codec. In this case,
* the default sampling rate of 48 is used.
*/
const TUint32 KVRSamplerateAac   		= 0x00000012;

/**
* Audio mode (mono or stereo) flag
*
* Possible values:  1 (mono) and 2 (stereo)
* the default is 1.
*/
const TUint32 KVRStereoMonoFlag  		= 0x00000013;

/**
* AAC-LC recording Length
*
* Possible values: to be determined
* the default is 60 minutes.
*/
const TUint32 KVRAacRecordLength		= 0x00000014;




#endif      // VOICERECORDERINTERNALCRKEYS_H

// End of File
