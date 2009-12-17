/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Inline versions of some member functions
*
*/


inline const TTimeIntervalMicroSeconds& CVRMemo::Duration() const
	{
	return iDuration;
	}

inline void CVRMemo::SetDuration( const TTimeIntervalMicroSeconds& aDuration )
	{
	iDuration = aDuration;
	}

inline const TTimeIntervalMicroSeconds& CVRMemo::Position() const
	{
	return iPosition;
	}

inline void CVRMemo::SetPosition( const TTimeIntervalMicroSeconds& aPosition )
	{
	iPosition = aPosition;
	}


inline const TDesC& CVRMemo::Filename() const
	{
	return iFilename;
	}

inline const TTime& CVRMemo::DateCreated() const
	{
	return iDateCreated;
	}

inline const TPtrC& CVRMemo::Name() const
	{
	return iNamePtr;
	}

inline RFile& CVRMemo::File()
	{
	return iFile;
	}

inline TVRQuality CVRMemo::Quality() const
	{
	return iQuality;
	}

	
inline TVRMemoStore CVRMemo::MemoStore() const
   {
   return iStoragePlace;
   }

inline TInt CVRMemo::StorageDrive() const
  {
  return iStorageDrive;
  }

// End of file
