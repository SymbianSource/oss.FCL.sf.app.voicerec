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


#ifndef __VRUTILS_H
#define __VRUTILS_H

// INCLUDES
#include <bldvariant.hrh>
#include <f32file.h>
#include <badesca.h>
#include "VRConsts.h"
#include "voicerecorder.hrh"

// CLASS DECLARATION

class TVRDriveInfo
    {
    public:
        TBool iDrivePresent;
        TBool iDriveLocked;
        TBool iDriveCorrupted;
		TBool iDriveReadOnly;
		TBool iDriveFull;
    };

class VRUtils
    {
    public: // New functions
        
		/**
        * Returns the currently effective path where to store voice memos
		* The directory is affected by the settings and the availability of MMC
		*
        * @return Returns a non-modifiable pointer descriptor containing the
		*         currently effective path where to store voice memos.
		          The descriptor is left on cleanup stack.
        */
		IMPORT_C static void MemoStoreDirectoryL( TDes &aPath );

        /**
        * This method sets the default memo store setting (Phone Memory / MMC)
		*
        * @param aMemory Memory to set as default store
        */
		IMPORT_C static void SetMemoStoreL( TVRMemoStore aMemory );

        /**
        * This method returns the default memo store setting
        * @return The default memo store
        */
		IMPORT_C static TVRMemoStore MemoStoreL();

		/**
		* This method returns the current status of MMC (inserted/locked/etc...)
		* @param aDriveInfo MMC status
		*/
		IMPORT_C static void GetMMCInfo( TVRDriveInfo& aDriveInfo );

		/**
		* Set the default speaker
		* @param aSpeaker ESpeakerEarPiece or ESpeakerIhf
		*/
		IMPORT_C static void SetDefaultSpeakerL( const TVRSpeaker aSpeaker);

		/**
		* This method returns the current speaker
		* @return Default speaker, either ESpeakerEarPiece or ESpeakerIhf
		*/
		IMPORT_C static TVRSpeaker DefaultSpeakerL();

		/**
		* Set the default volume setting for specified speaker
		* @param aSpeaker ESpeakerEarPiece or ESpeakerIhf
		* @param aVolume New TInt value to save
		*/
		IMPORT_C static void SetDefaultVolumeL( const TVRSpeaker aSpeaker, const TInt aVolume );

		/**
		* This method returns the default volume setting for specivied speaker
		* @param aSpeaker ESpeakerEarPiece or ESpeakerIhf
		* @return The retrieved volume setting value
		*/
		IMPORT_C static TInt DefaultVolumeL( const TVRSpeaker aSpeaker );


		/**
		* Set the default quality setting
		* @param aSpeaker EQualityMMSOptimized or EQualityHigh
		*/
		IMPORT_C static void SetQualityL( const TVRQuality aQuality );

		/**
		* This method returns the current quality setting
		* @return Default quality, either EQualityMMSOptimized or EQualityHigh
		*/
		IMPORT_C static TVRQuality QualityL();

		/**
		* This method shows the out-of-memory confirmation query
		* @param aMmc Shows qtn_memlo_not_enough_memory if EFalse or
		*			qtn_memlo_mmc_not_enough_memory if ETrue
		*/
		IMPORT_C static void ShowMemoryFullConfirmationQuery( TBool aMmc = EFalse );

		/**
		* This method checks if video or VOIP call is ongoing
		* @param aShowNote Show error note if ETrue
		* @return ETrue if unsupported call is ongoing, EFalse otherwise
		*/
		IMPORT_C static TBool UnsupportedCallTypeOngoing( TBool aShowNote = EFalse );
		
		/**
		* This method returns the maximum length for a memo
		* @return Maximum length
		*/
		IMPORT_C static TInt MaxLengthL();		

		/**
		* Check if variated feature is configured on
		* @param aFeature Internal feature id
		* @return ETrue if feature is enabled
		*/		
		IMPORT_C static TBool FeatureEnabled( TVRFeature aFeature );
		
		/**
		* Generate a filename that has unique body in given path.
		* @param aFs Reference to connected file server session handle
		* @param aName Path to destination folder. On return contains
		*         a unique file name with full path and extension
		* @param aType The type of file to be generated. Extension will be
		          appended according to this parameter.
		*/				
		IMPORT_C static void GenerateUniqueFilenameL( RFs& aFs, 
	                         TFileName& aName, TVRFiletype aType );
		
		/**
		* This method returns the running count of recorded memos
		* @return Count of recorded memos
		*/
		IMPORT_C static TInt MemoCount();		
		
		/**
		* This method returns the running count of recorded memos
		* @return Count of recorded memos
		*/
		IMPORT_C static void SetMemoCount( TInt aNewCount );
		
		/**
		* This method returns the bitrate that is used when recording amr clips
		* @return Bitrate
		*/
		IMPORT_C static TUint AMRBitrateL();	


		/**
		* This method returns the bitrate that is used when recording amr clips
		* @return Bitrate
		*/
		IMPORT_C static TUint AACBitrateL();	

				
		/**
		* This method returns the AAC-LC sampling rate that is used when recording AAC-LC clips
		* @return Sampleing rate
		*/		
		IMPORT_C static TInt AACSamplerateL();
		
		/**
		* This method returns the audio mode that is used when recording AAC-LC clips
		* @return audio mode
		*/		
       	IMPORT_C static TInt AACAudioModeL();

#ifdef RD_MULTIPLE_DRIVE    	
       	/**
        * This method sets the default memo store setting (to support multipledrives)
		*
        * @param aMemory Memory to set as default store
        */
		IMPORT_C static void SetMemoDriveL( TDriveNumber aDrive );
     	 
       	/**
        * This method returns default phone memory drive
		*
        * 
        */
		IMPORT_C static TInt DefaultMemoDriveL();
	
		/**
		* This method returns the current status of Drive (inserted/locked/etc...)
		* @param aDriveInfo MMC status
		*/
		IMPORT_C static void GetDriveInfo( TInt aDrive, TUint& aDriveInfo );

        /**
        * This method returns the default memo drive setting
        * @return The default memo store
        */
		IMPORT_C static TInt MemoDriveL();
#endif		
				
	private:

		/**
		* Default constructor declared as private to prohibit construction.
		*/
		VRUtils();	
		
		/**
		* Store a setting in Central Repository
		* @param aKey The Central Repository key 
		* @param aValue The value for the key
		*/
		static void SetSettingValueL( const TUint32 aKey, const TInt aValue );

		/**
		* Get a setting from Central Repository
		* @param aKey The requested Central Repository key
		* @param aDefaultValue If key was not found, this is the value used
		* @return The value of the requested key
		*/
		static TInt SettingValueL( const TUint32 aKey, const TInt aValue );
		
		/**
		* This method shows the out-of-memory confirmation query
		* @param aMmc Shows qtn_memlo_not_enough_memory if EFalse or
		*			qtn_memlo_mmc_not_enough_memory if ETrue
		*/
		static void ShowMemoryFullConfirmationQueryL( TBool aMmc = EFalse );
		
		/**
		* This method is called if video or VOIP call is ongoing and
		* information note needs to be shown
		* @param aResourceId
		*/
		static void ShowUnsupportedCallTypeNoteL( TInt aResourceId );
		
		/**
		* This method appends the requested file type extension to filename
		* @param aName Contains full file name after completion
		* @param aType File type that defines the extension to be appended
		*/
		static void AppendExtension( TFileName& aName, TVRFiletype aType );		
		
		/**
		* This method does the full check for the parameter filename existence
		* @param aFs Reference to connected file server session handle
		* @param aName Filename to check. No Path nor extension included.
		* @return ETrue if file exists, EFalse otherwise
		*/
		static TBool DoesFileExistL( RFs& aFs, TFileName aName );
		
		/**
		* This method checks if parameter filename exists or not
		* @param aFs Reference to connected file server session handle
		* @param aName Filename to check. Path included. Extension excluded.
		* @return ETrue if file exists, EFalse otherwise
		*/
		static TBool CheckFileExistence( RFs& aFs, TFileName aName );

    };

#endif      // __VRUTILS_H
            
// End of File
