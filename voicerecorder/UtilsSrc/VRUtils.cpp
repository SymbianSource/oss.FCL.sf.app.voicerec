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
*     This class contains a collection of small static utility functions
*     needed all around the application. The class has no prefix letter
*
*
*/


// INCLUDES

#include <eikapp.h>
#include <eikenv.h>
#include <bautils.h>
#include <centralrepository.h>
#include <coemain.h>
#include <f32file.h>
#include <StringLoader.h>
#include <AknGlobalNote.h>
#include <aknnotewrappers.h>
#include <e32property.h>
#include <ctsydomainpskeys.h>
#include <pathinfo.h>
#include <featmgr.h>
#include <avkon.rsg>
#include <e32math.h>

#include <voicerecorder.rsg>
#include <VoiceRecorderUID.h>

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#include <pathinfo.h>
#endif

#include "VoiceRecorderInternalCRKeys.h"
#include "VRUtils.h"
#include "VRConsts.h"
#include <bldvariant.hrh>

// CONSTANTS
_LIT( KVRAMRFileNameExtension,   ".amr" );
_LIT( KVRWAVFileNameExtension,   ".wav" );

// added for AAC
#ifdef  __AAC_ENCODER_PLUGIN   
_LIT( KVRAAC_LCFileNameExtension,".mp4" );
#endif

_LIT( KVROneToNineClipCountFormat, "%02d" ); // used with clip numbers 1..9
_LIT( KVRDefaultClipCountFormat, "%d" );	// used with clip numbers 10->


// ================= MEMBER FUNCTIONS ========================================

// ---------------------------------------------------------------------------
// VRUtils::MemoStoreDirectoryL
// Checks what is the current storage place and returns the corresponding path
// ---------------------------------------------------------------------------
//
EXPORT_C void VRUtils::MemoStoreDirectoryL( TDes &aPath )
	{
	
	TFileName phoneMemoryPath;
	
#ifndef RD_MULTIPLE_DRIVE
	if ( FeatureManager::FeatureSupported( KFeatureIdMmc ) )
		{
   		
		TVRMemoStore memoStore = MemoStoreL();
		if ( memoStore == EMemoStorePhoneMemory )
			{
			phoneMemoryPath = PathInfo::PhoneMemoryRootPath();
			phoneMemoryPath.Append( PathInfo::DigitalSoundsPath() );
			aPath.Copy( phoneMemoryPath );
			return;
			}
		else
			{
			// MMC is the selected memo store. Check if it's available.
			TVRDriveInfo mmcInfo;
			GetMMCInfo( mmcInfo );
			if ( mmcInfo.iDrivePresent && !mmcInfo.iDriveLocked &&
				!mmcInfo.iDriveCorrupted && !mmcInfo.iDriveReadOnly )
				{
				// MMC usage is OK.
				TFileName mmcPath = PathInfo::MemoryCardRootPath();
				mmcPath.Append( PathInfo::DigitalSoundsPath() );
				aPath.Copy( mmcPath );
				return;
				}
			else
				{
				// MMC is unavailable => return phone memory path
				phoneMemoryPath = PathInfo::PhoneMemoryRootPath();
				phoneMemoryPath.Append( PathInfo::DigitalSoundsPath() );
				aPath.Copy( phoneMemoryPath );
				return;
				}

			}

		}
	else
		{
		phoneMemoryPath = PathInfo::PhoneMemoryRootPath();
		phoneMemoryPath.Append( PathInfo::DigitalSoundsPath() );
		aPath.Copy( phoneMemoryPath );
		return;
		}

// using multiple drive
#else
		TInt memoDrive = MemoDriveL();
		TInt defaultDrive(0);
        User::LeaveIfError( defaultDrive = DefaultMemoDriveL());
        //default drive
		if ( memoDrive == defaultDrive )
			{
            User::LeaveIfError( PathInfo::GetRootPath( phoneMemoryPath, memoDrive ) );
			phoneMemoryPath.Append( PathInfo::DigitalSoundsPath() );
			aPath.Copy( phoneMemoryPath );
			return;
			}
		// other drives
		else
			{			
            TUint status( 0 );
            TInt err = VRUtils::GetDriveInfo(memoDrive, status);
            // check if drive status is ok 
			if ( ( err == KErrNone ) && (status & DriveInfo::EDrivePresent) && !(status & DriveInfo::EDriveLocked) &&
				!( status & DriveInfo::EDriveCorrupt ) && !( status & DriveInfo::EDriveReadOnly ) )
				{
				// Drive usage is OK.
                User::LeaveIfError( PathInfo::GetRootPath( phoneMemoryPath, memoDrive ) );
			    phoneMemoryPath.Append( PathInfo::DigitalSoundsPath() );
			    aPath.Copy( phoneMemoryPath );
			    return;
				}
			
			// if not ok, save to default device drive
			else
				{
            memoDrive = defaultDrive;
            User::LeaveIfError( PathInfo::GetRootPath( phoneMemoryPath, memoDrive ) );
			phoneMemoryPath.Append( PathInfo::DigitalSoundsPath() );
			aPath.Copy( phoneMemoryPath );
			return;
				}

			}
#endif
	}


