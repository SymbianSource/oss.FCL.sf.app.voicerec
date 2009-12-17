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
*     A compound control class. It is responsible for drawing and updating
*     a set of buttons. Button selections are also handled and forwarded
*     as commands to the AppUi.
*
*/


// INCLUDE FILES

#include <avkon.hrh>
#include <eikenv.h> 
#include <AknsUtils.h> 
#include <voicerecorder.rsg> 
#include <aknbutton.h> 					
#include <e32base.h> 					
#include <barsread.h> 					// TResourceReader
#include <w32std.h>
#include <aknlayoutscalable_apps.cdl.h> // New button highlight
#include <e32std.h> 					// Timer
#include <AknsDrawUtils.h>
#include <applayout.cdl.h>
#include <AknsConstants.h> 
#include <AknIconUtils.h>
#include <data_caging_path_literals.hrh>
#include <eikappui.h> 
#include <aknViewAppUi.h>
#include <aknview.h>

#include <voicerecorder.mbg>
#include "CVRButtonPanel.h"
#include "MVRButtonPanelModel.h"
#include "voicerecorder.hrh"
#include "VRConsts.h"

// CONSTANTS

// Timer delays
const TInt KVRFastForwStartDelay( 500000 );
const TInt KVRFastForwUpdateDelay( 170000 );

const TInt KVRButtonCount( 5 );			   // Total number of buttons
const TInt KVRAmountOfFocusShrink( 5 );


// ================= MEMBER FUNCTIONS ========================================

// ---------------------------------------------------------------------------
// CVRButtonPanel::CVRButtonPanel
// 
// ---------------------------------------------------------------------------
//	
CVRButtonPanel::CVRButtonPanel( MVRButtonPanelModel* aModel )
    : iModel( aModel ),
    iVRButtons( KVRButtonCount )
	{
	__ASSERT_DEBUG( iModel, User::Panic( KVRPanic, KErrNotFound ) );
	iModel->SetButtonPanelObserver( this );
	}


// ---------------------------------------------------------------------------
// CVRButtonPanel::CVRButtonPanel
// 
// ---------------------------------------------------------------------------
//	
CVRButtonPanel::CVRButtonPanel( MVRButtonPanelModel* aModel,
                               TUid aParentViewUid )
    : iModel( aModel ),  
    iParentViewUid( aParentViewUid ),
    iVRButtons( KVRButtonCount )
	{
	__ASSERT_DEBUG( iModel, User::Panic( KVRPanic, KErrNotFound ) );
	iModel->SetButtonPanelObserver( this );
	}


// ---------------------------------------------------------------------------
// CVRButtonPanel::~CVRButtonPanel
// Destructor
// ---------------------------------------------------------------------------
//	
CVRButtonPanel::~CVRButtonPanel()
	{
	if ( iModel )
		{
		iModel->SetButtonPanelObserver( NULL );
		}

	iVRButtons.ResetAndDestroy();
	delete iTimer;
	iModel = NULL;
	iSkin = NULL;
	}


// ---------------------------------------------------------------------------
// CVRButtonPanel::ConstructL
// 
// ---------------------------------------------------------------------------
//	
void CVRButtonPanel::ConstructL()
	{
	iSkin = AknsUtils::SkinInstance();
	
	// Create buttons from resources
   	CreateButtonsFromResourcesL( R_VR_BUTTON_PANEL_BUTTONS );
   	UpdateButtons();
   	
   	// Has to be done to get the buttons' background shown correctly
   	for ( TInt i = 0; i < iVRButtons.Count(); i++ )
    	{
    	iVRButtons.At( i )->SetBackground( this );
    	}
	}


// ---------------------------------------------------------------------------
// CVRButtonPanel::CreateButtonsFromResourcesL
// Creates buttons from resources
// ---------------------------------------------------------------------------
//    
void CVRButtonPanel::CreateButtonsFromResourcesL( TInt aButtonsResourceId )
	{	
	TResourceReader reader;
	iEikonEnv->CreateResourceReaderLC( reader, aButtonsResourceId );
	
	TInt numButtons( reader.ReadInt16() );

	// Create buttons
	for (TInt i = 0; i < numButtons; i++ )
		{
		CAknButton* aknButton = CAknButton::NewL();
		CleanupStack::PushL( aknButton );
		
		aknButton->SetContainerWindowL( *this );
		aknButton->SetMopParent( this ); 
	    aknButton->ConstructFromResourceL( reader );
		aknButton->SetObserver( this );
	    
	    // Set ready to be drawn and add it to array
	    aknButton->ActivateL();
	   	iVRButtons.AppendL( aknButton );
	   	
	   	CleanupStack::Pop( aknButton ); // aknButton
		}
		
	CleanupStack::PopAndDestroy(); // reader;
	}


