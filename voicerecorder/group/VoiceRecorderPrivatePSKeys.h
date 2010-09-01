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
* Description:  Voice Recorder Internal Publish & Subscribe keys
*
*/



#ifndef VOICERECORDERPSKEYS_H
#define VOICERECORDERPSKEYS_H

//  INCLUDES
#include <e32std.h>

// CONSTANTS

/******************************************************************************
* Voice Recorder Private Settings Pub&Sub API
* Keys under this category are used within Voice Recorder
*/
const TUid KPSUidVoiceRecorder = {0x101F87A4};

/**
 * Current mode of Voice Recorder. (Recording, playing etc.)
 * Possible values are 	EIdle = 0, ERecording,	EPlaying
 * Default value is 0.
 */
const TUint32 KVoiceRecorderMode	= 0x00000000;

#endif      // VOICERECORDERPSKEYS_H

// End of File
