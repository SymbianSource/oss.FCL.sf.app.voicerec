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
*     Defines commands and control identifiers.
*     The file can be included in C++ or resource file.
*
*/


#ifndef __VOICERECORDERCONTEXTS_H_
#define __VOICERECORDERCONTEXTS_H_

// Recorder View context IDs
enum TVRRecViewContexts
	{
	EContextNormal = 0,
	EContextEmptyNormal,
	EContextRecordNewNormal,
	EContextRecordNewForRemote,
	EContextViewer,
	EContextNokiaRingTone,
	ENumContexts	// this must be the last line
	};

#endif // __VOICERECORDERCONTEXTS_H_

