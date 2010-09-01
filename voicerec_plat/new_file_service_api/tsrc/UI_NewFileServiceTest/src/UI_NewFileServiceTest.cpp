/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:     UI_NewFileServiceTest test class for STIF Test Framework TestScripter.
*
*/






// INCLUDE FILES
#include <Stiftestinterface.h>
#include "UI_NewFileServiceTest.h"
#include <SettingServerClient.h>



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUI_NewFileServiceTest::CUI_NewFileServiceTest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUI_NewFileServiceTest::CUI_NewFileServiceTest( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {
    }

// -----------------------------------------------------------------------------
// CUI_NewFileServiceTest::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUI_NewFileServiceTest::ConstructL()
    {
    //Read logger settings to check whether test case name is to be
    //appended to log file name.
    RSettingServer settingServer;
    TInt ret = settingServer.Connect();
    if(ret != KErrNone)
        {
        User::Leave(ret);
        }
    // Struct to StifLogger settigs.
    TLoggerSettings loggerSettings; 
    // Parse StifLogger defaults from STIF initialization file.
    ret = settingServer.GetLoggerSettings(loggerSettings);
    if(ret != KErrNone)
        {
        User::Leave(ret);
        } 
    // Close Setting server session
    settingServer.Close();

    TFileName logFileName;
    
    if(loggerSettings.iAddTestCaseTitle)
        {
        TName title;
        TestModuleIf().GetTestCaseTitleL(title);
        logFileName.Format(KUI_NewFileServiceTestLogFileWithTitle, &title);
        }
    else
        {
        logFileName.Copy(KUI_NewFileServiceTestLogFile);
        }

    iLog = CStifLogger::NewL( KUI_NewFileServiceTestLogPath, 
                          logFileName,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );

    }

// -----------------------------------------------------------------------------
// CUI_NewFileServiceTest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUI_NewFileServiceTest* CUI_NewFileServiceTest::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CUI_NewFileServiceTest* self = new (ELeave) CUI_NewFileServiceTest( aTestModuleIf );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

// Destructor
CUI_NewFileServiceTest::~CUI_NewFileServiceTest()
    { 

    // Delete resources allocated from test methods
    Delete();
    
    if ( iNewFileServiceClient != NULL )
    	{
    	delete iNewFileServiceClient;
    	iNewFileServiceClient=NULL;
    	}
    
    // Delete logger
    delete iLog; 

    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL( 
    CTestModuleIf& aTestModuleIf ) // Backpointer to STIF Test Framework
    {

    return ( CScriptBase* ) CUI_NewFileServiceTest::NewL( aTestModuleIf );

    }


//  End of File
