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


#ifndef __CVRMEMO_H__
#define __CVRMEMO_H__

// INCLUDES
#include <f32file.h>
#include "voicerecorder.hrh"
#include "VRConsts.h"

// FORWARD DECLARATIONS
class CAknWaitDialog;
class CAknGlobalNote;

// CLASS DEFINITION
/**
* This class represents a voice memo. It is able to create new files,
* rename and delete existing files, and to save them to permanent storage.
*/
NONSHARABLE_CLASS( CVRMemo )
	: public CBase
	{

    public: // enumerations

        /*
        * File format of voice recorder
        * Possible values = 0 (AMR).
        */
        enum TVRFileFormat
	        {
	        EVRFormatAMR = 0	        
	        };

	public: // constructors and destructor

		/**
		* Default constructor.
		*/
		CVRMemo();

		/**
		* Destructor.
		*/
		~CVRMemo();

		/**
		* 2nd phase constructor.
        * @param aDefaultAudioFormat Default audio file format of VR
        *        Is really defined as TVRFileFormat, but for legacy reasons
        *        not used that way yet.
		*/
		void ConstructL(TInt aDefaultAudioFormat);
	
	public: // new methods

		/**
		* Changes the file name of this memo object.
		* @param aFilename New file name.
		*/
		void SetName( const TDesC& aFilename );

		/**
		* Sets the file name to a non-existent, new, name.
		* @param aEmbedded is true if creating temporary name for embedded
		* 				recording (different path)
		*/
		void SetTemporaryNameL( TBool aEmbedded = EFalse );

		/**
		* Shows a confirmation query and deletes the memo file
		* if user accepts.
		* @return ETrue if deletion was confirmed.
		*/
		TBool QueryAndDeleteL();

		/**
		* Deletes the current memo file.
		* The memo file name is set to KNullDesC
		*/
		void DeleteL();

		/**
		* Queries the user for a new name for the current memo file.
		* @return ETrue if rename was confirmed.
		*/
		TBool QueryAndRenameL();

		/**
		* Saves this memo to permanent storage. Removes the original
		* copy from RAM unless otherwise requested.
		* @param aProduceCopy Specify ETrue if you want to leave the
		*                     original memo file intact.
		*/
		void SavePermanentlyL( CAknGlobalNote* aWaitNote, TInt& aNoteId,
							   const TDesC& aLabel, TBool aProduceCopy = EFalse );

		/**
		* Checks wether the memo file represented by this object exists or not
		* @return ETrue if the file exists
		*/ 
		TBool IsValid() const;

		/**
		* Re-reads the file modification timestamp.
		*/
		void UpdateModifiedDate();

		/**
		* Returns the duration of the memo in microseconds.
		* @return Duration
		*/
		inline const TTimeIntervalMicroSeconds& Duration() const;

		/**
		* Sets the duration of the memo in microseconds.
		* This is for data storage only (does not affect playback).
		* @param aDuration The new duration.
		*/
		inline void SetDuration( const TTimeIntervalMicroSeconds& aDuration );

		/**
		* Returns the current playing position of the memo in microseconds.
		* @return Playing position
		*/
		inline const TTimeIntervalMicroSeconds& Position() const;

		/**
		* Sets the current playing position of the memo in microseconds.
		* This is for data storage only (does not affect playback).
		* @param aPosition The new position
		*/
		inline void SetPosition( const TTimeIntervalMicroSeconds& aPosition );

		/**
		* Returns the maximum duration of the memo in microseconds.
		* @return Maximum duration (for recording)
		*/
		const TTimeIntervalMicroSeconds& MaxDuration();

		/**
		* Returns the complete file name (path and everything).
		* @return Reference to the memo name.
		*/
		inline const TDesC& Filename() const;

		/**
		* Returns the name portion of the complete file name.
		* @return Reference to the memo name.
		*/
		inline const TPtrC& Name() const;

		/**
		* Returns the creation date of this file.
		* @return Reference to the date.
		*/
		inline const TTime& DateCreated() const;

		void SetSavingLocationL( const TDesC& aPath );
		
		/**
		* Returns the handle of the memo file.
		* You may have to check the SubSessionHandle() to
		* now if the file has been opened
		* @return Reference to the file handle
		*/
		inline RFile& File();

		/**
		* Returns the active quality setting.
		* @return Quality setting of the active memo
		*/
		inline TVRQuality Quality() const;

		/**
		* Used to set the target file handle.
		* @param aFile File handle
		* @param aEmbedded True if recording for external application and
		*		 handle must be kept open all the time
		*/
		void SetFileHandle( RFile& aFile, const TBool aEmbedded = EFalse );

		/**
		* Check if quality setting has changed and memo needs to be renamed.
		* Should be called at least before starting to record first memo
		* after application launch.
		*/
		void CheckQualityL();
		
		/**
		* Sets the attribute iIsRecorded ETrue if recording is started. EFalse
		* if memo is attached but recording isn't started.
		* @param aRecorded ETrue when recording is started
		*/
		void SetRecorded( TBool aRecorded );
		
		/**
		* Returns the current memo store.
		* .
		*/
		inline TVRMemoStore MemoStore() const;

		/**
		* Returns the creation date of this file.
		* @return Reference to the date.
		*/
		inline TInt StorageDrive() const;
		
		
		
	private:
		/**
		* Checks if iFile has an empty file open
		* and deletes it, leaving iFile closed
		* @return returns ETrue if file was deleted
		*/
		TBool DeleteEmptyFile();
		
		/**
		* Checks if recording to the currently open memo is started
		* Returns ETrue if recording is started, EFalse if not
		*/
		TBool IsRecorded() const;

	private: // data

		/**
		* Length of this memo in microseconds.
		*/
		TTimeIntervalMicroSeconds iDuration;

		/**
		* Length of this memo in microseconds.
		*/
		TTimeIntervalMicroSeconds iPosition;

		/**
		* Maximum length for all memos.
		*/
		TTimeIntervalMicroSeconds iMaxDuration;

		/**
		* The final path and file name of the memo.
		* Memo is moved to this path from temp drive when 
		* when it's saved
		*/
		TFileName iFilename;

		/**
		* Handle to actual file system file
		*/
		RFile iFile;

		/**
		* The name of this memo (the file name minus extension).
		* This is a ptr descriptor to the iFileName;
		*/
		TPtrC iNamePtr;

		/**
		* The creation time of this memo.
		*/
		TTime iDateCreated;

		/**
		* A reference to a connected file server session.
		* Used as a short cut. Not owned.
		*/
		RFs iFs;

		/**
		* Pointer to a file manager object. Owned.
		*/
		CFileMan* iFileMan;

		/**
		* The directory where memos will be saved.
		*/
		TFileName iSavingLocation;

		/**
		* Default audio format for recording audio clip
		*/
		TInt iVRAudioFormat;
        
		/**
		* Are we in embedded mode
		*/        
		TBool iEmbedded;
		
		/**
		* Is the RFile received from outside or should we close it
		*/        
		TBool iExternalFileHandle;		

        /**
        * Cached quality setting
        */
        TVRQuality iQuality;
		
		/**
		* Cached storage place setting
		*/
		TVRMemoStore iStoragePlace;

		/**
		* Cached storage drive setting
		*/
		TInt iStorageDrive;
		
		/**
		* Last time that the estimate was updated
		*/
		TTime iLastEstimate;
		
		/**
		* Tells if recording of current open memo is started. 
		* EFalse: not recording started
		* ETrue: recording is started
		*/
		TBool iIsRecorded;
	};

#include "CVRMemo.inl"

#endif	// __CVRMEMO_H__
