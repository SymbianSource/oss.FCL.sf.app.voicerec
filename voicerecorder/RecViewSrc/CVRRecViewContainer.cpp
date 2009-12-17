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
*     A compound control class. It is responsible for owning CVRButtonPanel
*     and CVRStateInfoPanel. It also handles the changing of softkeys, menubar
*     and volume control.
*
*/


// INCLUDE FILES

#include <eikenv.h>
#include <bldvariant.hrh>

#include <aknnavi.h>
#include <aknnavide.h>
#include <eikspane.h>
#include <avkon.rsg>
#include <avkon.hrh>
#include <aknappui.h>
#include <aknsoundsystem.h>
#include <aknview.h>
#include <eikbtgpc.h>
#include <eikmenub.h>
#include <AknUtils.h>
#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <AknsBasicBackgroundControlContext.h>
#include <akncontext.h>
#include <applayout.cdl.h>
#include <voicerecorder.rsg>
#include <AknDef.h>		// For layout change event definitions
#include <AknVolumePopup.h>
#include <featmgr.h>	// Feature manager

#include "CVRRecViewContainer.h"
#include "CVRButtonPanel.h"
#include "CVRStateInfoPanel.h"
#include "MVRDecoratorModel.h"
#include "MVRVolumeChangeObserver.h"
#include "CVRRecView.h"
#include "MVRKeyObserver.h"
#include "CVRRemConObserver.h"
#include "VRConsts.h"

#include <VoiceRecorderUID.h>

#include "VRUtils.h"

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CVRRecViewContainer::CVRRecViewContainer
//
// ----------------------------------------------------------------------------
//
CVRRecViewContainer::CVRRecViewContainer()
	{
	}


// ----------------------------------------------------------------------------
// CVRRecViewContainer::~CVRRecViewContainer
//
// ----------------------------------------------------------------------------
//
CVRRecViewContainer::~CVRRecViewContainer()
	{
	if ( iModel )
		{
		iModel->SetDecoratorObserver( NULL );
		}
	delete iButtonPanel;
	delete iStateInfoPanel;
	delete iIhfVolumeControl;
	delete iEarPieceVolumeControl;

	delete iBackgroundSkinContext;
	
	delete iRemConObserver;

	}


// ----------------------------------------------------------------------------
// CVRRecViewContainer::ConstructL
//
// ----------------------------------------------------------------------------
//
void CVRRecViewContainer::ConstructL( const TRect& aRect,
									  MVRButtonPanelModel* aButtonModel,
									  MVRDecoratorModel* aDecoratorModel,
									  MVRStateInfoModel* aStateInfoModel,
									  CVRRecView* aOwningView )
	{
	CreateWindowL();

	iRemConObserver = CVRRemConObserver::NewL( this );

	// Get the skin instance
	iSkinInstance = AknsUtils::SkinInstance();
	if ( !iSkinInstance )
	    {
	    User::Leave( KErrNoMemory );
	    }

	// Create background control context for skinning the background
	iBackgroundSkinContext = CAknsBasicBackgroundControlContext::NewL( 
			KAknsIIDQsnBgAreaMain, aRect, ETrue );

	// Refer to Series 60 European Application LAF for coordinate descriptions
	iButtonPanel = new( ELeave ) CVRButtonPanel( aButtonModel, 
												 aOwningView->Id() );
	iButtonPanel->SetContainerWindowL( *this );
	iButtonPanel->ConstructL();
	iButtonPanel->SetMopParent( this );
	TAknWindowLineLayout buttonPanelLayout = AppLayout::grid_vorec_pane();   
    AknLayoutUtils::LayoutControl( iButtonPanel, Rect(), buttonPanelLayout );

	// Refer to Series 60 European Application LAF for coordinate descriptions
	iStateInfoPanel = new( ELeave ) CVRStateInfoPanel( aStateInfoModel, 
													   aOwningView->Id() );
	iStateInfoPanel->SetContainerWindowL( *this );
	iStateInfoPanel->ConstructL();
    iStateInfoPanel->SetRect( TRect( TPoint( 0,0 ), aRect.Size() ) );

	// Set state info panel's MOP-parent so that it can get hold of the bg 
	// control context
	iStateInfoPanel->SetMopParent( this );

	iNaviPane = static_cast< CAknNavigationControlContainer* >(
				iEikonEnv->AppUiFactory()->StatusPane()->ControlL(
				TUid::Uid( EEikStatusPaneUidNavi ) ) );


	// Volume indicator with IHF icon
	iIhfVolumeControl = iNaviPane->CreateVolumeIndicatorL(
										 R_AVKON_NAVI_PANE_VOLUME_INDICATOR );
    // when touch UI is enabled
    if ( AknLayoutUtils::PenEnabled() )
        {
        static_cast<CAknVolumeControl*>
                  ( iIhfVolumeControl->DecoratedControl() )->SetObserver( this );     
        }
										     
    iEarPieceVolumeControl = iNaviPane->CreateVolumeIndicatorL(
	                            R_AVKON_NAVI_PANE_EARPIECE_VOLUME_INDICATOR );

    // when touch UI is enabled
    if ( AknLayoutUtils::PenEnabled() )
        {
        static_cast<CAknVolumeControl*>
                  ( iEarPieceVolumeControl->DecoratedControl() )->SetObserver( this );     
        }

    iIhfVolumeControl->MakeVisible( ETrue );
    iEarPieceVolumeControl->MakeVisible( ETrue );

	iModel = aDecoratorModel;
	iModel->SetDecoratorObserver( this );

	iOwningView = aOwningView;

    SetRect( aRect );
	// Update child control rects and other co-ordinates for current
	// layout ( Euro/APAC/Arabic&Hebrew )
	UpdateLayoutL();
	} 