// ---------------------------------------------------------------------------
// VRUtils::SetMemoStoreL
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void VRUtils::SetMemoStoreL( TVRMemoStore aMemory )
	{
	if ( FeatureManager::FeatureSupported( KFeatureIdMmc ) )
		{
		SetSettingValueL( KVRMemoStore, aMemory );
		}
	else
		{
		__ASSERT_DEBUG( ETrue, User::Panic( KVRPanic, EPanicInvalidSetting ) );
		}
	}


// ---------------------------------------------------------------------------
// VRUtils::MemoStoreL
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TVRMemoStore VRUtils::MemoStoreL()
	{
	// Phone memory is the default
	if ( FeatureManager::FeatureSupported( KFeatureIdMmc ) )
		{
		TInt ret = SettingValueL( KVRMemoStore,
								EMemoStorePhoneMemory );
		return static_cast< TVRMemoStore >( ret );
		}
	else
		{
		return EMemoStorePhoneMemory;
		}
	}


// ---------------------------------------------------------------------------
// VRUtils::DefaultSpeakerL
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TVRSpeaker VRUtils::DefaultSpeakerL()
	{
	if ( FeatureManager::FeatureSupported( KFeatureIdKeypadNoVoiceKey ) &&
		FeatureManager::FeatureSupported( 
							KFeatureIdApplicationControllableAudioRouting ) )
		{
		return ( TVRSpeaker ) SettingValueL( KVRDefaultSpeaker, ESpeakerIhf );
		}
	else
		{
		return ESpeakerIhf;
		}
	}


// ---------------------------------------------------------------------------
// VRUtils::SetDefaultSpeakerL
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void VRUtils::SetDefaultSpeakerL( const TVRSpeaker aSpeaker )
	{
	if ( FeatureManager::FeatureSupported( KFeatureIdKeypadNoVoiceKey ) &&
		FeatureManager::FeatureSupported( 
							KFeatureIdApplicationControllableAudioRouting ) )
		{
		SetSettingValueL( KVRDefaultSpeaker, aSpeaker );
		}
	else
		{
		__ASSERT_DEBUG( ETrue, User::Panic( KVRPanic, EPanicInvalidSetting ) );
		}
	}


// ---------------------------------------------------------------------------
// VRUtils::QualityL
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TVRQuality VRUtils::QualityL()
	{
    if ( VRUtils::FeatureEnabled( EVRFeatureShowQualitySetting ) )
        {   	
     	return ( TVRQuality ) SettingValueL( KVRQuality, 
     											 EQualityMMSOptimized );
 		}
	else
		{
		return EQualityMMSOptimized;
		}
	}


// ---------------------------------------------------------------------------
// VRUtils::SetQualityL
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void VRUtils::SetQualityL( const TVRQuality aQuality )
	{
    if ( VRUtils::FeatureEnabled( EVRFeatureShowQualitySetting ) )
        {
		SetSettingValueL( KVRQuality, aQuality );
		}
	else
		{
		__ASSERT_DEBUG( ETrue, User::Panic( KVRPanic, EPanicInvalidSetting ) );
		}
	}


