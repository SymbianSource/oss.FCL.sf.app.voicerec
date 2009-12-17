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






// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "UI_NewFileServiceTest.h"

#include <coeaui.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUI_NewFileServiceTest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CUI_NewFileServiceTest::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CUI_NewFileServiceTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CUI_NewFileServiceTest::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
      
        // test cases for the header "NewFileServiceClient.h"
        ENTRY( "CNewFileServiceClientCreate", CUI_NewFileServiceTest::CNewFileServiceClientCreate ),
        ENTRY( "NewFileL", CUI_NewFileServiceTest::NewFileL ),
        ENTRY( "NewFileByUid", CUI_NewFileServiceTest::NewFileByUid ),
        ENTRY( "NewFileByRFile", CUI_NewFileServiceTest::NewFileByRFile ),
        ENTRY( "NewFileByRFileAndUid", CUI_NewFileServiceTest::NewFileByRFileAndUid ),
        // test cases for the header "CNewFileServiceBase.h"
     
        ENTRY( "CreateCDummyNewFileServiceBase", CUI_NewFileServiceTest::CreateCDummyNewFileServiceBase ),
        ENTRY( "DestroyCDummyNewFileServiceBase", CUI_NewFileServiceTest::DestroyCDummyNewFileServiceBase ),
        ENTRY( "CDummyNewFileServiceBaseGetErrorCode", CUI_NewFileServiceTest::CDummyNewFileServiceBaseGetErrorCode ),
        ENTRY( "CDummyNewFileServiceBaseSetErrorCode", CUI_NewFileServiceTest::CDummyNewFileServiceBaseSetErrorCode ),
        ENTRY( "CDummyNewFileServiceBaseServiceL", CUI_NewFileServiceTest::CDummyNewFileServiceBaseServiceL ),
        ENTRY( "CDummyNewFileServiceBaseHandleCompletedNewServiceL", CUI_NewFileServiceTest::CDummyNewFileServiceBaseHandleCompletedNewServiceL ),
        ENTRY( "CDummyNewFileServiceBaseGenericParams", CUI_NewFileServiceTest::CDummyNewFileServiceBaseGenericParams )
  
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }


// -----------------------------------------------------------------------------
// CUI_NewFileServiceTest::CNewFileServiceClientCreate
// NewClientL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUI_NewFileServiceTest::CNewFileServiceClientCreate( CStifItemParser& /*aItem*/ )
    {
    TInt err = KErrNone;
    iLog->Log(_L("CUI_NewFileServiceTest::CNewFileServiceClientCreate begin:"));
    
    TRAP(err,iNewFileServiceClient=NewFileServiceFactory::NewClientL());
   
    iLog->Log(_L("CUI_NewFileServiceTest::CNewFileServiceClientCreate returned: %d"), err);
    return err;

    }
	

// -----------------------------------------------------------------------------
// CUI_NewFileServiceTest::NewFileL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------

TInt CUI_NewFileServiceTest::NewFileL( CStifItemParser& aItem )
    {
    TInt err = KErrNone;
    TPtrC string;
    iLog->Log(_L("CUI_NewFileServiceTest::NewFileL begin:"));
    
    CDesCArray* files = new (ELeave)CDesCArrayFlat(1);
    
    CleanupStack::PushL(files);
    CAiwGenericParamList* iCAiwGenericParamList=CAiwGenericParamList::NewL();
    
   
    while ( aItem.GetNextString( string ) == KErrNone )
    	{
		TBuf<120> KFrom;
		KFrom.Append(KNewFileServiceTestDataPath);
		KFrom.Append(string);
	    files->AppendL(KFrom);
	    iNewFileServiceClient=NewFileServiceFactory::NewClientL();
	    
	    TRAP(err,iNewFileServiceClient->NewFileL(*files,iCAiwGenericParamList,ENewFileServiceAudio,ETrue));
	    
	    
    	}
    CleanupStack::PopAndDestroy(files);
 
    iLog->Log(_L("CUI_NewFileServiceTest::NewFileL returned: %d"), err);
    return err;

    }

// -----------------------------------------------------------------------------
// CUI_NewFileServiceTest::NewFileByUid
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------

TInt CUI_NewFileServiceTest::NewFileByUid( CStifItemParser& aItem )
    {
    TInt err = KErrNone;
    TPtrC string;
    iLog->Log(_L("CUI_NewFileServiceTest::NewFileByUid begin:"));
    TUid uid = TUid::Uid(0x100058CA);//0x101F87A3 0x101F87A4  0x100058CA 
    CDesCArray* files = new (ELeave)CDesCArrayFlat(1);
    CleanupStack::PushL(files);
    CAiwGenericParamList* iCAiwGenericParamList=CAiwGenericParamList::NewL();
    
   
    while ( aItem.GetNextString( string ) == KErrNone )
    	{
		TBuf<120> KFrom;
		KFrom.Append(KNewFileServiceTestDataPath);
		KFrom.Append(string);
	    files->AppendL(KFrom);
	    iNewFileServiceClient=NewFileServiceFactory::NewClientL();
	    
	    TRAP(err,iNewFileServiceClient->NewFileL(uid,*files,iCAiwGenericParamList,ENewFileServiceAudio,ETrue));
	    
    
    	}
    CleanupStack::PopAndDestroy(files);
 
    iLog->Log(_L("CUI_NewFileServiceTest::NewFileByUid returned: %d"), err);
    return err;

    }


