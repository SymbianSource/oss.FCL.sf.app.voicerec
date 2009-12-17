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
*     Declares the document class for this application.
*
*/


#include "CVRDocument.h"
#include "CVRAppUi.h"

CVRDocument::~CVRDocument()
    {
    }

CVRDocument* CVRDocument::NewL( CEikApplication& aApp )
    {
    CVRDocument* self = new( ELeave ) CVRDocument( aApp );
    return self;
    }

CEikAppUi* CVRDocument::CreateAppUiL()
    {
    return new( ELeave ) CVRAppUi;
    }
