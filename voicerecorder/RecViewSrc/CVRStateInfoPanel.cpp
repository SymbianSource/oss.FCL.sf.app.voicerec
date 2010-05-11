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
*     a set of labels, and a progress bar.
*
*/


// INCLUDE FILES

#include <AknUtils.h>
#include <AknsUtils.h>
#include <eikprogi.h>
#include <aknenv.h>
#include <applayout.cdl.h>
#include <aknview.h>
#include <aknViewAppUi.h>
#include <AknDef.h>
#include <AknsConstants.h>
#include <aknlayoutscalable_apps.cdl.h>

#include "CVRLabel.h"
#include "CVRStateInfoPanel.h"
#include "MVRStateInfoModel.h"
#include "voicerecorder.hrh"
#include "VRConsts.h"

// CONSTANTS
const TInt KVRLabelMap[] =
	{
	CVRStateInfoPanel::ELabelName,
	CVRStateInfoPanel::ELabelDate,
	CVRStateInfoPanel::ELabelQuality,
	CVRStateInfoPanel::ELabelStatus,
	CVRStateInfoPanel::ELabelMax,
	CVRStateInfoPanel::ELabelMin,
	CVRStateInfoPanel::ELabelQualityText
	};
	
_LIT( KVREmptyLabel, " " );


// ================= MEMBER FUNCTIONS ========================================

// ----------------------------------------------------------------------------
// CVRStateInfoPanel::CVRStateInfoPanel
// 
// ----------------------------------------------------------------------------
//		
CVRStateInfoPanel::CVRStateInfoPanel( MVRStateInfoModel* aModel )
	: iModel( aModel ), iLabels( ENumLabels ), iPBposition( 0 )
	{
	if ( iModel )
		{
		iModel->SetStateInfoObserver( this );
		}
	}


// ----------------------------------------------------------------------------
// CVRStateInfoPanel::CVRStateInfoPanel
// 
// ----------------------------------------------------------------------------
//		
CVRStateInfoPanel::CVRStateInfoPanel( MVRStateInfoModel* aModel,
                               				TUid aParentViewUid )
	: iModel( aModel ), 
	iLabels( ENumLabels ),
	iParentViewUid( aParentViewUid ), 
	iPBposition( 0 )
	{
	if ( iModel )
		{
		iModel->SetStateInfoObserver( this );
		iModel->SetStateInfoPanelObserver( this );
		}
	}


// ----------------------------------------------------------------------------
// CVRStateInfoPanel::~CVRStateInfoPanel
// 
// ----------------------------------------------------------------------------
//			
CVRStateInfoPanel::~CVRStateInfoPanel()
	{
	if ( iModel )
		{
		iModel->SetStateInfoObserver( NULL );
		}
	iLabels.ResetAndDestroy();
	delete iProgressBar;
	iSkin = NULL;
	}


// ----------------------------------------------------------------------------
// CVRStateInfoPanel::ConstructL
// 
// ----------------------------------------------------------------------------
//		
void CVRStateInfoPanel::ConstructL()
	{
	// New skin instance is loaded
	iSkin = AknsUtils::SkinInstance();
	
	// Create labels
	for ( TInt i = 0; i < ENumLabels; i++ )
		{
		CVRLabel* label = CVRLabel::NewLC( *this );

		// Set the MOP-parent so that the labels can get the 
		// skin context from rec view container
		label->SetMopParent( this );
		iLabels.AppendL( label );
		CleanupStack::Pop(); // label;

		// Set initial text for label
		TBuf< VRLABELMAXLENGTH > str;
		iModel->GetLabel( str, i );
		label->SetText( str );
		}

	CreateProgressBarL();
	UpdateLayoutL();
	}


// ----------------------------------------------------------------------------
// CVRStateInfoPanel::CreateProgressBarL
// 
// ----------------------------------------------------------------------------
//		
void CVRStateInfoPanel::CreateProgressBarL()
	{
	delete iProgressBar;
	iProgressBar = NULL;

	// Create progress bar
	CEikProgressInfo::SInfo info = ProgressInfo();

	iProgressBar = new( ELeave ) CEikProgressInfo( info );
	iProgressBar->SetContainerWindowL( *this );
	
	// Set the MOP-parent so that the progress bar can get the 
	// skin context from rec view container
	iProgressBar->SetMopParent( this );
	iProgressBar->ConstructL();

	iProgressBar->ActivateL();
	AknLayoutUtils::LayoutControl( iProgressBar, Rect(),
		AppLayout::Voice_Recorder_elements_Line_4() );
	}


