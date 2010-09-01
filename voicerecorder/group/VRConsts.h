/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*     This header file defines some constants, string literals and
*     enumerations used globally in Voice Recorder.
*
*/


#ifndef __VRCONSTS_H__
#define __VRCONSTS_H__

// INCLUDES
#include <e32def.h>
#include <e32std.h>
#include <bldvariant.hrh>

// STRING LITERALS
_LIT( KVRAppResourceFile, "z:VoiceRecorder.rsc" );

_LIT( KVRBitmapFile, "z:VoiceRecorder.mbm" );
_LIT(KVRGSBitmapFile,"z:VoiceRecorder_aif.mif");

_LIT( KVRMemoStoreSharedDataKey, "MemoStore");
_LIT( KVRDefaultSpeakerSharedDataKey, "DefaultSpeaker" );
_LIT( KVRPanic, "VoiceRec" );

_LIT( KVRPluginFile,"z:GSVoiceRecorderPlugin.mif" );

// CONSTANTS
const TInt KVRBytes60SecAmr( 5000 );
const TInt KVRMaxTransitions( 13 );
const TInt KVRDefaultVolume( 4 );
const TInt KVRDefaultMaxLength( 60 );
const TInt KVRMaxVolumeSteps( 10 );
const TInt KVRSecondAsMicroSeconds( 1000000 );
const TInt KVRMinuteAsMicroSeconds( 60000000 );
const TInt KVRDisplayUpdateDelay( 250000 );

const TInt KVRHalfSecondAsMilliSeconds( 500 );
const TInt KVRSecondAsMilliSeconds( 1000 );
// Critical memory level that has to be taken into account when
// calculating estimate for Max recording time
//const TInt KVRCriticalMemoryLevel( 130000 ); // In Bytes

const TInt KVRAACCriticalMemoryLevel( 400000 ); // In Bytes

// This should so big that is covers 60 secs AMR or alternatively
// 5 secs WAV recording. 
const TInt KVRRecStartMemBuf( 100000 ); // In Bytes

const TInt KVRSendUiMsgSize( 1024 );

// AVKON volume control values go from 1 to 10
const TInt KVRVolumeControlMin( 1 );
const TInt KVRVolumeControlMax( 10 );

const TUint KVRDefaultAmrBitrate( 12200 );

// Time after which the maximum recording time label is updated for
// the first time in milliseconds
const TInt KVRFirstEstimateTime( 3000000 );

const TUint  KVRDefaultBitrateAac(64000);

const TInt KVRDefaultSamplerateAac(48000);

const TInt KVRDefaultStereoMonoFlag(1);

const TInt KVRDefaultAacRecordLength(60);

// ENUMERATIONS

// Don't change the order (snagged from SharedDataKeys.h)

enum TVRStateForVD
	{
	EVRIdle = 0,
	EVRRecording,
	EVRPlaying
	};

enum TVRAllowScreenSaver
	{
	EVRScreenSaverAllowed = 0,
	EVRScreenSaverNotAllowed
	};

enum TVRMemoStore
	{
	EMemoStorePhoneMemory,
	EMemoStoreMMC
	};

enum TVRPanic
	{
	EPanicUnknown,				// 0
	EPanicUnknownViewId,		// 1
	EPanicFunctionUnknown,		// 2
	EPanicInvalidContext,		// 3
	EPanicInvalidState,			// 4
	EPanicInvalidTransition,	// 5
	EPanicTooManyTransitions,	// 6
	EPanicNotAttached,			// 7
	EPanicAlreadyAttached,		// 8
	EPanicInvalidSetting		// 9
	};

// These values are used against a bitmask
// All values should be powers of 2
enum TVRFeature
	{
	EVRFeatureShowQualitySetting	= 1,
	EVRFeatureNext					= 2
	};
	
enum TVRFiletype
    {
    EVRFileAmr              = 1,
    EVRFileQCelp,
    EVRFileWav

#ifdef  __AAC_ENCODER_PLUGIN
    , EVRFileAAC_LC
#endif
    };

// Commands for special update events
enum TVRUpdateCommand
    {
    EVRUpdate               = 1,
    EVRUpdate1Second,           // Called when 1 second limit is passed during playback
    EVRUpdateStateChange,       // Called when state machine state has changed
    EVRUpdatePositionChange,	// Position changes so that update is needed
    EVRUpdateCBA				// Called when CBA buttons have to be updated
    };

#endif // __VRCONSTS_H__
