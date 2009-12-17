/*
* Copyright (c) 2002 - 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Sample file of  newfile server
*
*/






#ifndef CDUMMYNEWFILESERVICEBASE_H_
#define CDUMMYNEWFILESERVICEBASE_H_
#include <CNewfileServiceBase.h>

class CDummyNewFileServiceBase : public CNewFileServiceBase 
    {
public:  // Constructors and destructor
	CDummyNewFileServiceBase();  
    ~CDummyNewFileServiceBase();
    void HandleNewFileL( MNewFileServiceObserver* aObserver,
           							  CDesCArray& aFilenameArray,
           							  TNewServiceFileType aType,
           							  TBool aMultipleFiles );
    CPolicyServer::TCustomResult SecurityCheckL(const RMessage2& aMsg, TInt& aAction, TSecurityInfo& aMissing);
    TInt CountResources();  
    void Disconnect(const RMessage2& aMessage);
    void ExtensionInterface(TUid aInterfaceId, TAny*& aImplementaion);
    void CApaAppServiceBase_Reserved1();
    void CApaAppServiceBase_Reserved2();
    void HandleNewFileL( MNewFileServiceObserver* aObserver,
           							  RFile& aFile,
           				  TNewServiceFileType aType );
   	void ServiceCompleteL();
   	void ServiceLtest(const RMessage2& aMessage);
   	void HandleCompletedNewServiceLtest(TBool aBool);
   	CAiwGenericParamList* GenericParamstest();

    };
#endif /*CDUMMYNEWFILESERVICEBASE_H_*/