// -----------------------------------------------------------------------------
// CUI_NewFileServiceTest::NewFileByRFile
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------

TInt CUI_NewFileServiceTest::NewFileByRFile( CStifItemParser& aItem )
    {
    TInt err = KErrNone;
    TPtrC string;
    iLog->Log(_L("CUI_NewFileServiceTest::NewFileByRFile begin:"));
    
    CAiwGenericParamList* iCAiwGenericParamList=CAiwGenericParamList::NewL();
      
    while ( aItem.GetNextString( string ) == KErrNone )
    	{
		TBuf<120> KFrom;
		KFrom.Append(KNewFileServiceTestDataPath);
		KFrom.Append(string);
	   
	    RFile file;
	    RFs fs;
	    fs.Connect();
	    file.Open(fs,KFrom,EFileShareAny|EFileWrite);
	    iNewFileServiceClient=NewFileServiceFactory::NewClientL();
	    
	    TRAP(err,iNewFileServiceClient->NewFileL(file,iCAiwGenericParamList,ENewFileServiceAudio));
	    file.Close();
	    fs.Close();
    
    	}
    
    iLog->Log(_L("CUI_NewFileServiceTest::NewFileByRFile returned: %d"), err);
     return err;

    }

// -----------------------------------------------------------------------------
// CUI_NewFileServiceTest::NewFileByRFileAndUid
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------

TInt CUI_NewFileServiceTest::NewFileByRFileAndUid( CStifItemParser& aItem )
    {
    TInt err = KErrNone;
    TPtrC string;
    iLog->Log(_L("CUI_NewFileServiceTest::NewFileByRFileAndUid begin:"));
    TUid uid = TUid::Uid(0x100058CA); 
  
    CAiwGenericParamList* iCAiwGenericParamList=CAiwGenericParamList::NewL();
    
   
    while ( aItem.GetNextString( string ) == KErrNone )
    	{
		TBuf<120> KFrom;
		KFrom.Append(KNewFileServiceTestDataPath);
		KFrom.Append(string);
	   
	    RFile file;
	    RFs fs;
	    fs.Connect();
	    file.Open(fs,KFrom,EFileShareAny|EFileWrite);
	    iNewFileServiceClient=NewFileServiceFactory::NewClientL();
	    
	    TRAP(err,iNewFileServiceClient->NewFileL(uid,file,iCAiwGenericParamList,ENewFileServiceAudio));
	    file.Close();
	    fs.Close();
    
    	}
  
 
     iLog->Log(_L("CUI_NewFileServiceTest::NewFileByRFileAndUid returned: %d"), err);
     return err;

    }
// -----------------------------------------------------------------------------
// CUI_NewFileServiceTest::CreateCDummyNewFileServiceBase
// Create CDummyNewFileServiceBase
// (other items were commented in a header).
// -----------------------------------------------------------------------------

TInt CUI_NewFileServiceTest::CreateCDummyNewFileServiceBase( CStifItemParser& /*aItem*/ )
    {
    TInt err = KErrNone;
    iLog->Log(_L("CUI_NewFileServiceTest::CreateCDummyNewFileServiceBase begin:"));
    
	iDummyNewFileServiceBase = new (ELeave)CDummyNewFileServiceBase();
    
	if ( iDummyNewFileServiceBase == NULL )
		
    	{
    	  err = KErrNotFound;
    	  iLog->Log(_L("CUI_NewFileServiceTest::CreateCDummyNewFileServiceBase returned: %d"), err);
    	}
	
    return err;
    
    }
// -----------------------------------------------------------------------------
// CUI_NewFileServiceTest::DestroyCDummyNewFileServiceBase
// Destroy CDummyNewFileServiceBase
// (other items were commented in a header).
// -----------------------------------------------------------------------------

TInt CUI_NewFileServiceTest::DestroyCDummyNewFileServiceBase( CStifItemParser& /*aItem*/ )
    {
    TInt err = KErrNone;
    iLog->Log(_L("CUI_NewFileServiceTest::DestroyCDummyNewFileServiceBase begin:"));
    if ( iDummyNewFileServiceBase != NULL )
    	{
	    delete iDummyNewFileServiceBase;
	    iDummyNewFileServiceBase = NULL;
    	}
    iLog->Log(_L("CUI_NewFileServiceTest::DestroyCDummyNewFileServiceBase returned: %d"), err);
    return err;
    }
// -----------------------------------------------------------------------------
// CUI_NewFileServiceTest::CDummyNewFileServiceBaseGetErrorCode
// CDummyNewFileServiceBase GetErrorCode
// (other items were commented in a header).
// -----------------------------------------------------------------------------