// ----------------------------------------------------------------------------
// CVRRecViewContainer::GetHelpContext
//
// ----------------------------------------------------------------------------
//
void CVRRecViewContainer::GetHelpContext( TCoeHelpContext& aContext ) const
	{
	aContext.iMajor = TUid::Uid( KVoiceRecorderAppUID3 );
	if ( iModel )
		{
		iModel->GetHelpContext( aContext );
		}
	}


// ----------------------------------------------------------------------------
// CVRRecViewContainer::CountComponentControls
//
// ----------------------------------------------------------------------------
//
TInt CVRRecViewContainer::CountComponentControls() const
	{
	return 2;

	}


// ----------------------------------------------------------------------------
// CVRRecViewContainer::ComponentControl
//
// ----------------------------------------------------------------------------
//
CCoeControl* CVRRecViewContainer::ComponentControl( TInt aIndex ) const
	{
	switch ( aIndex )
		{
		case 0:
			{
			return iButtonPanel;
			}
		case 1:
			{
			return iStateInfoPanel;
			}

		case 2:
			{
			return iIhfVolumeControl->DecoratedControl();
			}
		case 3:
			{
            return iEarPieceVolumeControl->DecoratedControl();
			} 

		default:
			{
			return NULL;
			}
		}
	}



// ----------------------------------------------------------------------------
// CVRRecViewContainer::OfferKeyEventL
//
// ----------------------------------------------------------------------------
//
TKeyResponse CVRRecViewContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent,
												  TEventCode aType )
	{
	TBool consumed( EFalse );

			if ( aType == EEventKeyDown )
				{
				aType = aType;
				}

	if ( iKeyObserver )
		{
		consumed = iKeyObserver->ProcessKeyEventL( aKeyEvent, aType );
		}

	if ( !consumed )
		{
		if ( iActiveVolumeControl && iVolumeChangeObserver->CanSetVolume() && 
			   !FeatureManager::FeatureSupported( KFeatureIdSideVolumeKeys ) )
			{
			if ( ( aKeyEvent.iCode == EKeyLeftArrow ) ||
				 ( aKeyEvent.iCode == EKeyRightArrow ) )
				{
				HandleVolumeChangeL( aKeyEvent, aType );
				}
			}

		return iButtonPanel->OfferKeyEventL( aKeyEvent, aType );
		}

	return EKeyWasNotConsumed;

	}


