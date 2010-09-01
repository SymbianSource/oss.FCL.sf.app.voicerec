/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*     This class is a part of the standard application framework.
*     The application gets instantiated starting from this class.
*     Provides a factory method for instantiating the document object.
*
*/


#include <eikstart.h>
#include "CVRApplication.h"
#include "CVRAppServer.h"
#include "CVRDocument.h"

TUid CVRApplication::AppDllUid() const
    {
    return KVRAppUID;
    }

CApaDocument* CVRApplication::CreateDocumentL()
    {
    return CVRDocument::NewL( *this );
    }

LOCAL_C CApaApplication* NewApplication()
	{
	return new CVRApplication;
	}

void CVRApplication::NewAppServerL( CApaAppServer*& aAppServer )
	{
	aAppServer = new( ELeave ) CVRAppServer;
	}

GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication( NewApplication );
	}