// ---------------------------------------------------------------------------
// CVRButtonPanel::Update
// This method can be called via MVRObserver class when Button Panel needs 
// to be updated. 
// ---------------------------------------------------------------------------
//	
void CVRButtonPanel::Update( TVRUpdateCommand aCommand )
	{
	
	TBool drawNow( ETrue );
	
	// Recorder calls this after each tick, we have to check if button reset
	// is needed. Redraw is needed also after 1 secs playback to enable
	// rewind button.
    switch( aCommand )
        {
        case EVRUpdate1Second:
	case EVRUpdate:
        case EVRUpdateStateChange:
            {
            // Redraw
           	drawNow = ETrue;
            break;
            }
        case EVRUpdatePositionChange:
        	{
			// Skip redraw
			drawNow = EFalse;
            break;
            }    
        default:
            {
            // Skip redraw
            drawNow = EFalse;
            return;
            }
        }

	if ( iModel->ResetNeeded() )
		{
		iFocusedButton = iModel->InitialFocusButtonId();
		}
		
	UpdateButtons();
	
	// Change focus if focused button is dimmed
	TuneButtonFocus();
	
	if ( drawNow )	
		{
	    DrawNow();				
		}
	}


// ---------------------------------------------------------------------------
// CVRButtonPanel::SizeChanged
// Called when Button Panel layout needs to be updated 
// ---------------------------------------------------------------------------
//	
void CVRButtonPanel::SizeChanged()
	{
    TInt count( iModel->ButtonCount() );

    TSize size; // Size of the bitmap

    for ( TInt i( 0 ); i < count; i++ )
		{
		// Layout button.
        TAknWindowLineLayout cell = AppLayout::cell_vorec_pane( i );
        
    	TAknLayoutRect buttonLayout;
		buttonLayout.LayoutRect( Rect(), cell );
		TRect buttonRect = buttonLayout.Rect();
		TInt newSize = (TInt)(buttonRect.Width() * 0.1); // used to reduce the size of button
		buttonRect.Shrink( TSize(newSize,newSize) );
    	iVRButtons.At( i )->SetRect( buttonRect );
   		} 
	}
	
	
// ---------------------------------------------------------------------------
// CVRButtonPanel::OfferKeyEventL
// Key event handling
// ---------------------------------------------------------------------------
//	
TKeyResponse CVRButtonPanel::OfferKeyEventL( const TKeyEvent& aKeyEvent,
											 TEventCode aType )
	{
	if ( aType != EEventKey )
		{
		if( aType == EEventKeyDown )
			{
			// Rocker key pressed
			if ( aKeyEvent.iScanCode == EStdKeyDevice3 )
				{
				// Start timer to get continuous timercallbacks for 
				// FastForwarding and FastRewinding
				StartTimerL();
				}
			}	
		else if( aType == EEventKeyUp )
			{
			// Rocker key pressed
			if ( aKeyEvent.iScanCode == EStdKeyDevice3 )
				{
				StopTimer();
				}
			}			
		return EKeyWasNotConsumed;
		}
	// EEventKey
	switch ( aKeyEvent.iCode )
		{
		case EKeyDownArrow:
			{
			MoveFocus( EFocusDown, ETrue );
			break;
			}
		case EKeyUpArrow:
			{
			MoveFocus( EFocusUp, ETrue );
			break;
			}
		case EKeyEnter:	
		case EKeyOK:
			{	
			// This is in use as long as there are only 2 CBA buttons. When MSK
			// enabled, execution goes straight to CVRRecView::HandleCommandL
			FetchAndSendCommandL();
			break;
			}
		default:
			{
			return EKeyWasNotConsumed;
			}
		}

	return EKeyWasConsumed;
	}


// ---------------------------------------------------------------------------
// CVRButtonPanel::Draw
// Needed to get correct focus drawing with AknButtons
// ---------------------------------------------------------------------------
//		
void CVRButtonPanel::Draw( CWindowGc& /*aGc*/, const CCoeControl& /*aControl*/, 
		const TRect& /*aRect*/ ) const
	{
	}


