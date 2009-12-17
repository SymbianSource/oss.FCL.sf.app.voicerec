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
*     This class represents a voice memo. It is able to create new files,
*     rename and delete existing files, and to save them to permanent storage.
*
*/


// INCLUDE FILES

#include <eikapp.h>
#include <eikappui.h>
#include <eikenv.h>
#include <AknWaitDialog.h>
#include <AknQueryDialog.h>
#include <StringLoader.h>
#include <bautils.h>
#include <AknGlobalNote.h>
#include <sysutil.h>
#include <pathinfo.h>
#include <systemwarninglevels.hrh>
#include <AknNotifyStd.h>
#include <coeutils.h>

#include <voicerecorder.rsg>
#include <VoiceRecorderUID.h>
#include "CVRMemo.h"
#include "VRConsts.h"
#include "TVRRename.h"
#include "VRUtils.h"

// CONSTANTS
// Max length is 300 millisecs over one minute so possible cropping
// of the file after stop doesn't drop the length under one minute
const TInt KVRMMSMemoMaxRecordLength( 60300000 );
const TInt KVREstimateDelayDuration( 10000000 );


// ================= MEMBER FUNCTIONS ========================================

// ---------------------------------------------------------------------------
// CVRMemo::CVRMemo
// 
// ---------------------------------------------------------------------------
//
CVRMemo::CVRMemo()
	: iDuration( 0 ), iPosition( 0 ), iMaxDuration( KVRDefaultMaxLength ),
	iQuality( EQualityMMSOptimized ),iIsRecorded( EFalse )
	{
#ifndef RD_MULTIPLE_DRIVE	
     iStoragePlace = EMemoStorePhoneMemory;
#else
     TRAP_IGNORE(iStorageDrive = VRUtils::DefaultMemoDriveL());
#endif
	}


// ---------------------------------------------------------------------------
// CVRMemo::~CVRMemo
// 
// ---------------------------------------------------------------------------
//
CVRMemo::~CVRMemo()
	{
	DeleteEmptyFile();
	
	// Close the file handle and file server session
	iFile.Close();
	iFs.Close();

	delete iFileMan;
	}


// ---------------------------------------------------------------------------
// CVRMemo::ConstructL
// 
// ---------------------------------------------------------------------------
//
void CVRMemo::ConstructL(TInt aDefaultAudioFormat)
	{
	User::LeaveIfError( iFs.Connect() );
	iFs.ShareProtected();
	
	iFileMan = CFileMan::NewL( iFs );

	iVRAudioFormat = aDefaultAudioFormat;

	iQuality = VRUtils::QualityL();
	if ( !VRUtils::FeatureEnabled( EVRFeatureShowQualitySetting )
	     || iEmbedded )
		{
		iMaxDuration = KVRMMSMemoMaxRecordLength;
		iQuality = EQualityMMSOptimized;
		}
	else 
		{
		TInt64 max( VRUtils::MaxLengthL() );
		max = max * KVRMinuteAsMicroSeconds;
		iMaxDuration = max;
		}
	
	// Current storage place
#ifndef RD_MULTIPLE_DRIVE	
	iStoragePlace = VRUtils::MemoStoreL();	
#else
    iStorageDrive = VRUtils::MemoDriveL();
#endif	

	}


// ---------------------------------------------------------------------------
// CVRMemo::SetName
// 
// ---------------------------------------------------------------------------
//
void CVRMemo::SetName( const TDesC& aFilename )
	{
	iFilename.Copy( aFilename );
	TParsePtrC parse( iFilename );
	iNamePtr.Set( parse.Name().Left( VRMEMONAMEMAXLENGTH ) );
	}


