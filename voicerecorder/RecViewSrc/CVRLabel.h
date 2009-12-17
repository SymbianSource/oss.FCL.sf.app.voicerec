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


#ifndef __CVRLABEL_H__
#define __CVRLABEL_H__

// INCLUDES
#include <eiklabel.h>
#include <AknUtils.h>

// FORWARD DECLARATIONS
class MAknsSkinInstance;

// CLASS DEFINITION
/**
* This class is an extension to the standard Eikon label.
* It provides leave-safe SetText() and is able to
* clear its background before drawing.
*/
NONSHARABLE_CLASS( CVRLabel )
	: public CEikLabel
	{
	public: // Constructors and destructor

		/**
		* Two-phased constructor
		* @param aContainer Reference to the owning CCoeControl
		*/
		static CVRLabel* NewLC( const CCoeControl& aContainer );

		/**
		* Destructor.
		*/
		~CVRLabel();

	private: // Constructors

		/**
		* Default constructor.
		*/
		inline CVRLabel() {};

		/**
		* 2nd phase constructor.
		* @param aContainer Reference to the owning CCoeControl
		*/
		void ConstructL( const CCoeControl& aContainer );

	private: // from CCoeControl

		/**
		* Draws this label.
		* @param aRect The area that needs updating.
		*/
		void Draw( const TRect& aRect ) const;

	public: // New methods

		/**
		* Sets the text in this label.
		* Leave-safe. Panics if aText is too long.
		* @param aText The new contents.
		*/
		void SetText( const TDesC& aText );

		/**
		* Sets the text in this label.
		* Will draw the label if the given text
		* is different from the current one.
		* Leave-safe. Panics if aText is too long.
		* @param aText The new contents.
		*/
		void SetTextAndDraw( const TDesC& aText );

	private:
		// Skin instance. Not owned.
		MAknsSkinInstance* iSkin;

		TAknLayoutText iTextLayout;
	};

#endif	// __CVRLABEL_H__
