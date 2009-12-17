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
* Description:    sample file of Newfile server
*
*/






#include "CDummyNewFileServiceBase.h"

CDummyNewFileServiceBase::CDummyNewFileServiceBase():CNewFileServiceBase()
	{
	}
CDummyNewFileServiceBase::~CDummyNewFileServiceBase()
	{
	}
void CDummyNewFileServiceBase::HandleNewFileL( MNewFileServiceObserver* /*aObserver*/,
          							  CDesCArray& /*aFilenameArray*/,
          							  TNewServiceFileType /*aType*/,
          							  TBool /*aMultipleFiles*/ )
	{}
         
void CDummyNewFileServiceBase::HandleNewFileL( MNewFileServiceObserver* /*aObserver*/,
          							  RFile& /*aFile*/,
          				  TNewServiceFileType /*aType*/ )
	{}

void CDummyNewFileServiceBase::ServiceCompleteL()
	{
	GenericParams();
	}
CPolicyServer::TCustomResult CDummyNewFileServiceBase::SecurityCheckL(const RMessage2& /*aMsg*/, TInt& /*aAction*/, TSecurityInfo& /*aMissing*/)
	{return CPolicyServer::EPass;}
TInt CDummyNewFileServiceBase::CountResources()
	{return 0;}
void CDummyNewFileServiceBase::Disconnect(const RMessage2& /*aMessage*/)
	{}
void CDummyNewFileServiceBase::ExtensionInterface(TUid /*aInterfaceId*/, TAny*& /*aImplementaion*/)
	{}
void CDummyNewFileServiceBase::CApaAppServiceBase_Reserved1()
	{}
void CDummyNewFileServiceBase::CApaAppServiceBase_Reserved2()
	{}

void CDummyNewFileServiceBase::ServiceLtest(const RMessage2& aMessage)
	{
	ServiceL( aMessage );
	}

void CDummyNewFileServiceBase::HandleCompletedNewServiceLtest(TBool aBool)
	{
	HandleCompletedNewServiceL( aBool);
	}

CAiwGenericParamList* CDummyNewFileServiceBase::GenericParamstest()
	{
	return GenericParams();
	}

