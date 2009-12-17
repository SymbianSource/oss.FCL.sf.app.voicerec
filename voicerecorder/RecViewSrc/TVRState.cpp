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


// INCLUDE FILES
#include <barsread.h>	// TResourceReader
#include "TVRState.h"


// ================= MEMBER FUNCTIONS ========================================

// ---------------------------------------------------------------------------
// TVRState::ReadFromResource
// 
// ---------------------------------------------------------------------------
//
// Reads a STRUCT VR_STATE
void TVRState::ReadFromResource( TResourceReader& aReader )
	{
	iMenubarResourceId = aReader.ReadUint32();
	iCbaResourceId = aReader.ReadUint32();
	iStatus.Copy( aReader.ReadTPtrC() );
	iLabelSetId = TUint8( aReader.ReadUint8() );
	iHasVolumeControl = TUint8( aReader.ReadUint8() );
	iHasProgressBar = TUint8( aReader.ReadUint8() );
	iFocusButtonId = TUint8( aReader.ReadUint8() );
	iStateId = aReader.ReadUint8();
	
	// Reads an array of STRUCT VR_BUTTON_INFO
	iButtonCount = aReader.ReadInt16();
	for ( TInt i = 0; i < iButtonCount; i++ )
		{
		iButtons[ i ].ReadFromResource( aReader );
		}
	}


// ---------------------------------------------------------------------------
// TVRState::TButton::ReadFromResource
// Reads a STRUCT VR_BUTTON 
// ---------------------------------------------------------------------------
//
void TVRState::TButton::ReadFromResource( TResourceReader& aReader )
	{
	iCommandId = aReader.ReadUint16();
	iState = TUint8( aReader.ReadUint8() );
	}


// ---------------------------------------------------------------------------
// TVRState::ButtonState
// 
// ---------------------------------------------------------------------------
//
TUint TVRState::ButtonState( TInt aButtonId ) const
	{
	// aButtonId cannot be out of bounds
	return iButtons[ aButtonId ].iState;
	}


// ---------------------------------------------------------------------------
// TVRState::ButtonCommandId
// 
// ---------------------------------------------------------------------------
//
TUint TVRState::ButtonCommandId( TInt aButtonId ) const
	{
	// aButtonId cannot be out of bounds
	return iButtons[ aButtonId ].iCommandId;
	}