// ---------------------------------------------------------------------------
// VRUtils::SetDefaultVolumeL
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void VRUtils::SetDefaultVolumeL( const TVRSpeaker aSpeaker,
	const TInt aVolume )
	{
	switch ( aSpeaker )
		{
		case ESpeakerEarPiece:
			{
			SetSettingValueL( KVREarPieceVolume, aVolume );
			break;
			}
		case ESpeakerIhf:
			{
			SetSettingValueL( KVRIHFVolume, aVolume );
			break;
			}
		default:
			{
			break;
			}
		}	
	return;
	}


// ---------------------------------------------------------------------------
// VRUtils::DefaultVolumeL
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt VRUtils::DefaultVolumeL( const TVRSpeaker aSpeaker )
	{
	TInt volume( KVRDefaultVolume );
	switch ( aSpeaker )
		{
		case ESpeakerEarPiece:
			{
			volume = SettingValueL( KVREarPieceVolume, KVRDefaultVolume );
			break;
			}
		case ESpeakerIhf:
			{
			volume = SettingValueL( KVRIHFVolume, KVRDefaultVolume );
			break;
			}
		default:
			{
			break;
			}
		}
	return volume ? volume : KVRDefaultVolume;
	}



// ---------------------------------------------------------------------------
// VRUtils::SetSettingValueL, 
// save the data to central repository
// ---------------------------------------------------------------------------
//
void VRUtils::SetSettingValueL( const TUint32 aKey, const TInt aValue )
	{
    CRepository* repository = CRepository::NewL( KCRUidVoiceRecorder );
    CleanupStack::PushL( repository );
    User::LeaveIfError( repository->Set( aKey, aValue ) );
    CleanupStack::PopAndDestroy( repository );
	}


// ---------------------------------------------------------------------------
// VRUtils::SettingValueL
// 
// ---------------------------------------------------------------------------
//
TInt VRUtils::SettingValueL( const TUint32 aKey, const TInt aDefaultValue )
	{
    TInt data;

    CRepository* repository = CRepository::NewL( KCRUidVoiceRecorder );
	CleanupStack::PushL( repository );

	// Get the value, create the key if no value was found
    if ( repository->Get( aKey, data ) != KErrNone )
        {
        User::LeaveIfError( repository->Create( aKey, aDefaultValue ) );
        data = aDefaultValue;
        }

    CleanupStack::PopAndDestroy( repository );

	return data;
	}


// ---------------------------------------------------------------------------
// VRUtils::GetMMCInfo
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void VRUtils::GetMMCInfo( TVRDriveInfo& aMMCInfo )
    {
	if ( FeatureManager::FeatureSupported( KFeatureIdMmc ) )
		{
		TDriveInfo driveInfo;
		TVolumeInfo volumeInfo;
		RFs& fss = CCoeEnv::Static()->FsSession() ;

		TInt err( fss.Drive( driveInfo, EDriveE ) );

		if ( driveInfo.iType != EMediaNotPresent )
			{
			aMMCInfo.iDrivePresent = ETrue;
			if ( fss.Volume( volumeInfo, EDriveE ) == KErrCorrupt )
				{
				aMMCInfo.iDriveCorrupted = ETrue;
				}
			else
				{
				aMMCInfo.iDriveCorrupted = EFalse;
				}
			}
		else
			{
			aMMCInfo.iDriveCorrupted = EFalse;
			aMMCInfo.iDrivePresent = EFalse;
			}

		if ( driveInfo.iMediaAtt & KMediaAttLocked || err == KErrLocked )
			{
			aMMCInfo.iDriveLocked = ETrue;
			}
		else
			{
			aMMCInfo.iDriveLocked = EFalse;
			}

		if ( driveInfo.iMediaAtt & KMediaAttWriteProtected )
			{
			aMMCInfo.iDriveReadOnly = ETrue;
			}
		else
			{
			aMMCInfo.iDriveReadOnly = EFalse;
			}
		if ( volumeInfo.iFree ==0 )
			{
			aMMCInfo.iDriveFull = ETrue;
			}
		else
			{
			aMMCInfo.iDriveFull = EFalse;
			}


		}
    }