// ----------------------------------------------------------------------------
// CVRRecViewContainer::SizeChanged
//
// ----------------------------------------------------------------------------
//
void CVRRecViewContainer::SizeChanged()
	{
	iStateInfoPanel->SetRect( Rect() );

	TRect bgRect;
	AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, bgRect);
	iBackgroundSkinContext->SetRect( bgRect );	

	TAknWindowLineLayout buttonPanelLayout = AppLayout::grid_vorec_pane();   
    AknLayoutUtils::LayoutControl( iButtonPanel, Rect(), buttonPanelLayout );

	TAknWindowLineLayout statusPaneLayout = 
								AknLayout::status_pane( Rect(), 0 );
								
    TAknLayoutRect statusPanelRect;
    statusPanelRect.LayoutRect( Rect(), statusPaneLayout );

	TAknWindowLineLayout verticalLineLayout = 
								AppLayout::Voice_Recorder_elements_Line_2();
								
	iVerticalLineRect.LayoutRect( statusPanelRect.Rect(), verticalLineLayout );

	TAknWindowLineLayout horizontalLineLayout = 
								AppLayout::Voice_Recorder_elements_Line_3();
								
	iHorizontalLineRect.LayoutRect( statusPanelRect.Rect(), 
									horizontalLineLayout );
	}



// ----------------------------------------------------------------------------
// CVRRecViewContainer::Draw
//
// ----------------------------------------------------------------------------
//
void CVRRecViewContainer::Draw( const TRect& /*aRect*/ ) const
	{
	CWindowGc& gc = SystemGc();

	// Fill the background with the background bitmap from the skin
	// Note: if the skin doesn't define a background the area is simply
	// cleared (which is OK for us).
	AknsDrawUtils::Background( iSkinInstance, iBackgroundSkinContext,
			this, gc, Rect() );

	iVerticalLineRect.DrawRect( gc );
	iHorizontalLineRect.DrawRect( gc );
	}


// ----------------------------------------------------------------------------
// CVRRecViewContainer::SetKeyObserver
//
// ----------------------------------------------------------------------------
//
void CVRRecViewContainer::SetKeyObserver( MVRKeyObserver* aObserver )
	{
	iKeyObserver = aObserver;
	}


// ----------------------------------------------------------------------------
// CVRRecViewContainer::SetVolumeChangeObserver
//
// ----------------------------------------------------------------------------
//
void CVRRecViewContainer::SetVolumeChangeObserver(
										  MVRVolumeChangeObserver* aObserver )
	{
	iVolumeChangeObserver = aObserver;
	}


// ----------------------------------------------------------------------------
// CVRRecViewContainer::Update
//
// ----------------------------------------------------------------------------
//
void CVRRecViewContainer::Update( TVRUpdateCommand aCommand )
	{
	
	if ( aCommand == EVRUpdateCBA )
		{
		UpdateCBA();
		return;
		}
		
	// Change menubar
	iOwningView->MenuBar()->SetMenuTitleResourceId(
												iModel->MenuBarResourceId() );
	
	// error ignored, this rarely leaves but just to make sure
	TRAP_IGNORE( iNaviPane = static_cast< CAknNavigationControlContainer* >(iEikonEnv->AppUiFactory()->StatusPane()->ControlL( // CSI: 42 #
				TUid::Uid( EEikStatusPaneUidNavi ) ) ) ); 

	iNaviPane->DrawNow();
	
	UpdateCBA();

	TRAP_IGNORE( UpdateVolumeControlL() );
	}


// ----------------------------------------------------------------------------
// CVRRecViewContainer::UpdateCBA
// Updates the CBA buttons (LSK, MSK & RSK) according to the current state and 
// draws them
// ----------------------------------------------------------------------------
//
void CVRRecViewContainer::UpdateCBA()
	{
	// Change CBA buttons
	// Ignore any errors. SetCommandSetL() will leave if there is
	// not enough memory
	CEikButtonGroupContainer* cba = iOwningView->Cba();
	
	// e.g. recording is about to start and options menu shouldn't be opened
	if ( iModel->CbaResourceId() == R_VR_SOFTKEYS_OPTIONS_EXIT_SELECT &&
			!iModel->CBAEnabled() )
		{
		// Same texts remain but disables LSK and RSK use
		TRAP_IGNORE( cba->SetCommandSetL( 
						R_VR_SOFTKEYS_OPTIONS_EXIT_SELECT_NO_COMMAND ) );
		}
	
	// jeffery: added for CR to disable "Pause" key, will remove it later when plugin is updatedd

#ifdef  __AAC_ENCODER_PLUGIN   
	if (!iOwningView->IsEmbedded() && (iOwningView->Quality() == EQualityHigh) && (iOwningView->GetStateId() == EStateRecording ))

    	{
    	TRAP_IGNORE(cba->SetCommandSetL(R_VR_SOFTKEYS_STOP_SELECT));    	 
    	} 
#endif
	
	else
		{
		// Normal behaviour
		TRAP_IGNORE( cba->SetCommandSetL( iModel->CbaResourceId() ) );	
		}	
	
	cba->DrawNow();
	}


