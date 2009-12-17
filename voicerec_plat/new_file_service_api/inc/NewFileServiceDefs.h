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
* Description:  Definitions for the API
*
*/



#ifndef NEWSERVICEDEFS_H
#define NEWSERVICEDEFS_H

//  INCLUDES
#include <apaserverapp.h>

#define NEWSERVICEUID 0x101F8861

// DATA TYPES
enum TIpcMessageIds
	{
	ECreateNewFile = RApaAppServiceBase::KServiceCmdBase,
	ENewFileToHandle,
	EGetFilenameArrayLength,
	EGetFilenameArray
	};

enum TIpcNewFileArgumentIndices
    {
    ENewServiceIndexParams = 0,
    ENewServiceIndexFsHandle,
    ENewServiceIndexFileHandle,
    ENewServiceIndexFileName
    };

/**
*  Parameter class transferred through client-server interface.
*
*  @since Series 60 3.0
*/
class TNewServiceParamPack
	{
	public:
		TNewServiceFileType iType;
		TBool iMultipleFiles;
		TBool iReturnValue;
	};


#endif      // NEWSERVICEDEFS_H

// End of File