// ---------------------------------------------------------------------------
// CVRButtonPanel::Draw
// 
// ---------------------------------------------------------------------------
//		
void CVRButtonPanel::Draw( const TRect& /*aRect*/ ) const
	{
	CWindowGc& gc = SystemGc();
	// Acquire the control context through the MOP-chain
	MAknsControlContext* context = AknsDrawUtils::ControlContext( this );

	// Draw the background using the control context
	// Note: if there is no skin, the background is just cleared
	// (which is OK for us)
	AknsDrawUtils::Background( iSkin, context, this, gc, Rect() );

	// Draw button highlight
	DrawFocus( gc );
	}


// ---------------------------------------------------------------------------
// CVRButtonPanel::CountComponentControls
// 
// ---------------------------------------------------------------------------
//	
TInt CVRButtonPanel::CountComponentControls() const
	{
	return iVRButtons.Count();
	}


// ---------------------------------------------------------------------------
// CVRButtonPanel::ComponentControl
// 
// ---------------------------------------------------------------------------
//	
CCoeControl* CVRButtonPanel::ComponentControl( TInt aIndex ) const
	{
	return iVRButtons.At( aIndex );
	}


// ---------------------------------------------------------------------------
// CVRButtonPanel::MoveFocus
// Moves the focus to next button up or down. aDirection tells the which.
// ---------------------------------------------------------------------------
//	
void CVRButtonPanel::MoveFocus( const TFocusChange aDirection, TBool aDrawNow )
	{
	TInt buttonCount( iModel->ButtonCount() );
	TInt newButton( iFocusedButton );

	for ( TInt i( 0 ); i < buttonCount; i++ )
		{
		// calculate new button id. the modulo operator (%) handles looping
		newButton = ( newButton + aDirection + buttonCount ) % buttonCount;
		// dimmed buttons are skipped
		if ( iModel->ButtonState( newButton ) != EDimmed )
			{
			iFocusedButton = newButton;
			
			// Timer is used for FastForward/-Rewind. Stop that when focus is 
			// changed.
			StopTimer();
			break;
			}
		}
	
	if ( aDrawNow )
		{
		DrawNow();	
		}
	}


// ---------------------------------------------------------------------------
// CVRButtonPanel::UpdateLayoutL
// 
// ---------------------------------------------------------------------------
//	
void CVRButtonPanel::UpdateLayoutL( )
	{
	SizeChanged();
	}


// ---------------------------------------------------------------------------
// CVRButtonPanel::HandleResourceChangeL
// 
// ---------------------------------------------------------------------------
//	
void CVRButtonPanel::HandleResourceChangeL( TInt aType )
	{
	CCoeControl::HandleResourceChange( aType );
	
	if ( aType == KEikDynamicLayoutVariantSwitch )
		{
		UpdateLayoutL();
		}
	else if ( aType == KAknsMessageSkinChange )
		{
		// New skin instance is loaded
		iSkin = AknsUtils::SkinInstance();
		}
	
	// Inform Avkon Buttons		
	for( TInt buttonIndex = 0; buttonIndex < iVRButtons.Count() ; 
			buttonIndex++ )
		{
		iVRButtons.At( buttonIndex )->HandleResourceChange( aType );
		}
	}

	