// ----------------------------------------------------------------------------
// CVRStateInfoPanel::SizeChanged
// 
// ----------------------------------------------------------------------------
//		
void CVRStateInfoPanel::SizeChanged()
    {
    TAknTextComponentLayout layoutText;
    
    for ( TInt i = 0; i < ENumLabels; i++ )
        {
        switch ( KVRLabelMap[i] )
            {
            case 0: 
                layoutText = AknLayoutScalable_Apps::vorec_t1();
                break;
            case 1:
                layoutText = AknLayoutScalable_Apps::vorec_t2();
                break;
            case 2: 
                layoutText = AknLayoutScalable_Apps::vorec_t3();
                break;
            case 3:
                layoutText = AknLayoutScalable_Apps::vorec_t4();
                break;
            case 4: 
                layoutText = AknLayoutScalable_Apps::vorec_t5();
                break;
            case 5:
                layoutText = AknLayoutScalable_Apps::vorec_t6();
                break;
            case 6:
                layoutText = AknLayoutScalable_Apps::vorec_t7();
                break;          
            }
        
		AknLayoutUtils::LayoutLabel( iLabels.At( i ), Rect(), layoutText.LayoutLine() );
		
		if ( i != ELabelStatus )
			{
			// Get the correct text color from skin and override it
			// The status label is always red, i guess
			CCoeControl* label( iLabels.At( i ) );
			TRgb color( KRgbBlack );
			AknsUtils::GetCachedColor( iSkin, color, KAknsIIDQsnTextColors,
				EAknsCIQsnTextColorsCG6 );		
			// We can ignore the error, it only means we will have wrong color
			TRAP_IGNORE( AknLayoutUtils::OverrideControlColorL( *label,
				EColorLabelText, color ) );
			}
		}

	// There's nothing rational to do here if it leaves
	TRAP_IGNORE( CreateProgressBarL() );	
	}


// ----------------------------------------------------------------------------
// CVRStateInfoPanel::Draw
// 
// ----------------------------------------------------------------------------
//		
void CVRStateInfoPanel::Draw( const TRect& /*aRect*/ ) const
	{
	}


// ----------------------------------------------------------------------------
// CVRStateInfoPanel::CountComponentControls
// 
// ----------------------------------------------------------------------------
//		
TInt CVRStateInfoPanel::CountComponentControls() const
	{
	TInt componentCount( 0 );
	if ( iProgressBar )
		{
		componentCount = ENumLabels + 1;  // +1 means iProgressBar
		}
	return componentCount;
	}


// ----------------------------------------------------------------------------
// CVRStateInfoPanel::ComponentControl
// 
// ----------------------------------------------------------------------------
//		
CCoeControl* CVRStateInfoPanel::ComponentControl( TInt aIndex ) const
	{
	if ( aIndex < ENumLabels )
		{
		return iLabels.At( aIndex );
		}
	else
		{
		return iProgressBar;
		}
	}


// ----------------------------------------------------------------------------
// CVRStateInfoPanel::Update
// 
// ----------------------------------------------------------------------------
//		
void CVRStateInfoPanel::Update( TVRUpdateCommand /*aCommand*/ )
	{
	for ( TInt i = 0; i < ENumLabels; i++ )
		{
		TBuf< VRLABELMAXLENGTH > str;
		iModel->GetLabel( str, i );
		if ( str.Length() < 1 )
			{
			// "" doesn't empty the label, so use space
			str = KVREmptyLabel;
			}
		iLabels.At( i )->SetTextAndDraw( str );
		}
	
	UpdateProgressBar();
	
	}


// ----------------------------------------------------------------------------
// CVRStateInfoPanel::UpdateLayoutL
// 
// ----------------------------------------------------------------------------
//		
void CVRStateInfoPanel::UpdateLayoutL()
	{
	SizeChanged();
	}



// ----------------------------------------------------------------------------
// CVRStateInfoPanel::ProgressInfo
// 
// ----------------------------------------------------------------------------
//		
CEikProgressInfo::SInfo CVRStateInfoPanel::ProgressInfo()
	{
	CEikProgressInfo::SInfo info;

	TAknWindowLineLayout infobarLayout =
		AppLayout::Voice_Recorder_elements_Line_4();
    TAknLayoutRect infobarRect;
    infobarRect.LayoutRect( Rect(), infobarLayout );	
  
    info.iWidth = infobarRect.Rect().Width();
	info.iHeight = infobarRect.Rect().Height();

	return info;
	}


// ----------------------------------------------------------------------------
// CVRStateInfoPanel::HandleResourceChangeL
// 
// ----------------------------------------------------------------------------
//		
void CVRStateInfoPanel::HandleResourceChangeL( TInt aType )
	{
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
		UpdateLayoutL();
        }
	else if ( aType == KAknsMessageSkinChange )
		{
		// New skin instance is loaded
		iSkin = AknsUtils::SkinInstance();
		}
	
	for ( TInt i = 0; i < ENumLabels; i++ )
		{
		if ( i < iLabels.Count() )
			{
			iLabels.At( i )->HandleResourceChange( aType );	
			}
		}
		
	iProgressBar->HandleResourceChange( aType );
	UpdateProgressBar();
	
	CCoeControl::HandleResourceChange( aType );
	}


// ----------------------------------------------------------------------------
// CVRStateInfoPanel::UpdateProgressBar
// 
// ----------------------------------------------------------------------------
//		
void CVRStateInfoPanel::UpdateProgressBar()
	{
	
	if ( iProgressBar )
		{
		if ( iModel->HasProgressBar() )
			{
			iProgressBar->SetFinalValue( I64INT( iModel->Duration().Int64() /
										 KVRSecondAsMicroSeconds ) );
			iProgressBar->SetAndDraw( I64INT( iModel->Position().Int64() /
									  KVRSecondAsMicroSeconds ) );
			iProgressBar->MakeVisible( ETrue );
			
			iPBposition = I64INT( iModel->Position().Int64() /
									  KVRSecondAsMicroSeconds );
			}
		else
			{
			iProgressBar->MakeVisible( EFalse );
			}
		}
	}


