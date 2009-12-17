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
*     An empty control that is displayed until application is fully
*	  constructed
*
*/


// INCLUDE FILES

#include <AknDef.h> // For layout change event definitions
#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <AknsBasicBackgroundControlContext.h>

#include "CVRRecViewActivationContainer.h"


// ================= MEMBER FUNCTIONS ========================================

// ---------------------------------------------------------------------------
// CVRRecViewActivationContainer::CVRRecViewActivationContainer
//
// ---------------------------------------------------------------------------
//
CVRRecViewActivationContainer::CVRRecViewActivationContainer()
	{
	}


// ---------------------------------------------------------------------------
// CVRRecViewActivationContainer::~CVRRecViewActivationContainer
//
// ---------------------------------------------------------------------------
//
CVRRecViewActivationContainer::~CVRRecViewActivationContainer()
	{
	delete iBackgroundSkinContext;	
	}


// ---------------------------------------------------------------------------
// CVRRecViewActivationContainer::ConstructL
//
// ---------------------------------------------------------------------------
//
void CVRRecViewActivationContainer::ConstructL( const TRect& aRect )
	{
	CreateWindowL();

	// Get the skin instance
	iSkinInstance = AknsUtils::SkinInstance();

	// Create background control context for skinning the background
	iBackgroundSkinContext = CAknsBasicBackgroundControlContext::NewL( 
			KAknsIIDQsnBgAreaMain, aRect, ETrue );

    SetRect( aRect );
	} 


// ---------------------------------------------------------------------------
// CVRRecViewActivationContainer::HandleResourceChange
//
// ---------------------------------------------------------------------------
//
void CVRRecViewActivationContainer::HandleResourceChange(TInt /*aType*/)
	{
	}


// ---------------------------------------------------------------------------
// CVRRecViewActivationContainer::CountComponentControls
//
// ---------------------------------------------------------------------------
//
TInt CVRRecViewActivationContainer::CountComponentControls() const
	{
	return 0;
	}


// ---------------------------------------------------------------------------
// CVRRecViewActivationContainer::ComponentControl
//
// ---------------------------------------------------------------------------
//
CCoeControl* CVRRecViewActivationContainer::ComponentControl( 
													TInt /*aIndex*/ ) const
	{
	return NULL;
	}



// ---------------------------------------------------------------------------
// CVRRecViewActivationContainer::OfferKeyEventL
//
// ---------------------------------------------------------------------------
//
TKeyResponse CVRRecViewActivationContainer::OfferKeyEventL( 
		const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/ )
	{
	return EKeyWasNotConsumed;
	}


// ---------------------------------------------------------------------------
// CVRRecViewActivationContainer::SizeChanged
//
// ---------------------------------------------------------------------------
//
void CVRRecViewActivationContainer::SizeChanged()
	{
	TRect bgRect;
	AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, bgRect);
	iBackgroundSkinContext->SetRect( bgRect );		
	}


// ---------------------------------------------------------------------------
// CVRRecViewActivationContainer::Draw
//
// ---------------------------------------------------------------------------
//
void CVRRecViewActivationContainer::Draw( const TRect& /*aRect*/ ) const
	{
	CWindowGc& gc = SystemGc();

	// Fill the background with the background bitmap from the skin
	// Note: if the skin doesn't define a background the area is simply
	// cleared (which is OK for us).
	AknsDrawUtils::Background( iSkinInstance, iBackgroundSkinContext,
			this, gc, Rect() );

	}


// ---------------------------------------------------------------------------
// CVRRecViewActivationContainer::MopSupplyObject
//
// ---------------------------------------------------------------------------
//
TTypeUid::Ptr CVRRecViewActivationContainer::MopSupplyObject(TTypeUid aId)
	{
	
	if ( aId.iUid == MAknsControlContext::ETypeId )
		{
		// Return the control context for the childs
		return MAknsControlContext::SupplyMopObject( aId, 
													 iBackgroundSkinContext );
		
		}
	return CCoeControl::MopSupplyObject( aId );
	}