// ---------------------------------------------------------------------------
// CVRButtonPanel::UpdateButtons
// Updates all Button Panel buttons to correct visual state. Also few  
// exceptional cases are handled here (1. clip position is at the beginning 
// 2. clip position is at the end)
// ---------------------------------------------------------------------------
//		
void CVRButtonPanel::UpdateButtons()
	{
	TUint stateId;
	// Update all the buttons	
	for( TInt buttonIndex = 0; buttonIndex < iVRButtons.Count() ; 
			buttonIndex++ )
		{
		stateId = iModel->VisualStateId();
		
		// dim rewind button if there is nothing to rewind
		if ( buttonIndex == EButtonRewind && 
				iModel->ButtonState( buttonIndex ) == EDimmed )
			{
			iVRButtons.At( buttonIndex )->SetDimmed(ETrue);			
			stateId = EStateDisableButtons;
			}
		if ( buttonIndex == EButtonRewind && 
				iModel->ButtonState( buttonIndex ) != EDimmed )
			{
			iVRButtons.At( buttonIndex )->SetDimmed(EFalse);			
			}
		// dim forward button if there is nothing to forward
		if( buttonIndex == EButtonForward && 
				iModel->ButtonState( buttonIndex ) == EDimmed )
			{
			iVRButtons.At( buttonIndex )->SetDimmed(ETrue);		
			stateId = EStateDisableButtons;	
			}
		if( buttonIndex == EButtonForward && 
				iModel->ButtonState( buttonIndex ) != EDimmed )
			{
			iVRButtons.At( buttonIndex )->SetDimmed(EFalse);		
			}
		
		// ************ADDED CODE STARTS****************************
		if( buttonIndex == EButtonRecord && 
				iModel->ButtonState( buttonIndex ) == EDimmed )
			{								
			stateId = EStateDisableButtons;	
			}
		// ************ADDED CODE ENDS******************************

		if( iModel->ButtonState( buttonIndex ) == EDimmed )
			{
			iVRButtons.At( buttonIndex )->SetDimmed(ETrue);		
			}
		else
			{
			iVRButtons.At( buttonIndex )->SetDimmed(EFalse);	
			}	
		
		// Change the button state, don't redraw
		iVRButtons.At( buttonIndex )->SetCurrentState( stateId , EFalse );
		}
	}	
	


// ---------------------------------------------------------------------------
// CVRButtonPanel::DrawFocus() 
// Draws the focus rectangle around the currently focused button
// ---------------------------------------------------------------------------
//    
void CVRButtonPanel::DrawFocus( CWindowGc& aGc ) const
	{
	
	aGc.SetBrushStyle( CGraphicsContext::ENullBrush );

	if ( AknLayoutUtils::ScalableLayoutInterfaceAvailable() )
		{
		// From LAF-table
//		TAknWindowLineLayout area = 
//			AknLayoutScalable_Apps::grid_highlight_pane_cp05().LayoutLine();
		
//		TRect buttonRect = iVRButtons.At( iFocusedButton )->Rect();
		TAknWindowLineLayout area = AppLayout::cell_vorec_pane( iFocusedButton );

		TAknLayoutRect layoutRect;
		layoutRect.LayoutRect( Rect(), area );
//		layoutRect.LayoutRect( buttonRect, area );
		
		// The outer rectangle is calculated from the layoutRect
		TRect outerRect = TRect( layoutRect.Rect().iTl, layoutRect.Rect().iBr );
		
		// Inner rectangle is generated by shrinking the outer rect.
		TRect innerRect = outerRect;
		innerRect.Shrink( TSize( KVRAmountOfFocusShrink, 
									KVRAmountOfFocusShrink ) );
		
		if (iModel->ButtonState( iFocusedButton ) != EDimmed )
			{
			//Focus frame is drawn
			aGc.SetPenStyle( CGraphicsContext::ENullPen );
			TBool highlightDrawn = AknsDrawUtils::DrawFrame( iSkin, aGc, 
					outerRect, innerRect, KAknsIIDQsnFrGrid, KAknsIIDNone );
			} 
		}
	else
		{
		if ( iModel->ButtonState( iFocusedButton ) != EDimmed )
			{
			// Old basic rectangle is drawn
			aGc.SetPenStyle( CGraphicsContext::ESolidPen );
			aGc.DrawRect( iVRButtons.At( iFocusedButton )->Rect() );
			}
		}
	
	}


// ---------------------------------------------------------------------------
// CVRButtonPanel::FocusedButton() 
// Returns the index for the currently focused button
// ---------------------------------------------------------------------------
//
TInt CVRButtonPanel::FocusedButton() const
	{
	return iFocusedButton;
	}
	
	
// ---------------------------------------------------------------------------
// CVRButtonPanel::StartTimerL
// Create and start a timer with initial delay of KVRFastForwStartDelay
// and with callback frequency of KVRFastForwUpdateDelay
// ---------------------------------------------------------------------------
//	
void CVRButtonPanel::StartTimerL()
	{
	if ( iTimer )
		{
		// stop if iTimer is already running
		StopTimer();	
		}

	TCallBack cb( TimerCallBack, this );
	iTimer = CPeriodic::NewL( 0 );
	iTimer->Start( KVRFastForwStartDelay, KVRFastForwUpdateDelay, cb );
	}


