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
*     This class is an extension to the standard Eikon label.
*     It provides leave-safe SetText() and is able to
*     clear its background before drawing.
*
*/


// INCLUDE FILES

#include <AknUtils.h>
#include <AknsUtils.h>
#include <AknBidiTextUtils.h>
#include <AknsDrawUtils.h>

#include "CVRLabel.h"
#include "voicerecorder.hrh"


// ================= MEMBER FUNCTIONS ========================================

// ---------------------------------------------------------------------------
// CVRLabel::NewLC
// 
// ---------------------------------------------------------------------------
//	
CVRLabel* CVRLabel::NewLC( const CCoeControl& aContainer )
	{
	CVRLabel* self = new( ELeave ) CVRLabel;
	CleanupStack::PushL( self );
	self->ConstructL( aContainer );

	return self;
	}


// ---------------------------------------------------------------------------
// CVRLabel::~CVRLabel
// 
// ---------------------------------------------------------------------------
//	
CVRLabel::~CVRLabel()
	{
	}


// ---------------------------------------------------------------------------
// CVRLabel::ConstructL
// 
// ---------------------------------------------------------------------------
//	
void CVRLabel::ConstructL( const CCoeControl& aContainer )
	{
	SetContainerWindowL( aContainer );
	SetBufferReserveLengthL( VRLABELMAXLENGTH );
	iSkin = AknsUtils::SkinInstance();
	}


// ---------------------------------------------------------------------------
// CVRLabel::Draw
// Clears the label area and then draws the text
// ---------------------------------------------------------------------------
//	
void CVRLabel::Draw( const TRect& aRect ) const
	{
	// Acquire the control context through the MOP-chain
	MAknsControlContext* context = AknsDrawUtils::ControlContext( this );

	// Draw the background using the control context
	// Note: if there is no skin, the background is just cleared 
	// (which is OK for us)
	AknsDrawUtils::Background( iSkin, context, this, SystemGc(), Rect() );
	
	// Draw the actual text
	CEikLabel::Draw( aRect );
	}


// ---------------------------------------------------------------------------
// CVRLabel::SetText
// Non-leaving SetText variant.
// ---------------------------------------------------------------------------
//	
void CVRLabel::SetText( const TDesC& aText )
	{
	TBuf< VRLABELMAXLENGTH > clipped( aText );
	// will Panic if clipped is longer than VRLABELMAXLENGTH
	// AknTextUtils::ClipToFit( clipped, *iFont, iSize.iWidth );
        // AknBidiTextUtils::ConvertToVisualAndClipL(clipped,*iFont,iSize.iWidth, iSize.iWidth );
	*iText = clipped;
	}


// ---------------------------------------------------------------------------
// CVRLabel::SetTextAndDraw
// Updates the label if the given text is differentfrom the label's 
// current text.
// ---------------------------------------------------------------------------
//	
void CVRLabel::SetTextAndDraw( const TDesC& aText )
	{
	if ( iText->Compare( aText ) )
		{
		SetText( aText );
		DrawNow();
		}
	}