// ---------------------------------------------------------------------------
// VRUtils::ShowMemoryFullConfirmationQuery
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void VRUtils::ShowMemoryFullConfirmationQuery( TBool aMmc )
	{
	// Error ignored. If the note fails, there's nothing to do.
	TRAP_IGNORE( ShowMemoryFullConfirmationQueryL( aMmc ) );
	}


// ---------------------------------------------------------------------------
// VRUtils::UnsupportedCallTypeOngoing
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TBool VRUtils::UnsupportedCallTypeOngoing( TBool aShowNote )
	{
	TBool unsupported( EFalse );
	TInt value( 0 );

	TInt err( 0 );
    RProperty property;
    err = property.Attach( KPSUidCtsyCallInformation, KCTsyCallState );
	if ( err )
		{
		return EFalse;
		}

    property.Get( value );

    switch ( value )
	    {
    	case EPSCTsyCallStateUninitialized:
    	case EPSCTsyCallStateNone:
    		{ // No any kind of call in progress
    		return EFalse;
    		}
		default: // Call in progress, check if it is videocall
		    {
		    err = property.Attach( KPSUidCtsyCallInformation, KCTsyCallType);
        	if ( err )
        		{
        		return EFalse;
        		}
        	value = 0;
    		property.Get( value );
			break;
		    }
    	}

	switch( value )
		{
		// Video call ongoing
		case EPSCTsyCallTypeH324Multimedia:
			{
			unsupported = ETrue;
			
			if ( aShowNote )
				{
				// If the note fails, it's no big deal
				TRAP_IGNORE( ShowUnsupportedCallTypeNoteL( 
					R_VR_VIDEO_CALL_INFONOTE_LABEL ) );
				}
			break;			
			}
		// Voip call ongoing
		case EPSCTsyCallTypeVoIP:
			{
			unsupported = ETrue;
			
			if ( aShowNote )
				{
				// If the note fails, it's no big deal
				TRAP_IGNORE( ShowUnsupportedCallTypeNoteL( 
					R_VR_INTERNET_CALL_INFONOTE_LABEL ) );
				}
			break;			
			}			
		}
	
	return unsupported;
	}


// ---------------------------------------------------------------------------
// VRUtils::ShowMemoryFullConfirmationQueryL
// 
// ---------------------------------------------------------------------------
//
void VRUtils::ShowMemoryFullConfirmationQueryL( TBool aMmc )
	{
	CAknGlobalNote* note = NULL;
	
	note = CAknGlobalNote::NewLC();
	
	HBufC* label = NULL;
	TInt noteId( 0 );
	if ( aMmc )
		{
		noteId = R_VR_MEMORY_MMC_WARNING;
		}
	else 
		{
		noteId = R_VR_MEMORY_WARNING;
		}

	label = StringLoader::LoadLC( noteId );

	note->SetSoftkeys( R_AVKON_SOFTKEYS_OK_EMPTY );
	note->ShowNoteL( EAknGlobalWarningNote, *label );
	CleanupStack::PopAndDestroy( 2 );	// label, note;
	}


// ---------------------------------------------------------------------------
// VRUtils::ShowUnsupportedCallTypeNoteL
// 
// ---------------------------------------------------------------------------
//
void VRUtils::ShowUnsupportedCallTypeNoteL( TInt aResourceId )
	{
	CAknInformationNote* infoNote;
    HBufC* noteText;
    
    noteText = CEikonEnv::Static()->
    	AllocReadResourceLC( aResourceId );
	
	infoNote = new( ELeave ) CAknInformationNote( ETrue );
	infoNote->ExecuteLD( *noteText );

	CleanupStack::PopAndDestroy( noteText );
	}