// ---------------------------------------------------------------------------
// CVRButtonPanel::StopTimer
// Stop the timer by deleting the CPeriodic object
// ---------------------------------------------------------------------------
//	
void CVRButtonPanel::StopTimer()
	{
	delete iTimer;
	iTimer = NULL;
	}


// ---------------------------------------------------------------------------
// CVRButtonPanel::TimerCallBack
// Timer callback for iTimer. Time between callbacks is KVRFastForwUpdateDelay
// ---------------------------------------------------------------------------
//	
TInt CVRButtonPanel::TimerCallBack( TAny* aButtonPanel )
	{
	CVRButtonPanel* buttonPanel = reinterpret_cast< CVRButtonPanel* >
															( aButtonPanel );
	
	buttonPanel->HandleTimerCallBack();
		
	return ETrue;
	}


// ---------------------------------------------------------------------------
// CVRButtonPanel::HandleTimerCallBack
// Non-static variant of TimerCallBack (more convinient to implement)
// ---------------------------------------------------------------------------
//
void CVRButtonPanel::HandleTimerCallBack()
	{
	// Callbacks are handled only if focus is on rewind or forward button
	if (iFocusedButton == EButtonRewind || iFocusedButton == EButtonForward)
		{
		TRAPD( err, FetchAndSendCommandL() );
		
		if ( err )
			{
		RDebug::Print(_L("VoiceRecorder: FetchAndSendCommandL, error ID: %d"), 
				err);
			StopTimer();
			}
		}
	}
	
	
// ---------------------------------------------------------------------------
// CVRButtonPanel::FetchAndSendCommand
// Fetches the command relating to focused button from the model. Command is 
// sent on to active view.
// ---------------------------------------------------------------------------
//	
void CVRButtonPanel::FetchAndSendCommandL()
	{
	// Don't forward the command if dimmed button was clicked or 
	// if model can't handle commands
	if ( iModel->ButtonState( iFocusedButton ) == EDimmed || 
		!iModel->CanHandleCommands() )
		{
		return;
		}
	
	// Send a command id to the active view specified in the button
	CAknViewAppUi* appUi = reinterpret_cast< CAknViewAppUi* >(
									CEikonEnv::Static()->EikAppUi() );
	CAknView* view = appUi->View( iParentViewUid );
	view->HandleCommandL( iModel->CommandId( iFocusedButton ) );
	
	Update( EVRUpdateStateChange );

	}
	
	
// ---------------------------------------------------------------------------
// CVRButtonPanel::TuneButtonFocus
// The special cases when button focus has to be moved are handled here
// ---------------------------------------------------------------------------
//	
void CVRButtonPanel::TuneButtonFocus()
	{
	
	// CVRRecViewModel::ButtonState() returns EDimmed for EButtonRewind if
	// the position of the clip is at the beginning
	if ( iFocusedButton == EButtonRewind && 
			iModel->ButtonState( iFocusedButton ) == EDimmed )
		{
		iVRButtons.At( iFocusedButton )->SetDimmed(ETrue);	
		MoveFocus( EFocusDown, EFalse );	
		}
		
	// CVRRecViewModel::ButtonState() returns EDimmed for EButtonForward if
	// the position of the clip is at the end	
	else if( iFocusedButton == EButtonForward && 
				iModel->ButtonState( iFocusedButton ) == EDimmed )	
		{
		iVRButtons.At( iFocusedButton )->SetDimmed(ETrue);	
		MoveFocus( EFocusUp, EFalse );
		}	
	}


