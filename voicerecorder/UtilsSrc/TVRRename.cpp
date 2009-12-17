/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*     This class encapsulates the functionality associated with renaming
*     files in different situation. The implementation is made according
*     to the Folders v.4.0 specification.
*
*
*/


// INCLUDES
#include <f32file.h>
#include <AknQueryDialog.h>
#include <aknnotewrappers.h>
#include <bautils.h>
#include <StringLoader.h>
#include <apparc.h>

#include <voicerecorder.rsg>
#include "voicerecorder.hrh"
#include "TVRRename.h"

// CONSTANTS
const TInt KMaxFileNameLength( 64 );


// ================= MEMBER FUNCTIONS ========================================

// ---------------------------------------------------------------------------
// TVRRename::TVRRename
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TVRRename::TVRRename( RFs& aFs ) : iFs( aFs )
    {
    }


// ---------------------------------------------------------------------------
// TVRRename::GetNewNameL
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TBool TVRRename::GetNewNameL( const TDesC& aOldName, 
									   TDes& aNewName, 
									   const TInt aPrompt )
	{
	CCoeEnv* coeEnv = CCoeEnv::Static();

	TParsePtrC nameParse( aOldName );
	TBuf<KMaxFileNameLength> fileName;		

	// The query can fit only VRMEMONAMEMAXLENGTH characters
	fileName.Copy( nameParse.Name().Left( VRMEMONAMEMAXLENGTH ) );

	CAknStaticSizeDataQuerydialog* nameDlg = new( ELeave ) 
								CAknTextQueryDialog( fileName ); // CSI: 35 #
								
	nameDlg->PrepareLC( R_VR_RENAME_DIALOG );
	HBufC* prompt = StringLoader::LoadLC( aPrompt, coeEnv );
	nameDlg->SetPromptL( *prompt );
	CleanupStack::PopAndDestroy();

	while ( nameDlg->RunLD() != 0 )
		{
		// Construct the full path of the new proposed (by the user) name 
		aNewName.Copy( nameParse.DriveAndPath() );
		aNewName.Append( fileName );
		aNewName.Append( nameParse.Ext() );

		TInt backslash = fileName.Find(_L( "\\" ) );

		// Check that the name is valid for the fs32
		// Is ValidFileName does not check backslash
		if ( iFs.IsValidName( aNewName ) && backslash == KErrNotFound )
			{	
			// Check if it exists?
			if ( !BaflUtils::FileExists( iFs, aNewName ) ||
				aOldName == aNewName )
				{
				// Nope => accepted 
				return ETrue;
				}

			// Prompt the user for "Overwrite?"
			prompt = StringLoader::LoadLC( R_QTN_FLDR_OVERWRITE_QUERY, 
										   fileName, 
										   coeEnv );
										   
			CAknQueryDialog* queryDlg = CAknQueryDialog::NewL();
			TBool accepted( queryDlg->ExecuteLD( R_VR_CONFIRMATION_QUERY, 
												 *prompt ) );

			CleanupStack::PopAndDestroy( prompt );

			if ( accepted )
				{
				// User accepted overwrite
				return ETrue;
				}

			// Create a new dialog for the next round
			// TfileName. (CApaApplication::GenerateFileName needs it)
			TFileName generatedName;	 
			generatedName.Copy( nameParse.DriveAndPath() );
			generatedName.Append( fileName );
			generatedName.Append( nameParse.Ext() );
			User::LeaveIfError( 
					CApaApplication::GenerateFileName( iFs, generatedName ) );
			
			TParsePtrC generatedNameParse( generatedName );
			TPtrC generatedNamePtr( generatedNameParse.Name() );
			if ( generatedNamePtr.Length() > VRMEMONAMEMAXLENGTH )
				{
				// Name got too long => clip the suffix
				fileName.Copy( generatedNamePtr.Left( 
					generatedNamePtr.LocateReverse( '(' ) ) );
				}
			else 
				{
				fileName.Copy( generatedNamePtr );
				}
			
			}
		else
			{
			// The name wasn't a valid fs32 filename => show info note
			HBufC* noteText = coeEnv->AllocReadResourceLC
											( R_QTN_FLDR_ILLEGAL_CHARACTERS );
			CAknInformationNote* infoNote = new( ELeave ) 
												CAknInformationNote( ETrue );
			infoNote->ExecuteLD( *noteText );
			CleanupStack::PopAndDestroy();	// noteText
			}

		nameDlg = new( ELeave ) CAknTextQueryDialog( fileName );
		nameDlg->PrepareLC( R_VR_RENAME_DIALOG );	
		}

	// User canceled
	return EFalse;
	}


// ---------------------------------------------------------------------------
// TVRRename::RenameL
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TBool TVRRename::RenameL( TDes& aName, const TInt aPrompt )
	{
	HBufC* oldName = aName.AllocLC();
	if ( GetNewNameL( *oldName, aName, aPrompt ) )
		{
		TInt err ( BaflUtils::RenameFile( iFs, *oldName, aName ) );
		if ( err )
			{
			// Show error note "Unable to rename %U"
			CAknInformationNote* infoNote = NULL;
			TParsePtrC parse( oldName->Des() );
			HBufC* noteText = StringLoader::LoadLC( R_QTN_FLDR_CANT_RENAME_ITEM, 
													parse.Name(), 
													CCoeEnv::Static() );
		    if ( !noteText )
		    	{
		    	CleanupStack::PopAndDestroy(); // oldName 
		    	return EFalse;
		    	}
			
			infoNote = new( ELeave ) CAknInformationNote( ETrue );
			if ( infoNote )
				{
				infoNote->ExecuteLD( *noteText );
				}
			CleanupStack::PopAndDestroy( 2 ); // oldName, noteText	
			return EFalse;
			}
			
		CleanupStack::PopAndDestroy( oldName );
		return ETrue;
		}
	else
		{
		CleanupStack::PopAndDestroy( oldName );
		return EFalse;
		}
	}


// ---------------------------------------------------------------------------
// TVRRename::RenameL
// The file handle may not point to private folder, since renaming can't be
// used in embedded mode
// ---------------------------------------------------------------------------
//
EXPORT_C TBool TVRRename::RenameL( RFile& aFile, const TInt aPrompt )
	{
	TFileName name( KNullDesC );
	aFile.FullName( name );
	HBufC* oldName = name.AllocLC();
	
	if ( GetNewNameL( *oldName, name, aPrompt ) )
		{
		aFile.Close();
		TInt err ( BaflUtils::RenameFile( iFs, *oldName, name ) );
		if ( err )
			{
			// Show error note "Unable to rename %U"
			// and open the old file again			
			User::LeaveIfError( aFile.Open( iFs, oldName->Des(),
			                                EFileShareReadersOnly ) );
			
			CAknInformationNote* infoNote = NULL;
			TParsePtrC parse( oldName->Des() );
			HBufC* noteText = StringLoader::LoadLC(
			    R_QTN_FLDR_CANT_RENAME_ITEM, parse.Name(), CCoeEnv::Static() );
			
			infoNote = new( ELeave ) CAknInformationNote( ETrue );
			infoNote->ExecuteLD( *noteText );

			CleanupStack::PopAndDestroy( 2 ); // oldName, noteText	
			return EFalse;
			}
		CleanupStack::PopAndDestroy( oldName );
		User::LeaveIfError( aFile.Open( iFs, name, EFileShareReadersOnly ) );
		return ETrue;
		}
	else
		{
		CleanupStack::PopAndDestroy( oldName );
		return EFalse;
		}
	}