// ---------------------------------------------------------------------------
// CVRMemo::SetTemporaryNameL
// 
// ---------------------------------------------------------------------------
//
void CVRMemo::SetTemporaryNameL( TBool aEmbedded )
	{
	iEmbedded = aEmbedded;

	// We can use the same handle, no need to create new name	
	// Empty the file so it can be overwritten with new memo
	if ( iEmbedded && !iExternalFileHandle )
		{
		if ( iFile.SubSessionHandle() )
			{
			iFile.SetSize( 0 );
			return;
			}
		}		
	
	// Checks if the file handle can be found i.e if file is created 	
	if ( iFile.SubSessionHandle() )
		{	
		// Retrieving new settings
		TVRQuality newQuality( VRUtils::QualityL() );

#ifndef RD_MULTIPLE_DRIVE
		TVRMemoStore newStoragePlace ( VRUtils::MemoStoreL() );			

		// If the current file is empty, we can reuse it if quality or 
		// the storage place hasn't changed
		if ( iQuality == newQuality && iStoragePlace == newStoragePlace )	
		    {
    	    TInt size( 0 );		
    		iFile.Size( size );
			
			// If current file has already been recorded to, a new file has 
			// to be generated 
    		if ( !IsRecorded() )    		
    			{
    			iFile.SetSize( 0 );
    			return;
    			}
    		}
    	// The file has been created but the settings have been changed 
    	// before using the file -> delete file and create a new one
	    else
	    	{
	    	DeleteEmptyFile();
	    	}

#else
        // for Multiple drives
		TInt newStorageDrive ( VRUtils::MemoDriveL() );			

		// If the current file is empty, we can reuse it if quality or 
		// the storage place hasn't changed
		if ( iQuality == newQuality && iStorageDrive == newStorageDrive )	
		    {
    	    TInt size( 0 );		
    		iFile.Size( size );
			
			// If current file has already been recorded to, a new file has 
			// to be generated 
    		if ( !IsRecorded() )    		
    			{
    			iFile.SetSize( 0 );
    			return;
    			}
    		}
    	// The file has been created but the settings have been changed 
    	// before using the file -> delete file and create a new one
	    else
	    	{
	    	DeleteEmptyFile();
	    	}


#endif
	    }

	TFileName memoName;	
    // Retrieve storage path
	if ( iSavingLocation.Length() > 0 )
		{
		memoName = iSavingLocation;
		}
	else
		{
		VRUtils::MemoStoreDirectoryL( memoName );
		}

	// Recheck the quality before naming memo
	if ( !VRUtils::FeatureEnabled( EVRFeatureShowQualitySetting )
	     || iEmbedded )
		{
		iQuality = EQualityMMSOptimized;
		}
    else
        {
	    iQuality = VRUtils::QualityL();
        }
	
	// Current storage place
#ifndef RD_MULTIPLE_DRIVE		
	iStoragePlace = VRUtils::MemoStoreL();
#else
    iStorageDrive = VRUtils::MemoDriveL();
#endif

    // Choose the file type
	// Use amr if quality is MMS Optimized or we are recording
	// in embedded mode, wav otherwise    
	TVRFiletype type( EVRFileAmr );
    if( iEmbedded || iQuality == EQualityMMSOptimized )
		{
		type = EVRFileAmr;
		}
		
// **** updated for new CR, if it QualitySetting is Normal, save as WAV

#ifdef  __AAC_ENCODER_PLUGIN
    else if (iQuality == EQualityNormal)
		{
		type = EVRFileWav;
		}
		
// ****  the following are updated for new CR, if it QualitySetting is High, save as mp4
    else if (iQuality == EQualityHigh)
		{
		type = EVRFileAAC_LC;
		}
#else

	else
		{
		type = EVRFileWav;
		}

#endif

	// Generate unique final file name
    VRUtils::GenerateUniqueFilenameL( iFs, memoName, type );

	// Make sure that file handles are not leaked
	if ( iFile.SubSessionHandle() )
		{
		iFile.Close();
		}
	
	// Ensure that path exists
	BaflUtils::EnsurePathExistsL( iFs, memoName );
	
// Open the memo file
 
#ifdef  __AAC_ENCODER_PLUGIN 
    if((iQuality == EQualityHigh))  //for mp4 format	, 3gplib does not support EFileShareExclusive so EFileShareAny is used here
    	{
        User::LeaveIfError( iFile.Create( iFs, memoName, EFileWrite|EFileShareAny) );
    	}
    else  // for other formats
    	{
        User::LeaveIfError( iFile.Create( iFs, memoName, EFileShareExclusive|EFileWrite ) );		

    	}
#else  // it is not mp4, so still use the old flag
        User::LeaveIfError( iFile.Create( iFs, memoName, EFileShareExclusive|EFileWrite ) );		
 
#endif

    TInt error = iFile.SetAtt(KEntryAttHidden, KEntryAttNormal );	  		
	SetName( memoName );
	}


// ---------------------------------------------------------------------------
// CVRMemo::QueryAndDeleteL
// 
// ---------------------------------------------------------------------------
//
TBool CVRMemo::QueryAndDeleteL()
	{
	//Delete?\n%U" �qtn.query.common.conf.delete�
	TParsePtrC parse( iFilename );
	HBufC* text = StringLoader::LoadLC( R_QTN_QUERY_COMMON_CONF_DELETE,
										parse.Name() );

	// Show confirm note
    CAknQueryDialog* dlg = CAknQueryDialog::NewL();
    TInt result( dlg->ExecuteLD( R_VR_CONFIRMATION_QUERY, *text ) );
    CleanupStack::PopAndDestroy( text );

	if ( result )
		{
		DeleteL();
		return ETrue;
		}

	return EFalse;
	}