// ---------------------------------------------------------------------------
// VRUtils::MaxLengthL
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt VRUtils::MaxLengthL()
	{
	// If no value can be retrieved, use 60 minutes as max length
	TInt value( 0 );
	
#ifdef __AAC_ENCODER_PLUGIN   // mp4 is supported now. 	
    if (QualityL() == EQualityHigh)
    	{	
	    TRAPD( err, value = SettingValueL( KVRAacRecordLength, 
									   KVRDefaultAacRecordLength ) );
									   
	    if ( err || value <= 0 || value > 9999 )
		  {
		  return KVRDefaultAacRecordLength;
		  }
    	}
#endif
	TRAPD( err, value = SettingValueL( KVRMaxRecordTime, 
									   KVRDefaultMaxLength ) );
									   
	if ( err || value <= 0 || value > 9999 )
		{
		return KVRDefaultMaxLength;
		}


	return value;
	}


// ---------------------------------------------------------------------------
// VRUtils::FeatureEnabled
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TBool VRUtils::FeatureEnabled( TVRFeature aFeature )
	{
	TBool ret( EFalse );
    TInt data( 0 );

    TRAPD( err, data = SettingValueL( KVRVariationFlags, 0 ) );
    if( err != KErrNone )
        {
        return EFalse;
        }

    // Compare feature to bitmask
	if ( data & aFeature )
		{
		ret = ETrue;
		}
	return ret;
	}


// ---------------------------------------------------------------------------
// VRUtils::GenerateUniqueFilename
// Generates unique filename that doesn't exist in the directories where VR
// operates in. Unique filename is generated in Media Gallery sense so that no
// two sound clips can have same name there
// ---------------------------------------------------------------------------
//
EXPORT_C void VRUtils::GenerateUniqueFilenameL( RFs& aFs, TFileName& aName,
    TVRFiletype aType )
    {
           
    TBuf< VRLABELMAXLENGTH > valueStr;
    TBuf< VRLABELMAXLENGTH > formatStr;
    TFileName name( aName );
    TFileName clipName; // Used for file existence checking
    TBool fileExists( EFalse );
    
    // Get current count from cenrep
    TInt value( MemoCount() );
    		
    TBool uniqueFound( EFalse );
    
    // First memo
    if ( !value )
        {        
        StringLoader::Load( formatStr, R_VOREC_FIRST_MEMO_NAME );        
        name.Append( formatStr );
        clipName.Append( formatStr );
        
        TRAPD( err1, fileExists = DoesFileExistL( aFs, clipName ) );
        if ( err1 )
        	{
        	// if error occurs, no much to do->generate new filename
        	fileExists = ETrue;
        	}
        	
        AppendExtension( name, aType );
        
        if ( fileExists )
            {
            // Memo exists, continue name generation with next name
            ++value;
            }
        else
            {
            uniqueFound = ETrue;
            ++value; // This is saved to cenrep
            }
        }

    // Continue generation
    if ( !uniqueFound )
        {
        StringLoader::Load( formatStr, R_VOREC_DEFAULT_MEMO_NAME );


        // Loop until unique filename is generated
        FOREVER
            {
            // 0 is added before clip number when number is between 1 and 9
            if(value >= 1 && value <= 9)  
            	{
            	valueStr.Format( KVROneToNineClipCountFormat, value);	
            	}
            // no addings for clip numbers 10->
            else
            	{
            	valueStr.Format( KVRDefaultClipCountFormat, value);		
            	}

            StringLoader::Format( name, formatStr, -1, valueStr );

            // name copied before the path is added          
            clipName = name;
            
            // Insert path to beginning	
            name.Insert( 0, aName );
            
            AknTextUtils::LanguageSpecificNumberConversion( name );
        	    
           	TRAPD( err2, fileExists = DoesFileExistL( aFs, clipName ) );
           	if ( err2 )
	        	{
	        	fileExists = ETrue;
	        	}
  
            AppendExtension( name, aType );
            if ( !fileExists )
                {
                // Accept the name and increase value
                // Value increasing is mandatory to prevent similarly named clips when quality 
                // or store place is changed
                ++value;
                break;
                }
            // Try next...
            ++value;                
            }
         }

    aName = name;

    // Save the new value to cenrep
   SetMemoCount( value );
    }
   