// ----------------------------------------------------------------------------
// CVRStateInfoPanel::HandlePointerEventL
// From CCoeControl
// ----------------------------------------------------------------------------
//		
void CVRStateInfoPanel::HandlePointerEventL( const TPointerEvent& 
													aPointerEvent )
	{
	if( AknLayoutUtils::PenEnabled() && iModel->HasProgressBar() && 
		iProgressBar )
		{
		CCoeControl::HandlePointerEventL( aPointerEvent );
		
		switch( aPointerEvent.iType )
			{
			case TPointerEvent::EButton1Down:
				{
				TRect progressBarRect( iProgressBar->Rect() );
				// Check if pressed position is in progress bar's rect
				if( progressBarRect.Contains( aPointerEvent.iPosition ) )
					{
					HandleProgressBarTouchL( progressBarRect, 
											 aPointerEvent.iPosition.iX );
					}
				break;
				}
			case TPointerEvent::EDrag:
				{
				TRect progressBarRect( iProgressBar->Rect() );
				if( progressBarRect.Contains( aPointerEvent.iPosition ) )
					{
					HandleProgressBarTouchL( progressBarRect, 
											 aPointerEvent.iPosition.iX );
					}
				break;		
				}
			case TPointerEvent::EButton1Up:
				{		
				break;
				}		
			default:
				{
				break;	
				}	
			}
		}
	}


// ----------------------------------------------------------------------------
// CVRStateInfoPanel::HandleProgressBarTouchL
// 
// ----------------------------------------------------------------------------
//	
void CVRStateInfoPanel::HandleProgressBarTouchL( TRect aPBRect, 
												 TInt aPressedPoint )
	{
	if ( AknLayoutUtils::PenEnabled() )
		{
#ifdef _DEBUG
		RDebug::Print( _L( "VoiceRecorder: HandleProgressBarTouchL enter" ) );
#endif
		// Progress Bar start and end points
		TInt pbEndPoint( aPBRect.iBr.iX );
		TInt pbStartPoint( aPBRect.iTl.iX );
		
		TInt totalPBLength( pbEndPoint - pbStartPoint );
		// calculate what is the time position that was pressed in 
		// the progress bar
		TTimeIntervalMicroSeconds newPosition( 
										( ( aPressedPoint - pbStartPoint ) * 
							  			iModel->Duration().Int64() ) / 
							  			totalPBLength );
							  			
		// Round the position to the nearest second value
		TInt roundedNewPosition( RoundMicroSecsToSecs( newPosition ) );

		// Performace issue. Send command only if value has changed
		if ( roundedNewPosition != iPBposition )
			{
			iPBposition = roundedNewPosition;

			CAknViewAppUi* appUi = reinterpret_cast< CAknViewAppUi* >(
											CEikonEnv::Static()->EikAppUi() );
			CAknView* view = appUi->View( iParentViewUid );
			
			// inform the view that position has changed.
			view->HandleCommandL( ECmdSetNewPosition );	
			}	
#ifdef _DEBUG
		RDebug::Print( _L( "VoiceRecorder: HandleProgressBarTouchL exit" ) );
#endif
			
		} // PenEnabled
		
	}
	

// ----------------------------------------------------------------------------
// CVRStateInfoPanel::RoundMicroSecsToSecs
// Rounds aMicroSecs value to nearest second value (Round)
// ----------------------------------------------------------------------------
//			
TInt CVRStateInfoPanel::RoundMicroSecsToSecs( TTimeIntervalMicroSeconds 
											  aMicroSecs )
	{	
	// Rounds to secondss resolution (Floor)
	Int64 secondsInMicroSecs( ( ( aMicroSecs.Int64() / 
								  KVRSecondAsMicroSeconds ) * 
								  KVRSecondAsMicroSeconds ) );
								  
	// takes the leftover part in seconds resolution
	Int64 leftoversInMicroSecs ( aMicroSecs.Int64() - secondsInMicroSecs );
	
	// Nearer to upper sec value
	if ( leftoversInMicroSecs / ( KVRSecondAsMilliSeconds ) > 
		 KVRHalfSecondAsMilliSeconds )
		{
		// + 1 sec
		return ( ( secondsInMicroSecs + KVRSecondAsMicroSeconds ) / 
													 KVRSecondAsMicroSeconds );
		}
	// Nearer to lower sec value	
	else
		{
		return secondsInMicroSecs / KVRSecondAsMicroSeconds;
		}	
	}
	

// ----------------------------------------------------------------------------
// CVRStateInfoPanel::ProgressBarPosition
// Just returns the current progress bar position
// ----------------------------------------------------------------------------
//		
TInt CVRStateInfoPanel::ProgressBarPosition() const
	{
	return iPBposition;
	}
	
// End of file
