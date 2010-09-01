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
* Description:  Empty implementation for discovery class
*
*/



// INCLUDE FILES
#include    "CNewFileServiceDiscovery.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNewFileServiceDiscovery::CNewFileServiceDiscovery
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CNewFileServiceDiscovery::CNewFileServiceDiscovery()
    {
    }

// -----------------------------------------------------------------------------
// CNewFileServiceDiscovery::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNewFileServiceDiscovery::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CNewFileServiceDiscovery::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNewFileServiceDiscovery* CNewFileServiceDiscovery::NewL()
    {
    CNewFileServiceDiscovery* self = new( ELeave ) CNewFileServiceDiscovery;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CNewFileServiceDiscovery::~CNewFileServiceDiscovery()
    {
    
    }


//  End of File  