// ----------------------------------------------------------------------------
// CVRRecViewContainer::UpdateVolumeControlL
//
// ----------------------------------------------------------------------------
//
void CVRRecViewContainer::UpdateVolumeControlL()
	{
    TVRVolumeControlState state; 	
    TInt volume;

    // Get volume control data from model
    iModel->GetVolumeControlState( state, volume );

    if ( iActiveVolumeControl )
	    {
	    // pop the previous volume control if there was one
	    iNaviPane->Pop();
	    }

    switch ( state )
	    {
	    case EDisabled:
		    {
		    iActiveVolumeControl = NULL;
		    break;
		    }

	    case EIhf:
		    {

		    iActiveVolumeControl = iIhfVolumeControl;

		    break;
		    }

	    case EEarPiece:
		    {

		    iActiveVolumeControl = iEarPieceVolumeControl;
		    break;
		    }
        }

    if ( state != EDisabled )
	    {
	    iNaviPane->PushL( *iActiveVolumeControl );
	    CAknVolumeControl* control = static_cast< CAknVolumeControl* >(
								    iActiveVolumeControl->DecoratedControl() );
	    
	    
	    control->SetValue( volume );
	    }
	}


// ----------------------------------------------------------------------------
// CVRRecViewContainer::UpdateLayoutL
//
// ----------------------------------------------------------------------------
//
void CVRRecViewContainer::UpdateLayoutL()
	{
	SizeChanged();
	}


// ----------------------------------------------------------------------------
// CVRRecViewContainer::MopSupplyObject
//
// ----------------------------------------------------------------------------
//
TTypeUid::Ptr CVRRecViewContainer::MopSupplyObject(TTypeUid aId)
	{
	if ( aId.iUid == MAknsControlContext::ETypeId )
		{
		// Return the control context for the childs
		return MAknsControlContext::SupplyMopObject( aId, iBackgroundSkinContext );
		}
	return CCoeControl::MopSupplyObject( aId );
	}


// ----------------------------------------------------------------------------
// CVRRecViewContainer::HandleResourceChangeL
//
// ----------------------------------------------------------------------------
//
void CVRRecViewContainer::HandleResourceChangeL( TInt aType )
	{
	CCoeControl::HandleResourceChange( aType );
	
	if ( aType == KEikDynamicLayoutVariantSwitch )
		{
		UpdateLayoutL();
		}
	else if ( aType == KAknsMessageSkinChange )
		{
		// New skin instance is loaded
		iSkinInstance = AknsUtils::SkinInstance();
		}
	
	// Inform other controls	
	iButtonPanel->HandleResourceChangeL( aType );
	iStateInfoPanel->HandleResourceChangeL( aType );	
		
	}


// ----------------------------------------------------------------------------
// CVRRecViewContainer::HandleVolumeChangeL
//
// ----------------------------------------------------------------------------
//
TInt CVRRecViewContainer::HandleVolumeChangeL( const TKeyEvent& aKeyEvent,
											   TEventCode aType )
	{
	
	if( !iActiveVolumeControl || !iVolumeChangeObserver->CanSetVolume() )
		{
		// Can't process volume change yet
		return KErrNotReady;
		}
	
	CAknAppUi* appUi = static_cast< CAknAppUi* >(
										  iEikonEnv->EikAppUi() );
	CAknVolumeControl* control = static_cast< CAknVolumeControl* >
					( iActiveVolumeControl->DecoratedControl() );

	TInt prevLevel( control->Value() );
	
	control->OfferKeyEventL( aKeyEvent, aType );

	// Values range from 1 to 10 so the cast is ok
	TUint8 newValue( ( TUint8 ) control->Value() );

	if ( iVolumeChangeObserver )
		{
		iVolumeChangeObserver->SetVolume( newValue,
										  KVRMaxVolumeSteps );
		}

	// determine if the volume was already at either end
	// (of the scale)
	// play a system sound in that case.
	if ( prevLevel == control->Value() )
		{
		TInt sid( EAvkonSIDVolumeMaxTone );
		if ( prevLevel == 1 )
			{
			sid = EAvkonSIDVolumeMinTone;
			}
		appUi->KeySounds()->PlaySound( sid );
		return KErrTooBig;
		}	
		
	return KErrNone;
	}
	

