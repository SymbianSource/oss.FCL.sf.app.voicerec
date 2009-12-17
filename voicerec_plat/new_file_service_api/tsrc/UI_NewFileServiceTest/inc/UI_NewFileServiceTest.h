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
* Description:    UI_NewFileServiceTest test class for STIF Test Framework TestScripter.
*
*/






#ifndef UI_NewFileServiceTest_H
#define UI_NewFileServiceTest_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>

#include <CNewFileServiceBase.h>
#include <NewFileServiceClient.h>
#include <AiwGenericParam.h>
#include <coeaui.h>
#include "CDummyNewFileServiceBase.h"
// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
// Logging path
_LIT( KUI_NewFileServiceTestLogPath, "\\logs\\testframework\\UI_NewFileServiceTest\\" );
//Data path
_LIT( KNewFileServiceTestDataPath, "c:\\testing\\data\\" ); 
// Log file
_LIT( KUI_NewFileServiceTestLogFile, "UI_NewFileServiceTest.txt" ); 
_LIT( KUI_NewFileServiceTestLogFileWithTitle, "UI_NewFileServiceTest_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CUI_NewFileServiceTest;
class CCoeControl;
class CNewFileServiceClient;
class NewFileServiceFactory;
class CNewFileServiceBase;
class CEdwinTestControl;
class CListBoxTestControl;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CUI_NewFileServiceTest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CUI_NewFileServiceTest) : public CScriptBase
    {
    public:  // Constructors and destructor

    	enum TIpcNewFileArgumentIndices
    	    {
    	    ENewServiceIndexParams = 0,
    	    ENewServiceIndexFsHandle,
    	    ENewServiceIndexFileHandle,
    	    ENewServiceIndexFileName
    	    };
        /**
        * Two-phased constructor.
        */
        static CUI_NewFileServiceTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CUI_NewFileServiceTest();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );



    private:

        /**
        * C++ default constructor.
        */
        CUI_NewFileServiceTest( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

        /**
        * Test methods are listed below. 
        */

        /**
        * Example test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
     
        TInt CNewFileServiceClientCreate( CStifItemParser& aItem );
        TInt NewFileL( CStifItemParser& aItem );
        TInt NewFileByUid( CStifItemParser& aItem );
        TInt NewFileByRFile( CStifItemParser& aItem );
        TInt NewFileByRFileAndUid( CStifItemParser& aItem );
        
        TInt CreateCDummyNewFileServiceBase( CStifItemParser& /*aItem*/ );
        TInt DestroyCDummyNewFileServiceBase( CStifItemParser& /*aItem*/ );
        TInt CDummyNewFileServiceBaseGetErrorCode( CStifItemParser& /*aItem*/ );
        TInt CDummyNewFileServiceBaseSetErrorCode( CStifItemParser& aItem );
        TInt CDummyNewFileServiceBaseServiceL( CStifItemParser& /*aItem*/ );
        TInt CDummyNewFileServiceBaseHandleCompletedNewServiceL( CStifItemParser& /*aItem*/ );
        TInt CDummyNewFileServiceBaseGenericParams( CStifItemParser& /*aItem*/ );
        
        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove

    
    private:    // Friend classes
        //?friend_class_declaration;
    	
    	CNewFileServiceClient* iNewFileServiceClient;
    	CNewFileServiceBase* iNewFileServiceBase;
    	CDummyNewFileServiceBase* iDummyNewFileServiceBase;
    };

#endif      // UI_NewFileServiceTest_H

// End of File