// ---------------------------------------------------------------------------
// CVRMemo::DeleteL
// 
// ---------------------------------------------------------------------------
//
void CVRMemo::DeleteL()
	{
	if ( iFile.SubSessionHandle() )
		{
		if ( iExternalFileHandle )
			{
			// We shouldn't delete the file handle, so let's
			// just empty the file
			iFile.SetSize( 0 );
			}
		else
			{
			TFileName fileName( KNullDesC );
			iFile.FullName( fileName );
			iFile.Close();

			TInt err( iFileMan->Delete( fileName ) );
			if ( err != KErrNone && err != KErrNotFound )
				{
				// Try to open the file again so we wont end up
				// in goofy state without open file
				User::LeaveIfError( iFile.Open( iFs, fileName,
				                      EFileShareReadersOnly ) );
				User::LeaveIfError( err );
				}
			}
		}
	SetName( KNullDesC );
	}


// ---------------------------------------------------------------------------
// CVRMemo::QueryAndRenameL
// 
// ---------------------------------------------------------------------------
//
TBool CVRMemo::QueryAndRenameL()
	{
	TVRRename renamer( iFs );
	if ( renamer.RenameL( iFile, R_QTN_FLDR_ITEM_NAME_PRMPT ) )
		{
		TFileName name( KNullDesC );
		iFile.FullName( name );
		SetName( name );
		
		return ETrue;
		}
	return EFalse;
	}


// ---------------------------------------------------------------------------
// CVRMemo::SavePermanentlyL
// 
// ---------------------------------------------------------------------------
//
void CVRMemo::SavePermanentlyL( CAknGlobalNote* /*aWaitNote*/, 
								TInt& /*aNoteId*/,
								const TDesC& /*aLabel*/, 
								TBool /*aProduceCopy*/ )

	{
	// Don't do anything if recording to external file handle
	if ( iExternalFileHandle )
	    {
	    return;
	    }
	
	// Change file open mode to read
	TFileName name( KNullDesC );
	iFile.FullName( name );
	iFile.Close();
	User::LeaveIfError( iFile.Open( iFs, 
									name, 
									EFileRead|EFileShareReadersOnly) );	
	}


// ---------------------------------------------------------------------------
// CVRMemo::IsValid
// 
// ---------------------------------------------------------------------------
//
TBool CVRMemo::IsValid() const
	{
	return iFile.SubSessionHandle() == 0 ? EFalse : ETrue;
	}


// ---------------------------------------------------------------------------
// CVRMemo::UpdateModifiedDate
// 
// ---------------------------------------------------------------------------
//
void CVRMemo::UpdateModifiedDate()
	{
	if ( IsValid() )
		{
		TLocale locale;
		iFile.Modified( iDateCreated );		
		iDateCreated += locale.UniversalTimeOffset();
		}
	}


// ---------------------------------------------------------------------------
// CVRMemo::SetSavingLocationL
// 
// ---------------------------------------------------------------------------
//
void CVRMemo::SetSavingLocationL( const TDesC& aPath )
	{
	iSavingLocation = aPath;
	}


// ---------------------------------------------------------------------------
// CVRMemo::DeleteEmptyFile
// Deletes an empty file that hasn't been recorded into. After deleting 
// also decreases central repository's memo count value
// ---------------------------------------------------------------------------
//	
TBool CVRMemo::DeleteEmptyFile()
	{
	if ( iFile.SubSessionHandle() != 0 )
		{
		TInt size( 0 );
        
        // Error code ignored
        iFile.Size( size );
		
		if ( !iIsRecorded )
		    {
		    TFileName name( KNullDesC );
		    iFile.FullName( name );
		    
			iFile.Close();
			iFileMan->Delete( name );			
	
			// Central repository value has to be decreased by one because it 
			// was increased earlier, when current filename was generated
			VRUtils::SetMemoCount( VRUtils::MemoCount() - 1 );
			
			return ETrue;
    	    }
		}
		return EFalse;
	}


// ---------------------------------------------------------------------------
// CVRMemo::SetFileHandle
// 
// ---------------------------------------------------------------------------
//	
void CVRMemo::SetFileHandle( RFile& aFile, const TBool aEmbedded )
	{
	iEmbedded = aEmbedded;
	iExternalFileHandle = ETrue;
	
	iFile = aFile;

	// Set the correct name for UI
	TFileName name( KNullDesC );
	iFile.FullName( name );
	SetName( name );
	}


