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


#ifndef __CVRSTATEINFOPANEL_H__
#define __CVRSTATEINFOPANEL_H__

// INCLUDES
#include <coecntrl.h>
#include <eikprogi.h>
#include <AknUtils.h>

#include "MVRObserver.h"
#include "MVRStateInfoObserver.h"

// FORWARD DECLARATIONS
class CEikProgressInfo;
class CVRLabel;
class MVRStateInfoModel;


// CLASS DEFINITION
/**
* A compound control class. It is responsible for drawing and updating
* a set of labels, and a progress bar.
*/
NONSHARABLE_CLASS( CVRStateInfoPanel )
	: public CCoeControl, public MVRObserver, public MVRStateInfoObserver
	{
	public: // data types
	    enum TVRLabels
	        {
	        ELabelName = 0,
	        ELabelDate,
	        ELabelQuality,
	        ELabelMin,
	        ELabelMax,
	        ELabelStatus,
	        ELabelQualityText
	        };
	
	public: // constructors and destructor

		/**
		* Constructor.
		* @param aModel Pointer to the state info model.
		* Ownership is not transferred.
		*/
		CVRStateInfoPanel( MVRStateInfoModel* aModel );
		
		/**
		* Constructor.
		* @param aModel Pointer to the state info model.
		* @param aParentViewUid Uid of CVRRecView for fast swap
		* Ownership is not transferred.
		*/
		CVRStateInfoPanel( MVRStateInfoModel* aModel,
						   TUid aParentViewUid );
		/**
		* Destructor
		*/
		virtual ~CVRStateInfoPanel();

		/**
		* 2nd phase constructor.
		*/
		void ConstructL();

	private: // constructors

		/**
		* Default constructor. Not implemented.
		*/
		CVRStateInfoPanel();

	public:
		/**
		* Handles a change to the application's resources
		* @param aType The type of changed resource
		*/		
		void HandleResourceChangeL( TInt aType );

	public: // from CCoeControl

		/*
		* From CCoeControl
		* @see CCoeControl
		*/
		void SizeChanged();

		/**
		* Called by the framework to get the number of sub-components
		* contained in this component. 
		* @return The number of component controls contained by this control
		*/
		TInt CountComponentControls() const;

		/**
		* Called by the framework to get a specified sub-component
		* of this component.
		* @param aIndex The index of the component to get.
		* @return The component control with an index of aIndex.
		*/
		CCoeControl* ComponentControl( TInt aIndex ) const;
		
		/**
		* Gets called whenever a pointer event occurs.
		* @param aPointerEvent The pointer event to handle.
		*/					
		void HandlePointerEventL( const TPointerEvent& aPointerEvent );
	
	private: // From CCoeControl

		void Draw( const TRect& aRect ) const;
		
	public: // from MVRObserver

		/**
		* Called to notify a change in the observed subject's state.
		*/
		void Update( TVRUpdateCommand aCommand );
		
	public: // from MVRStateInfoObserver	
		
		/**
		* Returns the current progress bar position value
		* @return Position value
		*/
		TInt ProgressBarPosition() const;
		
	private: // Helpers

		CEikProgressInfo::SInfo ProgressInfo();

		void UpdateLayoutL();

		void CreateProgressBarL();
		
		/**
        * Updates the progress bar 
        */
		void UpdateProgressBar();
		
		/**
		* Handles progress bar touch events
		* @param aPBRect Current progress bar rectangle 
		* @param aPressedPoint The x coordinate value that was pressed
		* 			inside the progress bar
		*/
		void HandleProgressBarTouchL( TRect aPBRect, TInt aPressedPoint );
		
		/**
		* Rounds the parameter value to nearest second value
		* @param aMicroSecs Time value in microseconds
		* @return Rounded value in seconds
		*/
		TInt RoundMicroSecsToSecs( TTimeIntervalMicroSeconds aMicroSecs );


	private: // Data

		/**
		* Pointer to the button panel model. Not owned.
		*/
		MVRStateInfoModel* iModel;

		/**
		* Pointer to an array of labels. Owned.
		*/
		CArrayPtrSeg< CVRLabel > iLabels;

		/**
		* Pointer to a progress bar control. Owned.
		*/
		CEikProgressInfo* iProgressBar;
		
		/**
		* Uid of the parent view
		*/
		TUid iParentViewUid;
		
		/**
		* Current progress bar position
		*/
		TInt iPBposition;
		
		// Skin instance. Not owned.
		MAknsSkinInstance* iSkin;

	};

#endif // __CVRSTATEINFOPANEL_H__