// ---------------------------------------------------------------------------
// VRUtils::AppendExtension
// 
// ---------------------------------------------------------------------------
//    
void VRUtils::AppendExtension( TFileName& aName, TVRFiletype aType ) 
    {
    switch ( aType )
        {
        case EVRFileWav:
            {
            aName.Append( KVRWAVFileNameExtension );
            break;
            }

//Jeffery: the following case added for the new CR
#ifdef  __AAC_ENCODER_PLUGIN   
         case EVRFileAAC_LC:
            {
            aName.Append( KVRAAC_LCFileNameExtension );
            break;
            }
#endif

        // In error case amr extension is appended
        case EVRFileAmr:
        default:
            {
            aName.Append( KVRAMRFileNameExtension );
            break;
            }                    
        }    
    }


// ---------------------------------------------------------------------------
// VRUtils::MemoCount
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt VRUtils::MemoCount()
    {
	// If no value can be retrieved, use zero
	TInt value( 0 );
	TRAPD( err, value = SettingValueL( KVRMemoCount, 0 ) );
	if ( err != KErrNone )
		{
		return 0;
		}
	return value;    
    }


// ---------------------------------------------------------------------------
// VRUtils::SetMemoCount
// 
// ---------------------------------------------------------------------------
//    
EXPORT_C void VRUtils::SetMemoCount( TInt aNewCount )
    {
    TRAP_IGNORE( SetSettingValueL( KVRMemoCount, aNewCount ) );
    }    


// ---------------------------------------------------------------------------
// VRUtils::AMRBitrateL
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TUint VRUtils::AMRBitrateL()
    {
    TUint bitrate(KVRDefaultAmrBitrate);

    // Fetch bitrate from 
	TRAPD( err, bitrate = SettingValueL( KVRBitrateAmr, 
										 KVRDefaultAmrBitrate ) );
										 
	if ( err || bitrate <= 0 || bitrate > 12200 )
		{
		bitrate = KVRDefaultAmrBitrate;
		}
		
    return bitrate;    
    }


// ---------------------------------------------------------------------------
// VRUtils::AACBitrateL
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TUint VRUtils::AACBitrateL()
    {
       
    TUint bitrate(KVRDefaultBitrateAac);

    // Fetch bitrate from 
	TRAPD( err, bitrate = SettingValueL( KVRBitrateAac, 
										 KVRDefaultBitrateAac ) );

	if ( err || bitrate <= 8000 || bitrate > 288000 )
	  {
      bitrate = KVRDefaultBitrateAac;
	  }
    return bitrate;    
    }