// ---------------------------------------------------------------------------
// CVRMemo::MaxDuration
// Returns in microseconds the maximum time that can be still recorded with 
// current settings (codecs and mem storage place)
// ---------------------------------------------------------------------------
//	
const TTimeIntervalMicroSeconds& CVRMemo::MaxDuration()
	{
	if ( iEmbedded || 
		 !VRUtils::FeatureEnabled( EVRFeatureShowQualitySetting ) ||
	     iQuality == EQualityMMSOptimized )
		{
		//Voice Recorder change to remove 1 Min. limit for AMR for Stand alone recording
		//if embedded allow 1 min recording for EQualityMMSOptimized(AMR)
		//else allow 1 hour recording
		if(iEmbedded)
			{
			iMaxDuration = KVRMMSMemoMaxRecordLength;
			return iMaxDuration;
			}		
		}

	// Make the first estimate after KVRFirstEstimateTime seconds recording
	if ( Duration() < KVRFirstEstimateTime )
		{
		// Fetch the setting for high quality max length
		TInt64 max( 0 );
		TRAPD( err, max = VRUtils::MaxLengthL() );
		if ( err != KErrNone )
		    {
		    max = KVRMMSMemoMaxRecordLength;
		    }

		max = max * KVRMinuteAsMicroSeconds;
		iMaxDuration = max;

		// Reset the time of last estimate
		TDateTime date;
		date.SetYear( -1 );
		iLastEstimate = date; // invalid
		
		return iMaxDuration;
		}

	// Make new estimate if there's no last estimate or if 10 secs have passed
	// from the previous estimate
	TTime currentTime;
	currentTime.HomeTime();
	if ( iLastEstimate.DateTime().Year() == -1 ||
		currentTime.MicroSecondsFrom( iLastEstimate ) >=
		        TTimeIntervalMicroSeconds( KVREstimateDelayDuration ) )
		{
		iLastEstimate = currentTime;
		
		TEntry fileEntry;
		TFileName name( KNullDesC );
		iFile.FullName( name );		

		TInt err = iFs.Entry( name, fileEntry );
		if( err != KErrNone )
			{
			return iMaxDuration;
			}

		// Retrieve free space
		TVolumeInfo volInfo;

// old storage
#ifndef RD_MULTIPLE_DRIVE   
		TVRMemoStore memoStore( EMemoStorePhoneMemory );
		TRAP( err, memoStore = VRUtils::MemoStoreL() );
        if ( err != KErrNone )
            {
            memoStore = EMemoStorePhoneMemory;
		    }
 
		    if ( memoStore == EMemoStorePhoneMemory )
			{
			err = iFs.Volume( volInfo, EDriveC );			
			}
		else // memostore is MMC 
			{
			err = iFs.Volume( volInfo, EDriveE );
			}
			
// multiple drive
#else

        TInt drive = 0;
        TRAP_IGNORE(drive = VRUtils::DefaultMemoDriveL());
		TRAP( err, drive = VRUtils::MemoDriveL() );
        if ( err != KErrNone )
            {
            TRAP_IGNORE(drive = VRUtils::DefaultMemoDriveL());
		    }       
		err = iFs.Volume( volInfo, drive );			

#endif

		if( err != KErrNone )
			{

			return iMaxDuration;
			}
			
		// Calculate the current disk consumption "speed" of the memo
		TReal speed( fileEntry.iSize / ( Duration().Int64() / 
			KVRSecondAsMicroSeconds ) );
		
		// The total free memory
		TInt64 freeSpace(volInfo.iFree);
		// Free memory if Critical Memory isn't taken into account
		TInt64 freeSpaceMinusCrlevel( freeSpace -  KDRIVECCRITICALTHRESHOLD);
		
		// Estimate the time left
		TInt64 value( freeSpaceMinusCrlevel / speed );
		TTimeIntervalMicroSeconds estimate( value * KVRSecondAsMicroSeconds );

		// Estimate should include also the length of clip
		estimate = TTimeIntervalMicroSeconds( estimate.Int64()
										 + Duration().Int64() );
		if ( estimate < iMaxDuration)
			{
			iMaxDuration = estimate;
			}
		}

	return iMaxDuration;
	}


// ---------------------------------------------------------------------------
// CVRMemo::IsRecorded
// Returns the attribute iIsRecorded value that indicates if the recording of
// the clip currently open is started or not
// ---------------------------------------------------------------------------
//	
TBool CVRMemo::IsRecorded() const
	{
	return iIsRecorded;
	}	 
		

// ---------------------------------------------------------------------------
// CVRMemo::SetRecorded
// Sets the value of iIsRecorded attribute
// ---------------------------------------------------------------------------
//		
void CVRMemo::SetRecorded( TBool aRecorded )
	{
	iIsRecorded = aRecorded;
	}
	
// End of file