TInt CUI_NewFileServiceTest::CDummyNewFileServiceBaseGetErrorCode( CStifItemParser& /*aItem*/ )
    {
    TInt err = KErrNone;
    TInt errcode = 0;
    iLog->Log(_L("CUI_NewFileServiceTest::CDummyNewFileServiceBaseGetErrorCode begin:"));
    
	errcode = iDummyNewFileServiceBase->GetErrorCode();
	if ( errcode > 0 )
    	{
    	  err = KErrNotFound;
    	  iLog->Log(_L("CUI_NewFileServiceTest::CDummyNewFileServiceBaseGetErrorCode returned: %d"), err);
    	}
	
    return err;
    }
// -----------------------------------------------------------------------------
// CUI_NewFileServiceTest::CDummyNewFileServiceBaseSetErrorCode
// CDummyNewFileServiceBase SetErrorCode 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TInt CUI_NewFileServiceTest::CDummyNewFileServiceBaseSetErrorCode( CStifItemParser& aItem )
    {
    TInt err = KErrNone;
    TInt errcode;
    TInt compare;
    iLog->Log(_L("CUI_NewFileServiceTest::CDummyNewFileServiceBaseSetErrorCode begin:"));
	 while ( aItem.GetNextInt(errcode) == KErrNone )
	        {
	        iDummyNewFileServiceBase->SetErrorCode(errcode);
	        }
	 
	 compare = iDummyNewFileServiceBase->GetErrorCode();
	 
	if ( compare != errcode )
		{
		err = KErrNotFound;
		iLog->Log(_L("CUI_NewFileServiceTest::CDummyNewFileServiceBaseSetErrorCode returned: %d"), err);
		}
	
    return err;
    }

 
// -----------------------------------------------------------------------------
// CUI_NewFileServiceTest::CDummyNewFileServiceBaseServiceL
// CDummyNewFileServiceBase ServiceL
// (other items were commented in a header).
// -----------------------------------------------------------------------------

TInt CUI_NewFileServiceTest::CDummyNewFileServiceBaseServiceL( CStifItemParser& /*aItem*/ )
    {
	    TInt err = KErrNone;
	    iLog->Log(_L("CUI_NewFileServiceTest::CDummyNewFileServiceBaseServiceL begin:"));
//	    RMessage2* msg = new(ELeave)RMessage2();
//		HBufC* buf = HBufC::NewLC(100);
//		 buf->Des().Append(_L("aa"));
//	    TPtr ptr = buf->Des();
//	    ptr.Copy(_L("aa"));
//	    message->ReadL(0,ptr);
	    RMessage2* msg = new(ELeave)RMessage2();
	    HBufC* msg1 = HBufC::New( 20 );
	    TUint16 msgContent = ENewServiceIndexFileName; //(TUint8)3;
	    msg1->Des().Copy( &msgContent );
	    TPtr msgPtr1 = msg1->Des();
	    msg->ReadL( msgContent,msgPtr1 );
	    
		iDummyNewFileServiceBase->ServiceLtest(*msg);
		CleanupStack::PopAndDestroy();   // msg1
		//delete 	message;
		//delete buf;
	    iLog->Log(_L("CUI_NewFileServiceTest::CDummyNewFileServiceBaseServiceL returned: %d"), err);
    	
	
    return err;
    }

// -----------------------------------------------------------------------------
// CUI_NewFileServiceTest::CDummyNewFileServiceBaseHandleCompletedNewServiceL
// CDummyNewFileServiceBase HandleCompletedNewServiceL
// (other items were commented in a header).
// -----------------------------------------------------------------------------

TInt CUI_NewFileServiceTest::CDummyNewFileServiceBaseHandleCompletedNewServiceL( CStifItemParser& /*aItem*/ )
    {
     TInt err = KErrNone;
	 TBool tbool = ETrue;
	 iLog->Log(_L("CUI_NewFileServiceTest::CDummyNewFileServiceBaseHandleCompletedNewServiceL begin:"));
	
	 iDummyNewFileServiceBase->HandleCompletedNewServiceLtest(tbool);
	
	 iLog->Log(_L("CUI_NewFileServiceTest::CDummyNewFileServiceBaseHandleCompletedNewServiceL returned: %d"), err);
    	
     return err;
    }

// -----------------------------------------------------------------------------
// CUI_NewFileServiceTest::CDummyNewFileServiceBaseGenericParams
// CDummyNewFileServiceBase GenericParams
// (other items were commented in a header).
// -----------------------------------------------------------------------------

TInt CUI_NewFileServiceTest::CDummyNewFileServiceBaseGenericParams( CStifItemParser& /*aItem*/ )
    {
    TInt err = KErrNone;
    iLog->Log(_L("CUI_NewFileServiceTest::CDummyNewFileServiceBaseGenericParams begin:"));
    
    iDummyNewFileServiceBase->GenericParamstest();
	 
    iLog->Log(_L("CUI_NewFileServiceTest::CDummyNewFileServiceBaseGenericParams  returned: %d"), err);
	
    return err;
    }