// ---------------------------------------------------------------------------
// CVRButtonPanel::HandleControlEventL
// A callback function of Avkon buttons. Handles EEventStateChanged type events 
// which happen every time when some of the buttons is pressed and button state 
// changes. Event comes before CAknButton::Draw method is executed. 
// ---------------------------------------------------------------------------
//			
void CVRButtonPanel::HandleControlEventL( CCoeControl *aControl, 
										  TCoeEvent aEventType )
	{
	if( AknLayoutUtils::PenEnabled() )
		{	
		
		switch( aEventType )
			{
			case EEventStateChanged:
				{
				
				TInt newFocusedButton( 0 );
				for( TInt i = 0; i < iVRButtons.Count() ; i++ )
					{
					// Which of buttons was pressed 
					if ( aControl == iVRButtons.At( i ) )
						{
						// When new state is the first state, we have to set 
						// new state to ENumStates to be able to se a correct 
						// state in SetCurrentState( stateId - 1 , EFalse );
						TInt stateId( iVRButtons.At( i )->StateIndex() );
						if( stateId == 0 )
							{
							stateId = ENumStates;
							}
						
						// CAknButton automatically changes the state to next 
						// one always when it's ,pressed whether we want it or 
						// not. To prevent this we set the previous state 
						// before button draws itself
						iVRButtons.At( i )->SetCurrentState( stateId - 1,
															 EFalse );
						newFocusedButton = i;
						break;
						}
					}
				
				// Focus already was on pressed button so button's command is 
				// executed	
				if ( iFocusedButton == newFocusedButton )
					{
					// Don't forward the command if dimmed button was clicked
					if ( iModel->ButtonState( iFocusedButton ) == EDimmed )
						{
						break;
						}
					
					// Send a command id specified in the button to an 
					// active view 
					CAknViewAppUi* appUi = reinterpret_cast< CAknViewAppUi* >(
											CEikonEnv::Static()->EikAppUi() );
					CAknView* view = appUi->View( iParentViewUid );
					view->HandleCommandL( iModel->CommandId( iFocusedButton ) );
					
					// To make FastForwarding/FastRewinding possible
					if ( iFocusedButton == EButtonRewind || 
						 iFocusedButton == EButtonForward )
						{
						if ( iLastPointerEvent == EVRButtonDownEvent )
							{
							StartTimerL();	
							}							
						}
					
					Update( EVRUpdateStateChange );
					}
				// Focus was not on the focused button -> 
				// Focus should be changed	
				else if ( newFocusedButton >= 0 && 
						  newFocusedButton < iVRButtons.Count() )
					{
					//draw focus
					if ( iModel->ButtonState( newFocusedButton ) != EDimmed )
						{
						iFocusedButton = newFocusedButton;
						DrawNow();

						// Send a command id specified in the button to an 
						// active view 
						CAknViewAppUi* appUi = reinterpret_cast< CAknViewAppUi* >(
											CEikonEnv::Static()->EikAppUi() );
						CAknView* view = appUi->View( iParentViewUid );
						view->HandleCommandL( iModel->CommandId( iFocusedButton ) );
						// To make FastForwarding/FastRewinding possible
						if ( iFocusedButton == EButtonRewind || 
							 iFocusedButton == EButtonForward )
							{
							if ( iLastPointerEvent == EVRButtonDownEvent )
								{
								StartTimerL();	
								}							
							}
						}
                // for one error about touch UI, when click a dimmed button, the dimmed
                // button should not go back to umdimmed.                   
				else
					{
					DrawNow();	
					}
				}													
				break;	
				}
			default:
				{
				// No default behaviour
				break;	
				}
			}
			
		}
	}


// ---------------------------------------------------------------------------
// CVRButtonPanel::HandlePointerEventL
// Handles Button panel pointer events. True functionality is to stop 
// FastForward timer when FF should be stopped.
// ---------------------------------------------------------------------------
//		
void CVRButtonPanel::HandlePointerEventL( const TPointerEvent& aPointerEvent )
	{
	if( AknLayoutUtils::PenEnabled() )
		{
		
		
		switch( aPointerEvent.iType )
			{
			case TPointerEvent::EButton1Down:
				{
				// Don't start timer here. It's started in HandleControlEventL() 
				iLastPointerEvent = EVRButtonDownEvent;
				break;
				}
			case TPointerEvent::EDrag:
				{
				// Don't stop timer if just dragging inside the currently 
				// focused button
				TRect focusedButtonRect( 
									 iVRButtons.At( iFocusedButton )->Rect() );
				if( !focusedButtonRect.Contains( aPointerEvent.iPosition ) )
					{
					// Dragged outside
					iLastPointerEvent = EVRButtonDragOutsideEvent;
					StopTimer();	
					}
				break;		
				}
			case TPointerEvent::EButton1Up:
				{
				// Pen up
				iLastPointerEvent = EVRButtonUpEvent;
				StopTimer();	
				break;
				}		
			default:
				{
				break;	
				}	
			}
		
		CCoeControl::HandlePointerEventL( aPointerEvent );
		}
	}

// End of file	