// ---------------------------------------------------------------------------
// VRUtils::DoesFileExistL
// Makes the full checking if the filename (aName) exists in the directories 
// that Voice Recorder uses for clip storage
// ---------------------------------------------------------------------------
//
TBool VRUtils::DoesFileExistL( RFs& aFs, TFileName aName )
	{
// old memory storage
#ifndef RD_MULTIPLE_DRIVE	
	// Storage place at the moment
	TVRMemoStore memoStore = MemoStoreL();
	TFileName path;
	TFileName fileName(aName);
	
	// The path for the current storage place
	VRUtils::MemoStoreDirectoryL( path );
	
	// Add the path to the filename
    fileName.Insert( 0, path );
    
    if ( CheckFileExistence( aFs, fileName ) )
    	{
    	return ETrue;
    	}
    
    // Check possible alternative storage place
    TVRMemoStore secondaryStore;
	if ( memoStore == EMemoStorePhoneMemory )
		{
		if ( FeatureManager::FeatureSupported( KFeatureIdMmc ) )
			{
			secondaryStore = EMemoStoreMMC;	
			}
		else
			{
			secondaryStore = EMemoStorePhoneMemory;	
			}	
		}
	else
		{
		secondaryStore = EMemoStorePhoneMemory;
		}
	
	// If there is some secondary storage place to check
	if( memoStore != secondaryStore )
		{
		// Temporarily change the storage place (needed by MemoStoreDirectoryL)
		VRUtils::SetSettingValueL( KVRMemoStore, secondaryStore );
		
		// Retrieve the path and add it to the filename
		VRUtils::MemoStoreDirectoryL( path );
		    
	    fileName = aName;
	    fileName.Insert( 0, path );
	    
	    // Change back to the original 
	    VRUtils::SetSettingValueL( KVRMemoStore, memoStore );
		
	    if ( CheckFileExistence( aFs, fileName ) )
	    	{
	    	return ETrue;
	    	}
		}

// for multiple drives  
#else
	 TFileName path;
	 TFileName fileName(aName);

     TDriveList allDrives;
     TInt allDriveCount( 0 );

     User::LeaveIfError(DriveInfo::GetUserVisibleDrives( aFs, allDrives, allDriveCount ));

     TInt max( allDrives.Length());
     for ( TInt i = 0; i < max; i++ )
       {
  	       if ( allDrives[ i ] )
  	       	{
			    TUint status( 0 );  
			    VRUtils::GetDriveInfo( i, status);

			    if( status & DriveInfo::EDriveRemote )	
                    {
                    continue;
                    }
				
                User::LeaveIfError( PathInfo::GetRootPath( path, i) );
			    path.Append( PathInfo::DigitalSoundsPath() );
	
	            // Add the path to the filename
                fileName.Insert( 0, path );
    
                if ( CheckFileExistence( aFs, fileName ) )
    	            {
    	            return ETrue;
    	            }			         			
                
                fileName.Copy(aName);
  	       	}
     	}
#endif

	return EFalse;

	}
	
	
// ---------------------------------------------------------------------------
// VRUtils::CheckFileExistence
// Checks if the filename given as a parameter exists. This function checks all
// possible extensions (in VR). Note that aName includes path but no extension.
// ---------------------------------------------------------------------------
//	
TBool VRUtils::CheckFileExistence( RFs& aFs, TFileName aName )
	{
	TFileName fileName( aName );
    
    // AMR file  
    AppendExtension( fileName, EVRFileAmr );
    AknTextUtils::LanguageSpecificNumberConversion( fileName );
    if ( BaflUtils::FileExists( aFs, fileName ) )
    	{
    	return ETrue;
    	}	  
    
    // WAV file	
   	fileName = aName;
    AppendExtension( fileName, EVRFileWav );
    AknTextUtils::LanguageSpecificNumberConversion( fileName );
    if ( BaflUtils::FileExists( aFs, fileName ) )
    	{
    	return ETrue;
    	}
    
 	 
    // AAC_LC file	
 #ifdef  __AAC_ENCODER_PLUGIN   
   	fileName = aName;
    AppendExtension( fileName, EVRFileAAC_LC );
    AknTextUtils::LanguageSpecificNumberConversion( fileName );
  
    if ( BaflUtils::FileExists( aFs, fileName ) )
    	{
    	return ETrue;
    	}
 #endif
 
    return EFalse;			
	}


// ---------------------------------------------------------------------------
// VRUtils::AACSamplerateL
// ---------------------------------------------------------------------------
//	

EXPORT_C TInt VRUtils::AACSamplerateL()
    {

	TUint samplerate( KVRDefaultSamplerateAac );

    // Fetch bitrate from 
	TRAPD( err, samplerate = SettingValueL( KVRSamplerateAac, 
										 KVRDefaultSamplerateAac ) );

	if ( err || samplerate < 8000 || samplerate > 48000 )
		{
		return KVRDefaultSamplerateAac;
		}


	return samplerate;    
    }


// ---------------------------------------------------------------------------
// VRUtils::AACAudioModeL
// ---------------------------------------------------------------------------
//	