// ---------------------------------------------------------------------------
// CVRRecViewContainer::HandlePointerEventL
// Handles pointer events by passing them on to controls
// ---------------------------------------------------------------------------
//		
void CVRRecViewContainer::HandlePointerEventL( const TPointerEvent& aPointerEvent )
	{
	if( AknLayoutUtils::PenEnabled() )
		{
		CCoeControl::HandlePointerEventL(aPointerEvent);	
		iButtonPanel->HandlePointerEventL(aPointerEvent);

		if ( iActiveVolumeControl && iVolumeChangeObserver->CanSetVolume() )
	    	{
	    	if ( (iOwningView->GetStateId() == EStatePlayingPaused ) || (iOwningView->GetStateId() == EStatePlaying ) )
	    		{
	    		// pop the previous volume control if there was one
	    		CAknVolumeControl* control = static_cast< CAknVolumeControl* >
	    						( iActiveVolumeControl->DecoratedControl() );
	    		CAknVolumePopup* popup = static_cast<CAknVolumePopup*> ( control->Parent() );
	    		TRect volumePopupRect( popup->Rect() );
	    		volumePopupRect.Move( popup->Position() );
	    		if( volumePopupRect.Contains(aPointerEvent.iParentPosition) )
	    			{
	    			TRAP_IGNORE( popup->ShowVolumePopupL() );
	    			}
		    	}
	    	}
		}
	}

// ---------------------------------------------------------------------------
// CVRRecViewContainer::FocusedButton
// Returns the currently focused button in Button Panel
// ---------------------------------------------------------------------------
//		
TInt CVRRecViewContainer::FocusedButton() const
	{
	return iButtonPanel->FocusedButton();
	}


// ---------------------------------------------------------------------------
// CVRRecViewContainer::UpdateButtonPanel
// Gives the update command to Button Panel
// ---------------------------------------------------------------------------
//
void CVRRecViewContainer::UpdateButtonPanel( TVRUpdateCommand aCommand )
	{
	iButtonPanel->Update( aCommand );
	}

	
// ----------------------------------------------------------------------------
// void CVRRecViewContainer::HandleControlEventL
// ----------------------------------------------------------------------------
//
void CVRRecViewContainer::HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType )
	{
    // Added for touch UI to adjust volume
	if( AknLayoutUtils::PenEnabled() )
		{			
		switch( aEventType )
			{
			case EEventStateChanged:
				{
				if ( aControl == static_cast< CAknVolumeControl*>(iActiveVolumeControl->DecoratedControl()))
 				    {
						if( !iActiveVolumeControl || !iVolumeChangeObserver->CanSetVolume() )
						{
						// Can't process volume change yet
						User::Leave(KErrNotReady);
						}
	
						CAknAppUi* appUi = static_cast< CAknAppUi* >(iEikonEnv->EikAppUi() );
						CAknVolumeControl* control = static_cast< CAknVolumeControl* >(iActiveVolumeControl->DecoratedControl() );

						TInt prevLevel( control->Value() );
	
						// Values range from 1 to 10 so the cast is ok
						TUint8 newValue( ( TUint8 ) control->Value() );

						if ( iVolumeChangeObserver )
							{
							iVolumeChangeObserver->SetVolume( newValue, KVRMaxVolumeSteps );
							}

						// determine if the volume was already at either end
						// (of the scale)
						// play a system sound in that case.
						if ( prevLevel == control->Value() )
							{
							TInt sid( EAvkonSIDVolumeMaxTone );
							if ( prevLevel == 1 )
								{
								sid = EAvkonSIDVolumeMinTone;
								}
							appUi->KeySounds()->PlaySound( sid );
							User::Leave(KErrTooBig);
							}	
 				    }
                }
			default:
				break;                
            }
        }
    }