EXPORT_C TInt VRUtils::AACAudioModeL()
    {

	TUint mode( KVRDefaultStereoMonoFlag );

    // Fetch audiomode from 
	TRAPD( err, mode = SettingValueL( KVRStereoMonoFlag, 
										 KVRDefaultStereoMonoFlag ) );

	if ( err || (mode != 1 && mode != 2))
		{
		return KVRDefaultStereoMonoFlag;
		}


	return mode;    
    }

// ---------------------------------------------------------------------------
// DriveValid checks the drive is valid or not
// ---------------------------------------------------------------------------
//
EXPORT_C TBool VRUtils::DriveValid( const TInt aDrive )
    {
    TUint status( 0 );
    TBool flag( ETrue );
    TInt err = VRUtils::GetDriveInfo( aDrive, status );
    if ( err != KErrNone )
    	{ flag = EFalse; }
    else 
    	{
    	if ( !(status & DriveInfo::EDrivePresent) ||
    			(status & DriveInfo::EDriveLocked) ||
    			(status & DriveInfo::EDriveReadOnly) ||
    			(status & DriveInfo::EDriveCorrupt) ||
    			(status & DriveInfo::EDriveInUse) )
    		{ flag = EFalse; }
    	}
    return flag;
    
    }

EXPORT_C TBool VRUtils::MultipleMassStorageAvailable()
	{
	TBool flag( ETrue );
	TInt driveDefaultMassStorage(0);
	TInt driveRemovableMassStorage(0);
	TInt defaultStorageErr = DriveInfo::GetDefaultDrive(
			DriveInfo::EDefaultMassStorage, driveDefaultMassStorage );
	TInt removableStorageErr = DriveInfo::GetDefaultDrive(
			DriveInfo::EDefaultRemovableMassStorage,    driveRemovableMassStorage );
	if ( (defaultStorageErr) || (removableStorageErr) ||
			( driveDefaultMassStorage == driveRemovableMassStorage ) ||
			!DriveValid(driveDefaultMassStorage) || !DriveValid(driveRemovableMassStorage) )
		{ flag = EFalse; }
	
	return flag;
	}

EXPORT_C TInt VRUtils::GetRemovableMassStorageL()
	{
    TInt drive(0);
    User::LeaveIfError( DriveInfo::GetDefaultDrive(
    			DriveInfo::EDefaultRemovableMassStorage , drive ) );
	return drive;
	}

#ifdef RD_MULTIPLE_DRIVE
// ---------------------------------------------------------------------------
// VRUtils::SetMemoDriveL
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void VRUtils::SetMemoDriveL( TDriveNumber aDrive )
	{
	if ( FeatureManager::FeatureSupported( KFeatureIdMmc ) )
		{
		SetSettingValueL( KVRMemoStore, aDrive );
		}
	else
		{
		__ASSERT_DEBUG( ETrue, User::Panic( KVRPanic, EPanicInvalidSetting ) );
		}
	}


// ---------------------------------------------------------------------------
// VRUtils::MemoDriveL
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt VRUtils::MemoDriveL()
	{
    TInt defaultDrive = DefaultMemoDriveL();	
	// Phone memory is the default
	if ( FeatureManager::FeatureSupported( KFeatureIdMmc ) )
		{
		TInt ret = SettingValueL( KVRMemoStore,
								defaultDrive );
		return static_cast< TInt >( ret );
		}
	else
		{
		return defaultDrive;
		}
	}


// ---------------------------------------------------------------------------
// VRUtils::DefaultMemoDriveL
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt VRUtils::DefaultMemoDriveL()
	{
    TInt drive(0);
    User::LeaveIfError( DriveInfo::GetDefaultDrive(DriveInfo::EDefaultMassStorage, drive ) );
    return drive;
	}


// ---------------------------------------------------------------------------
// VRUtils::Get drive Info
// it is for multiple drive feature
// ---------------------------------------------------------------------------
//
EXPORT_C TInt VRUtils::GetDriveInfo( TInt aDrive, TUint& aDriveInfo )
    {
	RFs& iFs = CCoeEnv::Static()->FsSession() ;
	TInt err = DriveInfo::GetDriveStatus( iFs, aDrive, aDriveInfo ); 
	return err;
    }
#endif


//  End of File
